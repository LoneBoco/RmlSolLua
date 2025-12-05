#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

namespace Rml::SolLua
{
	class SolLuaDataModel;

	class SolLuaDataModelProxy final : public Rml::VariableDefinition
	{
	public:
		SolLuaDataModelProxy(SolLuaDataModel* datamodel, sol::table table);
		bool Get(void* ptr, Rml::Variant& variant) override;
		bool Set(void* ptr, const Rml::Variant& variant) override;
		int Size(void* ptr) override;
		DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override;

		sol::object get(const sol::object& key) const;
		void set(const sol::object& key, sol::object value);

		void bind(bool topLevel);
		void rebind(const sol::table& newTable);

	protected:
		SolLuaDataModel* m_datamodel;
		sol::table m_table;

		// Children proxies for nested tables
		std::unordered_map<std::string, SolLuaDataModelProxy> m_children;

		// Store keys of non-table values in a set just to keep alive the strings
		std::unordered_set<std::string> m_keys;

		// Not string_view to avoid transient copy since Rml expects String&
		const std::string* m_topLevelKey = nullptr;

		bool m_dirty = false;
	};

	class SolLuaDataModel
	{
	public:
		SolLuaDataModel(const sol::table& model, const Rml::DataModelConstructor& constructor);

		Rml::DataModelConstructor constructor() const;
		SolLuaDataModelProxy& topLevelProxy();

	private:
		Rml::DataModelConstructor m_constructor;
		SolLuaDataModelProxy m_topLevelProxy;
	};

} // end namespace Rml::SolLua
