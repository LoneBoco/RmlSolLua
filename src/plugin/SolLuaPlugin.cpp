#include "SolLuaPlugin.h"

#include "SolLuaInstancer.h"

#include "bind/bind.h"



namespace Rml::SolLua
{

	SolLuaPlugin::SolLuaPlugin(sol::state_view lua_state)
		: m_lua_state{ lua_state }
	{
		bind_color(lua_state);
		bind_context(lua_state);
		bind_element(lua_state);
		bind_document(lua_state);
		bind_event(lua_state);
		bind_global(lua_state);
		bind_log(lua_state);
		bind_vector(lua_state);
		bind_convert(lua_state);
	}

	int SolLuaPlugin::GetEventClasses()
	{
		return EVT_BASIC;
	}

	void SolLuaPlugin::OnInitialise()
	{
		document_element_instancer = std::make_unique<SolLuaDocumentElementInstancer>(m_lua_state);
		event_listener_instancer = std::make_unique<SolLuaEventListenerInstancer>(m_lua_state);
		Factory::RegisterElementInstancer("body", document_element_instancer.get());
		Factory::RegisterEventListenerInstancer(event_listener_instancer.get());
	}

	void SolLuaPlugin::OnShutdown()
	{
		delete this;
	}

} // end namespace Rml::SolLua
