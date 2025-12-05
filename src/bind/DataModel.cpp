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

		static void
		dataModelSet(SolLuaDataModelProxy& self, const sol::object& key, sol::object value)
		{
			self.set(key, value);
		}
	} // namespace functions

	void bind_datamodel(sol::state_view& lua)
	{
		// clang-format off
		lua.new_usertype<SolLuaDataModelProxy>("SolLuaDataModelProxy", sol::no_constructor,
		    sol::meta_function::index, &functions::dataModelGet,
		    sol::meta_function::new_index, &functions::dataModelSet
		);
		// clang-format on
	}
} // end namespace Rml::SolLua
