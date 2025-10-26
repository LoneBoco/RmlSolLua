#pragma once

#include <RmlUi/Core.h>

#ifdef RMLUILUA_API
    #undef RMLUILUA_API
#endif

#if !defined RMLSOLLUA_STATIC_LIB
    #ifdef RMLUI_PLATFORM_WIN32
        #if defined RmlSolLua_EXPORTS
            #define RMLUILUA_API __declspec(dllexport)
        #else
            #define RMLUILUA_API __declspec(dllimport)
        #endif
    #else
        #define RMLUILUA_API __attribute__((visibility("default")))
    #endif
#else
    #define RMLUILUA_API
#endif

namespace sol
{
    class state_view;
}

namespace Rml::SolLua
{
    /// @brief Initializes RmlSolLua using the supplied Lua state.
    /// @param state The Lua state to initialize into.
    RMLUILUA_API void Initialise(sol::state_view* state);

    /// @brief Initializes RmlSolLua using the supplied Lua state.
    /// Sets the Lua variable specified by lua_environment_identifier to the document's id when running Lua code.
    /// @param state The Lua state to initialize into.
    /// @param lua_environment_identifier The Lua variable name that is set to the document's id.
    RMLUILUA_API void Initialise(sol::state_view* state, const Rml::String& lua_environment_identifier);

    /// @brief Initializes RmlSolLua using the supplied Lua state.
    /// @param state The Lua state to initialize into.
    RMLUILUA_API void Initialize(sol::state_view* state);

    /// @brief Initializes RmlSolLua using the supplied Lua state.
    /// Sets the Lua variable specified by lua_environment_identifier to the document's id when running Lua code.
    /// @param state The Lua state to initialize into.
    /// @param lua_environment_identifier The Lua variable name that is set to the document's id.
    RMLUILUA_API void Initialize(sol::state_view* state, const Rml::String& lua_environment_identifier);

    /// @brief Registers RmlSolLua into the specified Lua state.
    /// @param state The Lua state to register into.
    RMLUILUA_API void RegisterLua(sol::state_view* state);

} // end namespace Rml::SolLua
