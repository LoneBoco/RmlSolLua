#include "bind.h"


namespace Rml::SolLua
{

	void bind_event(sol::state& lua)
	{
		lua.new_usertype<Rml::Event>("Event", sol::no_constructor,
			// M
			"StopPropagation", &Rml::Event::StopPropagation,

			// G
			"current_element", sol::readonly_property(&Rml::Event::GetCurrentElement),
			"type", sol::readonly_property(&Rml::Event::GetType),
			"target_element", sol::readonly_property(&Rml::Event::GetTargetElement),
			"parameters", sol::readonly_property(&Rml::Event::GetParameters)
			);
	}

} // end namespace Rml::SolLua
