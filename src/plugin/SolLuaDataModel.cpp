#include <string>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

#include "SolLuaDataModel.h"

#include "SolLuaDocument.h"
#include "bind/bind.h"

namespace Rml::SolLua
{
	namespace
	{

		class LiteralIntDefinition final : public VariableDefinition
		{
		public:
			LiteralIntDefinition()
			    : VariableDefinition(DataVariableType::Scalar)
			{
			}

			bool Get(void* ptr, Variant& variant) override
			{
				variant = static_cast<int>(reinterpret_cast<intptr_t>(ptr));
				return true;
			}
		};

		DataVariable MakeLiteralIntVariable(int value)
		{
			static LiteralIntDefinition literal_int_definition;
			return DataVariable(&literal_int_definition, reinterpret_cast<void*>(static_cast<intptr_t>(value)));
		}

	} // namespace

	SolLuaDataModel::SolLuaDataModel(const sol::table& model, const Rml::DataModelConstructor& constructor)
	    : m_constructor(constructor),
	      m_topLevelProxy(this, model)
	{
		m_topLevelProxy.bind(true);
	}

	Rml::DataModelConstructor SolLuaDataModel::constructor() const
	{
		return m_constructor;
	}

	SolLuaDataModelProxy& SolLuaDataModel::topLevelProxy()
	{
		return m_topLevelProxy;
	}

	/// SolLuaDatamodelProxy
	SolLuaDataModelProxy::SolLuaDataModelProxy(SolLuaDataModel* datamodel, sol::table table)
	    : VariableDefinition(DataVariableType::Scalar),
	      m_datamodel(datamodel), m_table(std::move(table))
	{
	}

	bool SolLuaDataModelProxy::Get(void* ptr, Rml::Variant& variant)
	{
		sol::object obj;
		auto* key = const_cast<const char*>(static_cast<char*>(ptr));
		if (key[0] == '[')
		{
			// Pseudo-key: access by index
			int idx;
			std::from_chars_result result = std::from_chars(key + 1, key + std::strlen(key) - 1, idx);
			RMLUI_ASSERT(result.ec == std::errc{} && "Rml failed to sanitize user input to be well-formed");
			if (idx < 0 || idx >= m_table.size())
			{
				Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
				return false;
			}
			obj = m_table[idx + 1]; // Lua is 1-based
		}
		else
		{
			obj = m_table[key];
		}

		if (obj.is<bool>())
		{
			variant = obj.as<bool>();
		}
		else if (obj.is<std::string>())
		{
			variant = obj.as<std::string>();
		}
		else if (obj.is<Rml::Vector2i>())
		{
			variant = obj.as<Vector2i>();
		}
		else if (obj.is<Rml::Vector2f>())
		{
			variant = obj.as<Vector2f>();
		}
		else if (obj.is<Rml::Colourb>())
		{
			variant = obj.as<Rml::Colourb>();
		}
		else if (obj.is<Rml::Colourf>())
		{
			variant = obj.as<Rml::Colourf>();
		}
		else if (obj.is<double>())
		{
			variant = obj.as<double>();
		}
		else
		{
			variant = Rml::Variant{};
		}

		return true;
	}

	bool SolLuaDataModelProxy::Set(void* ptr, const Rml::Variant& variant)
	{
		auto* key = const_cast<const char*>(static_cast<char*>(ptr));
		if (key[0] == '[')
		{
			// Pseudo-key: access by index
			int idx;
			std::from_chars_result result = std::from_chars(key + 1, key + std::strlen(key) - 1, idx);
			RMLUI_ASSERT(result.ec == std::errc{} && "Rml failed to sanitize user input to be well-formed");
			if (idx < 0 || idx >= m_table.size())
			{
				Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
				return false;
			}
			sol::table_proxy obj = m_table[idx + 1]; // Lua is 1-based
			obj = makeObjectFromVariant(&variant, m_table.lua_state());
		}
		else
		{
			sol::table_proxy obj = m_table[key];
			obj = makeObjectFromVariant(&variant, m_table.lua_state());
		}
		return true;
	}

	int SolLuaDataModelProxy::Size(void* ptr)
	{
		SolLuaDataModelProxy& proxy = *static_cast<SolLuaDataModelProxy*>(ptr);
		return static_cast<int>(proxy.m_table.size());
	}

