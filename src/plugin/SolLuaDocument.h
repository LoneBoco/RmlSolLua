#pragma once

#include <RmlUi/Core/ElementDocument.h>

#include <sol/sol.hpp>


namespace Rml::SolLua
{
    class SolLuaDocument : public ::Rml::ElementDocument
    {
    public:
        SolLuaDocument(sol::state_view state, const String& tag);
        void LoadInlineScript(const String& content, const String& source_path, int source_line) override;
        void LoadExternalScript(const String& source_path) override;

    protected:
        sol::state_view m_state;
    };

} // namespace Rml::SolLua
