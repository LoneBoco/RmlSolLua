#include "bind.h"

#include <memory>


namespace Rml::SolLua
{

	std::shared_ptr<Rml::ElementList> getDocuments(Rml::Context& self, sol::this_state s)
	{
		auto result = std::make_shared<Rml::ElementList>();

		for (int i = 0; i < self.GetNumDocuments(); ++i)
		{
			auto child = self.GetDocument(i);
			result->push_back(child);
		}

		return result;
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
			"documents", sol::readonly_property(&getDocuments),
			"focus_element", sol::readonly_property(&Rml::Context::GetFocusElement),
			"hover_element", sol::readonly_property(&Rml::Context::GetHoverElement),
			"name", sol::readonly_property(&Rml::Context::GetName),
			"root_element", sol::readonly_property(&Rml::Context::GetRootElement)
			);
	}

} // end namespace Rml::SolLua
