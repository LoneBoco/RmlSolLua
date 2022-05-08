#include "bind.h"

#include <memory>


namespace Rml::SolLua
{

	namespace document
	{
		auto getDocument(Rml::Context& self)
		{
			auto resolved = static_cast<Rml::ElementDocument* (Rml::Context::*)(int)>(&Rml::Context::GetDocument);
			std::function<Rml::ElementDocument* (int)> result = std::bind(resolved, &self, std::placeholders::_1);
			return result;
		}
	}

	void bind_context(sol::state_view& lua)
	{
		lua.new_usertype<Rml::Context>("Context", sol::no_constructor,
			// M
			"AddEventListener", &Rml::Context::AddEventListener,
			"CreateDocument", [](Rml::Context& self) { return self.CreateDocument(); },
			"LoadDocument", sol::resolve<Rml::ElementDocument* (const Rml::String&)>(&Rml::Context::LoadDocument),
			"Render", &Rml::Context::Render,
			"UnloadAllDocuments", &Rml::Context::UnloadAllDocuments,
			"UnloadDocument", &Rml::Context::UnloadDocument,
			"Update", &Rml::Context::Update,

			// G+S
			"dimensions", sol::property(&Rml::Context::GetDimensions, &Rml::Context::SetDimensions),

			// G
			"documents", sol::readonly_property(&getIndexedTable<Rml::ElementDocument, Rml::Context, &document::getDocument, &Rml::Context::GetNumDocuments>),
			"focus_element", sol::readonly_property(&Rml::Context::GetFocusElement),
			"hover_element", sol::readonly_property(&Rml::Context::GetHoverElement),
			"name", sol::readonly_property(&Rml::Context::GetName),
			"root_element", sol::readonly_property(&Rml::Context::GetRootElement)
		);
	}

} // end namespace Rml::SolLua
