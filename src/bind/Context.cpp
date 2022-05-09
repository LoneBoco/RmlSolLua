#include "bind.h"

#include "plugin/SolLuaDocument.h"

#include <memory>


namespace Rml::SolLua
{

	namespace document
	{
		/// <summary>
		/// Return a SolLuaDocument.
		/// </summary>
		auto getDocumentBypass(Rml::Context& self, int idx)
		{
			auto document = self.GetDocument(idx);
			auto result = dynamic_cast<SolLuaDocument*>(document);
			return result;
		}

		/// <summary>
		/// Return a SolLuaDocument.
		/// </summary>
		auto getDocumentBypassString(Rml::Context& self, const Rml::String& name)
		{
			auto document = self.GetDocument(name);
			return dynamic_cast<SolLuaDocument*>(document);
		}

		/// <summary>
		/// Helper function to fill the indexed table with data.
		/// </summary>
		auto getDocument(Rml::Context& self)
		{
			std::function<SolLuaDocument* (int)> result = [&self](int idx) -> auto { return getDocumentBypass(self, idx); };
			return result;
		}
	}

	void bind_context(sol::state_view& lua)
	{
		lua.new_usertype<Rml::Context>("Context", sol::no_constructor,
			// M
			"AddEventListener", &Rml::Context::AddEventListener,
			"CreateDocument", [](Rml::Context& self) { return self.CreateDocument(); },
			"LoadDocument", [](Rml::Context& self, const Rml::String& document) {
				auto doc = self.LoadDocument(document);
				return dynamic_cast<SolLuaDocument*>(doc);
			},
			"GetDocument", &document::getDocumentBypassString,
			"Render", &Rml::Context::Render,
			"UnloadAllDocuments", &Rml::Context::UnloadAllDocuments,
			"UnloadDocument", &Rml::Context::UnloadDocument,
			"Update", &Rml::Context::Update,

			// G+S
			"dimensions", sol::property(&Rml::Context::GetDimensions, &Rml::Context::SetDimensions),

			// G
			"documents", sol::readonly_property(&getIndexedTable<SolLuaDocument, Rml::Context, &document::getDocument, &Rml::Context::GetNumDocuments>),
			"focus_element", sol::readonly_property(&Rml::Context::GetFocusElement),
			"hover_element", sol::readonly_property(&Rml::Context::GetHoverElement),
			"name", sol::readonly_property(&Rml::Context::GetName),
			"root_element", sol::readonly_property(&Rml::Context::GetRootElement)
		);
	}

} // end namespace Rml::SolLua
