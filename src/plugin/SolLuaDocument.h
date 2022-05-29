#pragma once

#include <RmlUi/Core/ElementDocument.h>

#include <sol/sol.hpp>


namespace Rml::SolLua
{
	class SolLuaDocument : public ::Rml::ElementDocument
	{
	public:
		/// <summary>
		/// Construct the SolLuaDocument.
		/// </summary>
		/// <param name="state">The Lua state to register into.</param>
		/// <param name="tag">The document tag (body).</param>
		SolLuaDocument(sol::state_view state, const Rml::String& tag);

		/// <summary>
		/// Loads an inline script.
		/// </summary>
		/// <param name="context">The UI context.</param>
		/// <param name="source_path">Source path.</param>
		/// <param name="source_line">Source line.</param>
		void LoadInlineScript(const Rml::String& content, const Rml::String& source_path, int source_line) override;

		/// <summary>
		/// Loads a script from a file.
		/// </summary>
		/// <param name="source_path">The file to load.</param>
		void LoadExternalScript(const Rml::String& source_path) override;

	protected:
		sol::state_view m_state;
	};

} // namespace Rml::SolLua
