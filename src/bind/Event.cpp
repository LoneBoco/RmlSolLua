#include "bind.h"


namespace Rml::SolLua
{

	namespace functions
	{
		SolObjectMap getParameters(Rml::Event& self, sol::this_state s)
		{
			SolObjectMap result;

			const auto& parameters = self.GetParameters();
			for (auto& [key, value] : parameters)
			{
				if (self.GetId() == Rml::EventId::Tabchange && value.GetType() == Rml::Variant::INT)
				{
					auto object = sol::make_object(s, value.Get<int>());
					result.insert(std::make_pair(key, object));
				}
				else
				{
					auto object = makeObjectFromVariant(&value, s);
					result.insert(std::make_pair(key, object));
				}
			}

			return result;
		}
	}

	void bind_event(sol::state_view& lua)
	{
		lua.new_usertype<Rml::Event>("Event", sol::no_constructor,
			// M
			"StopPropagation", &Rml::Event::StopPropagation,

			// G
			"current_element", sol::readonly_property(&Rml::Event::GetCurrentElement),
			"type", sol::readonly_property(&Rml::Event::GetType),
			"target_element", sol::readonly_property(&Rml::Event::GetTargetElement),
			"parameters", sol::readonly_property(&functions::getParameters)
		);
	}

} // end namespace Rml::SolLua
