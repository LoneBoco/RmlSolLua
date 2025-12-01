#include <string>

#include <RmlSolLua_private.h>
#include SOLHPP

#include "plugin/SolLuaDataModel.h"

namespace Rml::SolLua {
namespace functions {
static sol::object dataModelGet(SolLuaDataModelTableProxy &self, const sol::object &key, sol::this_state s) {
    std::string skey = key.is<std::string>() ? key.as<std::string>() : std::format("[{}]", key.as<int>() - 1);
    auto proxyTable = self.children.find(skey);
    if (proxyTable != self.children.end()) {
        return sol::make_object(s, &proxyTable->second);
    }
    return self.objectDef->table().get<sol::object>(key);
}

static void
dataModelSet(SolLuaDataModelTableProxy &self, const sol::object &key, sol::object value, sol::this_state s) {
    std::string skey = key.is<std::string>() ? key.as<std::string>() : std::format("[{}]", key.as<int>() - 1);
    self.objectDef->table().set(key, value);
    self.modelHandle.DirtyVariable(self.topLevelKey ? *self.topLevelKey : skey);
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
