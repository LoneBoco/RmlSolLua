#include "RmlSolLua.h"

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
            
            bind_color(*state);
            bind_context(*state);
            bind_document(*state);
            bind_element(*state);
            bind_event(*state);
            bind_vector(*state);
        }
    }

} // end namespace Rml::SolLua
