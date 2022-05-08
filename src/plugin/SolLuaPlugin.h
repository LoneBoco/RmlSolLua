// RmlSolLua.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "../RmlSolLua.h"

#include <RmlUi/Lua/Header.h>
#include <RmlUi/Core/Plugin.h>
#include <RmlUi/Core/Platform.h>
#include <sol/sol.hpp>

#include <memory>


namespace Rml::SolLua
{

    class SolLuaDocumentElementInstancer;
    class SolLuaEventListenerInstancer;

    class RMLUILUA_API SolLuaPlugin : public Plugin
    {
    public:
        SolLuaPlugin(sol::state_view lua_state);

    private:
        int GetEventClasses() override;

        void OnInitialise() override;

        void OnShutdown() override;

        std::unique_ptr<SolLuaDocumentElementInstancer> document_element_instancer;
        std::unique_ptr<SolLuaEventListenerInstancer> event_listener_instancer;

        sol::state_view m_lua_state;
    };

} // end namespace Rml::SolLua
