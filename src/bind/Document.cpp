#include "bind.h"

#include "plugin/SolLuaDocument.h"


namespace Rml::SolLua
{

	namespace document
	{
		auto show(SolLuaDocument& self)
		{
			self.Show();
		}

		auto showModal(SolLuaDocument& self, Rml::ModalFlag modal)
		{
			self.Show(modal);
		}

		auto showModalFocus(SolLuaDocument& self, Rml::ModalFlag modal, Rml::FocusFlag focus)
		{
			self.Show(modal, focus);
		}

		auto reloadStyleSheet(SolLuaDocument& self)
		{
			self.ReloadStyleSheet();

		}

		auto reloadStyleSheetAndLoad(SolLuaDocument& self, bool load)
		{
			reloadStyleSheet(self);
			if (load)
			{
				// Dispatch the load event so we can re-bind any scripts that got wiped out.
				self.DispatchEvent(EventId::Load, Dictionary());
			}
		}
	}

	void bind_document(sol::state_view& lua)
	{
		lua.new_enum<Rml::ModalFlag>("RmlModalFlag",
			{
				{ "None", Rml::ModalFlag::None },
				{ "Modal", Rml::ModalFlag::Modal },
				{ "Keep", Rml::ModalFlag::Keep }
			}
		);

		lua.new_enum<Rml::FocusFlag>("RmlFocusFlag",
			{
				{ "", Rml::FocusFlag::None },
				{ "", Rml::FocusFlag::Document },
				{ "", Rml::FocusFlag::Keep },
				{ "", Rml::FocusFlag::Auto }
			}
		);

		lua.new_usertype<SolLuaDocument>("Document", sol::no_constructor,
			// M
			"PullToFront", &SolLuaDocument::PullToFront,
			"PushToBack", &SolLuaDocument::PushToBack,
			"Show", sol::overload(&document::show, &document::showModal, &document::showModalFocus),
			"Hide", &SolLuaDocument::Hide,
			"Close", &SolLuaDocument::Close,
			"CreateElement", &SolLuaDocument::CreateElement,
			"CreateTextNode", &SolLuaDocument::CreateTextNode,
			"ReloadStyleSheet", sol::overload(&document::reloadStyleSheet, &document::reloadStyleSheetAndLoad),

			// G+S
			"title", sol::property(&SolLuaDocument::GetTitle, &SolLuaDocument::SetTitle),

			// G
			"context", sol::readonly_property(&SolLuaDocument::GetContext),

			// B
			sol::base_classes, sol::bases<Rml::Element>()
		);
	}

} // end namespace Rml::SolLua
