#include <string>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

#include "SolLuaDataModel.h"
#include "SolLuaDocument.h"
#include "bind/bind.h"

namespace Rml::SolLua {
namespace {

class LiteralIntDefinition final : public VariableDefinition {
  public:
    LiteralIntDefinition()
        : VariableDefinition(DataVariableType::Scalar) {
    }

    bool Get(void *ptr, Variant &variant) override {
        variant = static_cast<int>(reinterpret_cast<intptr_t>(ptr));
        return true;
    }
};

DataVariable MakeLiteralIntVariable(int value) {
    static LiteralIntDefinition literal_int_definition;
    return DataVariable(&literal_int_definition, reinterpret_cast<void *>(static_cast<intptr_t>(value)));
}

}

SolLuaDataModel::SolLuaDataModel(const sol::table &model, const Rml::DataModelConstructor &constructor)
    : m_constructor(constructor),
      m_topLevelProxy{ .modelHandle = m_constructor.GetModelHandle(),
                       .objectDef = std::make_unique<SolLuaObjectDef>(model) } {
    wrapTable(m_topLevelProxy, true);
}

SolLuaDataModelTableProxy &SolLuaDataModel::topLevelProxy() {
    return m_topLevelProxy;
}

void SolLuaDataModel::wrapTable(SolLuaDataModelTableProxy &proxy, bool topLevel) {
    for (auto &[key, value] : proxy.objectDef->table()) {
        std::string skey;
        if (key.get_type() == sol::type::string) {
            skey = key.as<std::string>();
        } else if (key.get_type() == sol::type::number) {
            // Assign a pseudo-key for numeric indices
            // TODO: check if the number is an integer?
            skey = std::format("[{}]", key.as<int>() - 1); // Lua is 1-based
        } else {
            Rml::Log::Message(Log::LT_ERROR, "Data model key with type other than string or integer is unsupported");
        }

        if (value.get_type() == sol::type::table) {
            auto childProxyIt = proxy.children.emplace(
                skey,
                SolLuaDataModelTableProxy{ .modelHandle = m_topLevelProxy.modelHandle,
                                           .objectDef = std::make_unique<SolLuaObjectDef>(value.as<sol::table>()) }
            );
            assert(childProxyIt.second);
            childProxyIt.first->second.topLevelKey = proxy.topLevelKey ? proxy.topLevelKey : &childProxyIt.first->first;
            wrapTable(childProxyIt.first->second, false);
            if (topLevel && skey[0] != '[') {
                // Only bind top-level non-integer keys
                m_constructor.BindCustomDataVariable(
                    skey, Rml::DataVariable(childProxyIt.first->second.objectDef.get(), &childProxyIt.first->second)
                );
            }
        } else {
            if (value.get_type() == sol::type::function) {
                if (!topLevel) {
                    Rml::Log::Message(
                        Log::LT_WARNING, "Event callbacks are only allowed at the top level of a data model."
                    );
                    continue;
                }

                m_constructor.BindEventCallback(
                    skey,
                    [cb = sol::protected_function{ value },
                     state = sol::state_view{ proxy.objectDef->table().lua_state(
                     ) }](Rml::DataModelHandle, Rml::Event &event, const Rml::VariantList &varlist) {
                        if (cb.valid()) {
                            std::vector<sol::object> args;
                            for (const auto &variant : varlist) {
                                args.push_back(makeObjectFromVariant(&variant, state));
                            }
                            auto pfr = cb(event, sol::as_args(args));
                            if (!pfr.valid()) {
                                ErrorHandler(cb.lua_state(), std::move(pfr));
                            }
                        }
                    }
                );
            } else {
                auto it = proxy.keys.emplace(skey);
                if (topLevel && skey[0] != '[') {
                    // Only bind top-level non-integer keys
                    m_constructor.BindCustomDataVariable(
                        skey, Rml::DataVariable(proxy.objectDef.get(), const_cast<char *>(it.first->data()))
                    );
                }
            }
        }
    }
}

void SolLuaDataModel::rebindNestedTable(SolLuaDataModelTableProxy &proxy, const sol::object &key) {
    sol::table_proxy valueProxy = proxy.objectDef->table()[key];
}

/// SolLuaObjectDef
SolLuaObjectDef::SolLuaObjectDef(sol::table table)
    : VariableDefinition(DataVariableType::Scalar),
      m_table(std::move(table)) {
}

bool SolLuaObjectDef::Get(void *ptr, Rml::Variant &variant) {
    sol::object obj;
    auto *key = const_cast<const char *>(static_cast<char *>(ptr));
    if (key[0] == '[') {
        // Pseudo-key: access by index
        int idx;
        std::from_chars_result result = std::from_chars(key + 1, key + std::strlen(key) - 1, idx);
        assert(result.ec == std::errc{} && "Rml failed to sanitize user input to be well-formed");
        if (idx < 0 || idx >= m_table.size()) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
            return false;
        }
        obj = m_table[idx + 1]; // Lua is 1-based
    } else {
        obj = m_table[key];
    }

