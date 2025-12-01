#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

namespace Rml::SolLua {
class SolLuaObjectDef;

/// Userdata that proxies any table in the model, recursively
struct SolLuaDataModelTableProxy {
    Rml::DataModelHandle modelHandle;
    std::unique_ptr<SolLuaObjectDef> objectDef;

    // Store keys in a set to keep alive the strings
    std::unordered_set<std::string> keys;

    // Children proxies for nested tables
    std::unordered_map<std::string, SolLuaDataModelTableProxy> children;
};

class SolLuaDataModel {
  public:
    SolLuaDataModel(const sol::table &model, const Rml::DataModelConstructor &constructor);

    SolLuaDataModelTableProxy &topLevelProxy();

  private:
    void wrapTable(SolLuaDataModelTableProxy &proxy, bool topLevel);

    Rml::DataModelConstructor m_constructor;

    SolLuaDataModelTableProxy m_topLevelProxy;
};

class SolLuaObjectDef final : public Rml::VariableDefinition {
  public:
    SolLuaObjectDef(sol::table table);
    bool Get(void *ptr, Rml::Variant &variant) override;
    bool Set(void *ptr, const Rml::Variant &variant) override;
    int Size(void *ptr) override;
    DataVariable Child(void *ptr, const Rml::DataAddressEntry &address) override;

    sol::table &table();

  protected:
    sol::table m_table;
};

} // end namespace Rml::SolLua
