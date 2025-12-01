#include <string>

#include <RmlSolLua_private.h>
#include SOLHPP

#include "plugin/SolLuaDataModel.h"

namespace Rml::SolLua {
namespace functions {
static sol::object dataModelGet(SolLuaDataModelTableProxy &self, const std::string &name, sol::this_state s) {
    auto proxyTable = self.children.find(name);
    if (proxyTable != self.children.end()) {
        return sol::make_object(s, &proxyTable->second);
    }
    return self.objectDef->table().get<sol::object>(name);
}

static void
dataModelSet(SolLuaDataModelTableProxy &self, const std::string &name, sol::object value, sol::this_state s) {
    self.objectDef->table().set(name, value);
    self.modelHandle.DirtyVariable(name);
}
}

void bind_datamodel(sol::state_view &lua) {
    lua.new_usertype<SolLuaDataModelTableProxy>(
        "SolLuaDataModelTableProxy",
        sol::no_constructor,
        sol::meta_function::index,
        &functions::dataModelGet,
        sol::meta_function::new_index,
        &functions::dataModelSet
    );
}

} // end namespace Rml::SolLua
