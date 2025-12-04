#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

namespace Rml::SolLua
{

	void bind_stylesheet(sol::state_view& lua)
	{
		// clang-format off
		lua.new_usertype<Rml::StyleSheet>("StyleSheet", sol::no_constructor,
			// M
			//--
			"MergeStyleSheet", &Rml::StyleSheet::MergeStyleSheet
		);
		// clang-format on
	}

} // end namespace Rml::SolLua
