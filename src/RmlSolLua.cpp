#include "RmlSolLua.h"

#include <sol/sol.hpp>
#include <RmlUi/Core.h>

#include "src/plugin/SolLuaPlugin.h"


namespace Rml::SolLua
{

    void Initialise(sol::state_view* state)
    {
        if (state != nullptr)
            ::Rml::RegisterPlugin(new SolLuaPlugin(*state));
    }

} // end namespace Rml::SolLua