    if (obj.is<bool>()) {
        variant = obj.as<bool>();
    } else if (obj.is<std::string>()) {
        variant = obj.as<std::string>();
    } else if (obj.is<Rml::Vector2i>()) {
        variant = obj.as<Vector2i>();
    } else if (obj.is<Rml::Vector2f>()) {
        variant = obj.as<Vector2f>();
    } else if (obj.is<Rml::Colourb>()) {
        variant = obj.as<Rml::Colourb>();
    } else if (obj.is<Rml::Colourf>()) {
        variant = obj.as<Rml::Colourf>();
    } else if (obj.is<double>()) {
        variant = obj.as<double>();
    } else {
        variant = Rml::Variant{};
    }

    return true;
}

bool SolLuaObjectDef::Set(void *ptr, const Rml::Variant &variant) {
    auto *key = const_cast<const char *>(static_cast<char *>(ptr));
    if (key[0] == '[') {
        // Pseudo-key: access by index
        int idx;
        std::from_chars_result result = std::from_chars(key + 1, key + std::strlen(key) - 1, idx);
        assert(result.ec == std::errc{} && "Rml failed to sanitize user input to be well-formed");
        if (idx < 0 || idx >= m_table.size()) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
            return false;
        }
        sol::table_proxy obj = m_table[idx + 1]; // Lua is 1-based
        obj = makeObjectFromVariant(&variant, m_table.lua_state());
    } else {
        sol::table_proxy obj = m_table[key];
        obj = makeObjectFromVariant(&variant, m_table.lua_state());
    }
    return true;
}

int SolLuaObjectDef::Size(void *ptr) {
    // TODO: can size be called on non-proxy objects?
    SolLuaDataModelTableProxy &proxy = *static_cast<SolLuaDataModelTableProxy *>(ptr);
    return static_cast<int>(proxy.objectDef->table().size());
}

DataVariable SolLuaObjectDef::Child(void *ptr, const Rml::DataAddressEntry &address) {
    SolLuaDataModelTableProxy &proxy = *static_cast<SolLuaDataModelTableProxy *>(ptr);
    sol::table t = proxy.objectDef->table();

    std::string skey;
    sol::object obj;
    if (address.index != -1) {
        if (address.index < 0 || address.index >= t.size()) {
            Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
            return {};
        }
        // Access by index
        skey = std::format("[{}]", address.index);
        obj = t[address.index + 1]; // Lua is 1-based
    } else {
        if (address.name == "size") {
            return MakeLiteralIntVariable(t.size());
        }

        skey = address.name;
        obj = proxy.objectDef->table()[address.name];
    }

    if (obj.get_type() == sol::type::table) {
        auto it = proxy.children.find(skey);
        assert(it != proxy.children.end());

        // Pass proxy as ptr to be used in `Child` calls further down the chain
        return { it->second.objectDef.get(), &it->second };
    }

    auto it = proxy.keys.find(skey);
    assert(it != proxy.keys.end());
    return { proxy.objectDef.get(), const_cast<char *>(it->data()) };
}

sol::table &SolLuaObjectDef::table() {
    return m_table;
}

} // end namespace Rml::SolLua