	DataVariable SolLuaDataModelProxy::Child(void* ptr, const Rml::DataAddressEntry& address)
	{
		SolLuaDataModelProxy& proxy = *static_cast<SolLuaDataModelProxy*>(ptr);

		std::string skey;
		sol::object obj;
		if (address.index != -1)
		{

			if (address.index < 0 || address.index >= m_table.size())
			{
				Rml::Log::Message(Rml::Log::LT_ERROR, "Data array index out of bounds.");
				return {};
			}
			// Access by index
			skey = std::format("[{}]", address.index);
			obj = m_table[address.index + 1]; // Lua is 1-based
		}
		else
		{
			if (address.name == "size")
			{
				return MakeLiteralIntVariable(m_table.size());
			}

			skey = address.name;
			obj = proxy.m_table[address.name];
		}

		if (obj.get_type() == sol::type::table)
		{
			auto it = proxy.m_children.find(skey);
			RMLUI_ASSERT(it != proxy.m_children.end());
			if (it->second.m_dirty)
			{
				it->second.rebind(obj);
			}

			// Pass proxy as ptr to be used in `Child` calls further down the chain
			return {&it->second, &it->second};
		}

		auto it = proxy.m_keys.find(skey);
		RMLUI_ASSERT(it != proxy.m_keys.end());
		return {&proxy, const_cast<char*>(it->data())};
	}

	sol::object SolLuaDataModelProxy::get(const sol::object& key) const
	{
		std::string skey = key.is<std::string>() ? key.as<std::string>() : std::format("[{}]", key.as<int>() - 1);
		auto proxyIt = m_children.find(skey);
		if (proxyIt != m_children.end())
		{
			return sol::make_object_userdata(m_table.lua_state(), &proxyIt->second);
		}
		return m_table.get<sol::object>(key);
	}

	void SolLuaDataModelProxy::set(const sol::object& key, sol::object value)
	{
		std::string skey = key.is<std::string>() ? key.as<std::string>() : std::format("[{}]", key.as<int>() - 1);
		m_table.set(key, value);
		m_datamodel->constructor().GetModelHandle().DirtyVariable(m_topLevelKey ? *m_topLevelKey : skey);

		if (value.get_type() == sol::type::table)
		{
			// Also dirty nested table's proxy
			auto proxyTableIt = m_children.find(skey);
			RMLUI_ASSERT(proxyTableIt != m_children.end());
			proxyTableIt->second.m_dirty = true;
		}
	}

	void SolLuaDataModelProxy::bind(bool topLevel)
	{
		for (auto& [key, value] : m_table)
		{
			std::string skey;
			if (key.get_type() == sol::type::string)
			{
				skey = key.as<std::string>();
			}
			else if (key.get_type() == sol::type::number)
			{
				// Assign a pseudo-key for numeric indices
				// TODO: check if the number is an integer?
				skey = std::format("[{}]", key.as<int>() - 1); // Lua is 1-based
			}
			else
			{
				Rml::Log::Message(Log::LT_ERROR, "Data model key with type other than string or integer is unsupported");
				return;
			}

			if (value.get_type() == sol::type::table)
			{
				auto childProxyIt = m_children.try_emplace(skey, m_datamodel, value.as<sol::table>());
				RMLUI_ASSERT(childProxyIt.second);
				childProxyIt.first->second.m_topLevelKey = m_topLevelKey ? m_topLevelKey : &childProxyIt.first->first;
				childProxyIt.first->second.bind(false);
				if (topLevel && skey[0] != '[')
				{
					// Only bind top-level non-integer keys
					m_datamodel->constructor().BindCustomDataVariable(
					    skey, Rml::DataVariable(&childProxyIt.first->second, &childProxyIt.first->second)
					);
				}
			}
			else
			{
				if (value.get_type() == sol::type::function)
				{
					if (!topLevel)
					{
						Rml::Log::Message(
						    Log::LT_WARNING, "Event callbacks are only allowed at the top level of a data model."
						);
						continue;
					}

					m_datamodel->constructor().BindEventCallback(
					    skey,
					    [cb = sol::protected_function{value},
					     state = sol::state_view{m_table.lua_state(
					     )}](Rml::DataModelHandle, Rml::Event& event, const Rml::VariantList& varlist)
					    {
						    if (cb.valid())
						    {
							    std::vector<sol::object> args;
							    for (const auto& variant : varlist)
							    {
								    args.push_back(makeObjectFromVariant(&variant, state));
							    }
							    auto pfr = cb(event, sol::as_args(args));
							    if (!pfr.valid())
							    {
								    ErrorHandler(cb.lua_state(), std::move(pfr));
							    }
						    }
					    }
					);
				}
				else
				{
					auto it = m_keys.emplace(skey);
					if (topLevel && skey[0] != '[')
					{
						// Only bind top-level non-integer keys
						m_datamodel->constructor().BindCustomDataVariable(
						    skey, Rml::DataVariable(this, const_cast<char*>(it.first->data()))
						);
					}
				}
			}
		}
	}

	void SolLuaDataModelProxy::rebind(const sol::table& newTable)
	{
		RMLUI_ASSERT(m_dirty);

		m_children.clear(); // Orphan existing children
		m_table = newTable; // Update table
		bind(false);        // Nested rebind

		m_dirty = false;
	}

} // end namespace Rml::SolLua
