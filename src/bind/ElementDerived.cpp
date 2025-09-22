#include <RmlUi/Core.h>
#include <sol/sol.hpp>

#include "bind/bind.h"


namespace Rml::SolLua
{

	void bind_element_derived(sol::state_view& lua)
	{

		lua.new_usertype<Rml::ElementText>("ElementText", sol::no_constructor,
			// G
			"text", sol::property(&Rml::ElementText::GetText, &Rml::ElementText::SetText),

			// B
			sol::base_classes, sol::bases<Rml::Element>()
		);

		///////////////////////////

		lua.new_usertype<Rml::ElementTabSet>("ElementTabSet", sol::no_constructor,
			// M
			"SetPanel", sol::resolve<void(int, const Rml::String&)>(&Rml::ElementTabSet::SetPanel),
			"SetTab", sol::resolve<void(int, const Rml::String&)>(&Rml::ElementTabSet::SetTab),
			//--
			"RemoveTab", &Rml::ElementTabSet::RemoveTab,

			// G+S
			"active_tab", sol::property(&Rml::ElementTabSet::GetActiveTab, &Rml::ElementTabSet::SetActiveTab),

			// G
			"num_tabs", &Rml::ElementTabSet::GetNumTabs,

			// B
			sol::base_classes, sol::bases<Rml::Element>()
		);

		///////////////////////////

		//--
		lua.new_usertype<Rml::ElementProgress>("ElementProgress", sol::no_constructor,
			// G+S
			//--
			"value", sol::property(&Rml::ElementProgress::GetValue, &Rml::ElementProgress::SetValue),
			"max", sol::property(&Rml::ElementProgress::GetMax, &Rml::ElementProgress::SetMax),

			// B
			sol::base_classes, sol::bases<Rml::Element>()
		);
	}

} // end namespace Rml::SolLua
