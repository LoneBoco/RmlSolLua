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
class SolLuaDataModel;

/// Userdata that proxies any table in the model, recursively
struct SolLuaDataModelTableProxy {
    SolLuaDataModel *model = nullptr;
    std::unique_ptr<SolLuaObjectDef> objectDef;

    // Children proxies for nested tables
    std::unordered_map<std::string, SolLuaDataModelTableProxy> children;

    // Store keys of non-table values in a set just to keep alive the strings
    std::unordered_set<std::string> keys;

    // Not string_view to avoid transient copy since Rml expects String&
    const std::string *topLevelKey = nullptr;

    bool dirty = false;
};

class SolLuaDataModel {
  public:
    SolLuaDataModel(const sol::table &model, const Rml::DataModelConstructor &constructor);

    Rml::DataModelHandle modelHandle() const;

    SolLuaDataModelTableProxy &topLevelProxy();
    void rebindNestedTable(SolLuaDataModelTableProxy &nestedProxy, const sol::table &newTable);

  private:
    void bindTable(SolLuaDataModelTableProxy &proxy, bool topLevel);

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

    void setTable(sol::table table);

  protected:
    sol::table m_table;
};

} // end namespace Rml::SolLua
