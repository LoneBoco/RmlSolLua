#include <utility>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

#include "plugin/SolLuaDocument.h"

namespace Rml::SolLua
{

	namespace document
	{
		static auto show(SolLuaDocument& self)
		{
			self.Show();
		}

		static auto showModal(SolLuaDocument& self, Rml::ModalFlag modal)
		{
			self.Show(modal);
		}

		static auto showModalFocus(SolLuaDocument& self, Rml::ModalFlag modal, Rml::FocusFlag focus)
		{
			self.Show(modal, focus);
		}

		static auto reloadStyleSheet(SolLuaDocument& self)
		{
			self.ReloadStyleSheet();
		}

		static auto reloadStyleSheetAndLoad(SolLuaDocument& self, bool load)
		{
			reloadStyleSheet(self);
			if (load)
			{
				// Dispatch the load event so we can re-bind any scripts that got wiped out.
				self.DispatchEvent(EventId::Load, Dictionary());
			}
		}

		static auto loadInlineScript3(SolLuaDocument& self, const Rml::String& content, const Rml::String& source_path, int source_line)
		{
			self.LoadInlineScript(content, source_path, source_line);
		}

		static auto loadInlineScript2(SolLuaDocument& self, const Rml::String& content, const Rml::String& source_path)
		{
			loadInlineScript3(self, content, source_path, 0);
		}

		static auto loadInlineScript1(SolLuaDocument& self, const Rml::String& content)
		{
			loadInlineScript3(self, content, self.GetSourceURL(), 0);
		}

		static auto appendToStyleSheet(SolLuaDocument& self, const Rml::String& content)
		{
			auto styleSheet = Rml::Factory::InstanceStyleSheetString(content);
			auto combined = styleSheet->CombineStyleSheetContainer(*self.GetStyleSheetContainer());
			self.SetStyleSheetContainer(std::move(combined));
		}
	} // namespace document

	void bind_document(sol::state_view& lua)
	{
		// clang-format off
		lua.new_enum<Rml::ModalFlag>("RmlModalFlag",
			{
				{ "None", Rml::ModalFlag::None },
				{ "Modal", Rml::ModalFlag::Modal },
				{ "Keep", Rml::ModalFlag::Keep }
			}
		);

		lua.new_enum<Rml::FocusFlag>("RmlFocusFlag",
			{
				{ "None", Rml::FocusFlag::None },
				{ "Document", Rml::FocusFlag::Document },
				{ "Keep", Rml::FocusFlag::Keep },
				{ "Auto", Rml::FocusFlag::Auto }
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
			//--
			"ReloadStyleSheet", sol::overload(&document::reloadStyleSheet, &document::reloadStyleSheetAndLoad),
			"LoadInlineScript", sol::overload(&document::loadInlineScript1, &document::loadInlineScript2, &document::loadInlineScript3),
			"LoadExternalScript", &SolLuaDocument::LoadExternalScript,
			"UpdateDocument", &SolLuaDocument::UpdateDocument,
			"AppendToStyleSheet", &document::appendToStyleSheet,

			// G+S
			"title", sol::property(&SolLuaDocument::GetTitle, &SolLuaDocument::SetTitle),

			// G
			"context", sol::readonly_property(&SolLuaDocument::GetContext),
			//--
			"url", sol::readonly_property(&SolLuaDocument::GetSourceURL),
			"modal", sol::readonly_property(&SolLuaDocument::IsModal),

			// B
			sol::base_classes, sol::bases<Rml::Element>()
		);
		// clang-format on
	}

} // end namespace Rml::SolLua
