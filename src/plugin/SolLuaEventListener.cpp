#include "SolLuaEventListener.h"

#include "plugin/SolLuaDocument.h"

#include <RmlUi/Core/Element.h>
#include <string>
//#include <format>


namespace Rml::SolLua
{

    SolLuaEventListener::SolLuaEventListener(sol::state_view& lua, const Rml::String& code, Rml::Element* element)
        : m_element(element)
    {
        //auto f = std::format("return function (event,element,document) {} end", code);
        std::string f{ "return function (event,element,document) " };
        f.append(code);
        f.append(" end");

        auto result = lua.safe_script(f);
        if (result.valid())
        {
            sol::protected_function func = result.get<sol::protected_function>();
            m_func = func;
        }
    }

    SolLuaEventListener::SolLuaEventListener(sol::protected_function func, Rml::Element* element)
        : m_func(func), m_element(element)
    {
    }

    void SolLuaEventListener::OnDetach(Rml::Element* element)
    {
        delete this;
    }
    
    void SolLuaEventListener::ProcessEvent(Rml::Event& event)
    {
        auto document = dynamic_cast<SolLuaDocument*>(m_element->GetOwnerDocument());
        if (document != nullptr)
            m_func.call(event, m_element, document);
    }

} // namespace Rml::SolLua
