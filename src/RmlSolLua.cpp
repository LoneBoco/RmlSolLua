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
        }
    }

    void Initialize(sol::state_view* state)
    {
        Initialise(state);
    }

} // end namespace Rml::SolLua
