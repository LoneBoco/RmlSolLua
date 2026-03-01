#include <RmlSolLua_private.h>
#include SOLHPP

#include "plugin/SolLuaDataModel.h"

namespace Rml::SolLua
{
	namespace functions
	{
		static sol::object dataModelGet(SolLuaDataModelProxy& self, const sol::object& key)
		{
			return self.get(key);
		}

		static void dataModelSet(SolLuaDataModelProxy& self, const sol::object& key, sol::object value)
		{
			self.set(key, value);
		}

		static sol::object dataModelLength(SolLuaDataModelProxy& self, sol::this_state state)
		{
			return sol::make_object(state, self.Size(nullptr));
		}
	} // namespace functions

	void bind_datamodel(sol::state_view& lua)
	{
		// clang-format off
		lua.new_usertype<SolLuaDataModelProxy>("SolLuaDataModelProxy", sol::no_constructor,
		    sol::meta_function::index, &functions::dataModelGet,
		    sol::meta_function::new_index, &functions::dataModelSet,
			sol::meta_function::length, &functions::dataModelLength
		);
		// clang-format on
	}
} // end namespace Rml::SolLua
