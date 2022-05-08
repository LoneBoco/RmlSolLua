#pragma once

#include <RmlUi/Core/ElementInstancer.h>
#include <RmlUi/Core/EventListenerInstancer.h>

#include <sol/sol.hpp>


namespace Rml::SolLua
{

	class SolLuaDocumentElementInstancer : public ::Rml::ElementInstancer
	{
	public:
		SolLuaDocumentElementInstancer(sol::state_view state) : m_state{ state } {}
		ElementPtr InstanceElement(Element* parent, const String& tag, const XMLAttributes& attributes) override;
		void ReleaseElement(Element* element) override;
	protected:
		sol::state_view m_state;
	};

	class SolLuaEventListenerInstancer : public ::Rml::EventListenerInstancer
	{
	public:
		SolLuaEventListenerInstancer(sol::state_view state) : m_state{ state } {}
		EventListener* InstanceEventListener(const String& value, Element* element) override;
	protected:
		sol::state_view m_state;
	};

} // end namespace Rml::SolLua
