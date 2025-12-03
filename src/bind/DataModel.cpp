#include <string>

#include <RmlSolLua_private.h>
#include SOLHPP

#include "plugin/SolLuaDataModel.h"

namespace Rml::SolLua
{
	namespace functions
	{
		static sol::object dataModelGet(SolLuaDataModel& self, const std::string& name, sol::this_state s)
		{
			return self.Table.get<sol::object>(name);
		}

		static void dataModelSet(SolLuaDataModel& self, const std::string& name, sol::object value, sol::this_state s)
		{
			self.Handle.DirtyVariable(name);
			self.Table.set(name, value);
		}
	} // namespace functions

	void bind_datamodel(sol::state_view& lua)
	{
		// clang-format off
		lua.new_usertype<SolLuaDataModel>("SolLuaDataModel", sol::no_constructor,
			sol::meta_function::index, &functions::dataModelGet,
			sol::meta_function::new_index, &functions::dataModelSet
		);
		// clang-format on
	}

} // end namespace Rml::SolLua
