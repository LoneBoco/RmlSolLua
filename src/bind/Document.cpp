#include "bind.h"

#include "plugin/SolLuaDocument.h"


namespace Rml::SolLua
{

	void bind_document(sol::state_view& lua)
	{
		lua.new_usertype<SolLuaDocument>("Document", sol::no_constructor,
			// M
			"PullToFront", &SolLuaDocument::PullToFront,
			"PushToBack", &SolLuaDocument::PushToBack,
			"Show", &SolLuaDocument::Show,
			"Hide", &SolLuaDocument::Hide,
			"Close", &SolLuaDocument::Close,
			"CreateElement", &SolLuaDocument::CreateElement,
			"CreateTextNode", &SolLuaDocument::CreateTextNode,

			// G+S
			"title", sol::property(&SolLuaDocument::GetTitle, &SolLuaDocument::SetTitle),

			// G
			"context", sol::readonly_property(&SolLuaDocument::GetContext),

			sol::base_classes, sol::bases<Rml::Element>()
			);
	}

} // end namespace Rml::SolLua
