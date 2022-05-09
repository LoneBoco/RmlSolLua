#include "RmlSolLua/RmlSolLua.h"

#include <sol/sol.hpp>
#include <RmlUi/Core.h>

#include "bind/bind.h"
#include "plugin/SolLuaPlugin.h"


namespace Rml::SolLua
{

    void Initialise(sol::state_view* state)
    {
        if (state != nullptr)
        {
            ::Rml::RegisterPlugin(new SolLuaPlugin(*state));
            RegisterLua(state);
        }
    }

    void Initialize(sol::state_view* state)
    {
        Initialise(state);
    }

    void RegisterLua(sol::state_view* state)
    {
        bind_color(*state);
        bind_context(*state);
        bind_element(*state);
        bind_element_derived(*state);
        bind_element_form(*state);
        bind_document(*state);
        bind_event(*state);
        bind_global(*state);
        bind_log(*state);
        bind_vector(*state);
        bind_convert(*state);
    }

} // end namespace Rml::SolLua
