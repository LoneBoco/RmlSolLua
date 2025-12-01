#include <string>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

#include "SolLuaDataModel.h"
#include "SolLuaDocument.h"
#include "bind/bind.h"

namespace Rml::SolLua {
namespace {}

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
        auto skey = key.as<std::string>();

        if (value.get_type() == sol::type::table) {
            auto childProxyIt = proxy.children.emplace(
                skey,
                SolLuaDataModelTableProxy{ .modelHandle = m_topLevelProxy.modelHandle,
                                           .objectDef = std::make_unique<SolLuaObjectDef>(value.as<sol::table>()) }
            );
            assert(childProxyIt.second);
            wrapTable(childProxyIt.first->second, false);
        } else {
            if (value.get_type() == sol::type::function) {
                if (!topLevel) {
                    Rml::Log::Message(
                        Log::LT_ERROR, "Event callbacks are only allowed at the top level of a data model."
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
                m_constructor.BindCustomDataVariable(
                    skey, Rml::DataVariable(proxy.objectDef.get(), const_cast<char *>(it.first->data()))
                );
            }
        }
    }
}

/// SolLuaObjectDef
SolLuaObjectDef::SolLuaObjectDef(sol::table table)
    : VariableDefinition(DataVariableType::Scalar),
      m_table(std::move(table)) {
}

bool SolLuaObjectDef::Get(void *ptr, Rml::Variant &variant) {
    auto *key = const_cast<const char *>(static_cast<char *>(ptr));
    sol::object obj = m_table[key];

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
        variant = obj.is<double>();
    } else {
        variant = Rml::Variant{};
    }

    return true;
}

bool SolLuaObjectDef::Set(void *ptr, const Rml::Variant &variant) {
    auto *key = const_cast<const char *>(static_cast<char *>(ptr));
    sol::table_proxy obj = m_table[key];
    obj = makeObjectFromVariant(&variant, m_table.lua_state());
    return true;
}

int SolLuaObjectDef::Size(void *ptr) {
    // Non-table types are 1 entry long.
    auto object = static_cast<sol::object *>(ptr);
    if (object->get_type() != sol::type::table) {
        return 1;
    }

    auto t = object->as<sol::table>();
    return static_cast<int>(t.size());
}

DataVariable SolLuaObjectDef::Child(void *ptr, const Rml::DataAddressEntry &address) {
    __debugbreak(); // TODO: Implement Child access for SolLua objects.

    // Failure.
    return DataVariable{};
}

sol::table &SolLuaObjectDef::table() {
    return m_table;
}

} // end namespace Rml::SolLua
