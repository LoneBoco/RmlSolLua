﻿#include "SolLuaPlugin.h"

#include "SolLuaInstancer.h"

#include "bind/bind.h"



namespace Rml::SolLua
{

	SolLuaPlugin::SolLuaPlugin(sol::state_view lua_state)
		: m_lua_state{ lua_state }
	{
	}

	SolLuaPlugin::SolLuaPlugin(sol::state_view lua_state, const Rml::String& lua_environment_identifier)
		: m_lua_state{ lua_state }, m_lua_env_identifier{ lua_environment_identifier }
	{
	}

	int SolLuaPlugin::GetEventClasses()
	{
		return EVT_BASIC;
	}

	void SolLuaPlugin::OnInitialise()
	{
		document_element_instancer = std::make_unique<SolLuaDocumentElementInstancer>(m_lua_state, m_lua_env_identifier);
		event_listener_instancer = std::make_unique<SolLuaEventListenerInstancer>(m_lua_state);
		Factory::RegisterElementInstancer("body", document_element_instancer.get());
		Factory::RegisterEventListenerInstancer(event_listener_instancer.get());
	}

	void SolLuaPlugin::OnShutdown()
	{
		m_lua_state.collect_garbage();
		delete this;
	}

} // end namespace Rml::SolLua
