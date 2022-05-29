#include "SolLuaDocument.h"

#include <RmlUi/Core/Stream.h>
#include <RmlUi/Core/Log.h>


namespace Rml::SolLua
{
	/// <summary>
	/// Lua error handler.
	/// </summary>
	/// <param name="">(Unused) Lua state.</param>
	/// <param name="pfr">The result that holds our error.</param>
	/// <returns>The error result.</returns>
	static sol::protected_function_result ErrorHandler(lua_State*, sol::protected_function_result pfr)
	{
		if (!pfr.valid())
		{
			sol::error err = pfr;
			Rml::Log::Message(Rml::Log::LT_ERROR, "[LUA][ERROR] %s", err.what());
		}
		return pfr;
	}

	SolLuaDocument::SolLuaDocument(sol::state_view state, const Rml::String& tag)
		: m_state(state), ElementDocument(tag)
	{
	}

	void SolLuaDocument::LoadInlineScript(const Rml::String& context, const Rml::String& source_path, int source_line)
	{
		Rml::String buffer;
		buffer += "--";
		buffer += source_path;
		buffer += ":";
		buffer += Rml::ToString(source_line);
		buffer += "\n";
		buffer += context;

		m_state.safe_script(buffer, ErrorHandler);
	}

	void SolLuaDocument::LoadExternalScript(const String& source_path)
	{
		m_state.safe_script_file(source_path, ErrorHandler);
	}

} // namespace Rml::SolLua
