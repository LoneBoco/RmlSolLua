#pragma once

#include <RmlUi/Core.h>

#ifdef RMLUILUA_API
    #undef RMLUILUA_API
#endif

#if !defined RMLUI_STATIC_LIB
    #ifdef RMLUI_PLATFORM_WIN32
        #if defined RmlLua_EXPORTS
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

    RMLUILUA_API void Initialise(sol::state_view* state);
    RMLUILUA_API void Initialize(sol::state_view* state);

} // end namespace Rml::SolLua
