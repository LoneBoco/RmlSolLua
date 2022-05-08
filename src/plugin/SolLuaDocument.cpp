#include "SolLuaDocument.h"

#include <RmlUi/Core/Stream.h>


namespace Rml::SolLua
{
    SolLuaDocument::SolLuaDocument(sol::state_view state, const String& tag)
        : m_state(state), ElementDocument(tag)
    {
    }

    void SolLuaDocument::LoadInlineScript(const String& context, const String& source_path, int source_line)
    {
        String buffer;
        buffer += "--";
        buffer += source_path;
        buffer += ":";
        buffer += Rml::ToString(source_line);
        buffer += "\n";
        buffer += context;

        m_state.safe_script(buffer);
    }

    void SolLuaDocument::LoadExternalScript(const String& source_path)
    {
        m_state.safe_script_file(source_path);
    }

} // namespace Rml::SolLua
