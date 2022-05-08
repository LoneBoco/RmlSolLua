#include "bind.h"

#include "src/plugin/SolLuaDocument.h"


namespace Rml::SolLua
{

	void bind_document(sol::state& lua)
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
			"context", sol::readonly_property(&SolLuaDocument::GetContext)
			);
	}

} // end namespace Rml::SolLua
