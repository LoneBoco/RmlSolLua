#include <RmlUi/Core.h>
#include <sol/sol.hpp>


namespace Rml::SolLua
{

	void bind_stylesheet(sol::state_view& lua)
	{
		lua.new_usertype<Rml::StyleSheet>("StyleSheet", sol::no_constructor,
			// M
			//--
			"MergeStyleSheet", &Rml::StyleSheet::MergeStyleSheet
		);
	}

} // end namespace Rml::SolLua
