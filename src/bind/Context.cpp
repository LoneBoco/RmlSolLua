#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <RmlSolLua_private.h>
#include <RmlUi/Core.h>
#include SOLHPP

#include "bind.h"
#include "plugin/SolLuaDataModel.h"
#include "plugin/SolLuaDocument.h"

namespace Rml::SolLua {

namespace document {
/// <summary>
/// Return a SolLuaDocument.
/// </summary>
static auto getDocumentBypass(Rml::Context &self, int idx) {
    auto document = self.GetDocument(idx);
    auto result = dynamic_cast<SolLuaDocument *>(document);
    return result;
}

/// <summary>
/// Return a SolLuaDocument.
/// </summary>
static auto getDocumentBypassString(Rml::Context &self, const Rml::String &name) {
    auto document = self.GetDocument(name);
    return dynamic_cast<SolLuaDocument *>(document);
}

/// <summary>
/// Helper function to fill the indexed table with data.
/// </summary>
static auto getDocument(Rml::Context &self) {
    std::function<SolLuaDocument *(int)> result = [&self](int idx) -> auto {
        return getDocumentBypass(self, idx);
    };
    return result;
}
}

namespace datamodel {

/// <summary>
/// Opens a Lua data model.
/// </summary>
/// <param name="self">The context that called this function.</param>
/// <param name="name">The name of the data model.</param>
/// <param name="model">The table to bind as the data model.</param>
/// <param name="s">Lua state.</param>
/// <returns>A unique pointer to a Sol Lua Data Model.</returns>
static std::shared_ptr<SolLuaDataModelTableProxy>
openDataModel(Rml::Context &self, const Rml::String &name, sol::object model, sol::this_state s) {
    if (model.get_type() != sol::type::table) {
        Rml::Log::Message(Log::LT_ERROR, "Data model must be a table.");
        return nullptr;
    }

    // Create data model.
    auto constructor = self.CreateDataModel(name);

    // Already created?  Get existing.
    if (!constructor) {
        constructor = self.GetDataModel(name);
        if (!constructor) {
            return nullptr;
        }
    }

    auto dataModel = std::make_shared<SolLuaDataModel>(model.as<sol::table>(), constructor);

    // Alias data model to it's top level proxy.
    return { dataModel, &dataModel->topLevelProxy() };
}
}

namespace element {
static auto getElementAtPoint1(Rml::Context &self, Rml::Vector2f point) {
    return self.GetElementAtPoint(point);
}

static auto getElementAtPoint2(Rml::Context &self, Rml::Vector2f point, Rml::Element &ignore) {
    return self.GetElementAtPoint(point, &ignore);
}
}

/// <summary>
/// Binds the Rml::Context class to Lua.
/// </summary>
/// <param name="lua">The Lua state to bind into.</param>
void bind_context(sol::state_view &lua) {
    lua.new_usertype<Rml::Context>(
        "Context",
        sol::no_constructor,
        // M
        "AddEventListener",
        &Rml::Context::AddEventListener,
        "CreateDocument",
        [](Rml::Context &self) {
            return self.CreateDocument();
        },
        "LoadDocument",
        [](Rml::Context &self, const Rml::String &document) {
            auto doc = self.LoadDocument(document);
            return dynamic_cast<SolLuaDocument *>(doc);
        },
        "GetDocument",
        &document::getDocumentBypassString,
        "Render",
        &Rml::Context::Render,
        "UnloadAllDocuments",
        &Rml::Context::UnloadAllDocuments,
        "UnloadDocument",
        &Rml::Context::UnloadDocument,
        "Update",
        &Rml::Context::Update,
        "OpenDataModel",
        &datamodel::openDataModel,
        "ProcessMouseMove",
        &Rml::Context::ProcessMouseMove,
        "ProcessMouseButtonDown",
        &Rml::Context::ProcessMouseButtonDown,
        "ProcessMouseButtonUp",
        &Rml::Context::ProcessMouseButtonUp,
        "ProcessMouseWheel",
        sol::resolve<bool(float, int)>(&Rml::Context::ProcessMouseWheel),
        "ProcessMouseLeave",
        &Rml::Context::ProcessMouseLeave,
        "IsMouseInteracting",
        &Rml::Context::IsMouseInteracting,
        "ProcessKeyDown",
        &Rml::Context::ProcessKeyDown,
        "ProcessKeyUp",
        &Rml::Context::ProcessKeyUp,
        "ProcessTextInput",
        sol::resolve<bool(const Rml::String &)>(&Rml::Context::ProcessTextInput),
        //--
        "EnableMouseCursor",
        &Rml::Context::EnableMouseCursor,
        "ActivateTheme",
        &Rml::Context::ActivateTheme,
        "IsThemeActive",
        &Rml::Context::IsThemeActive,
        "GetElementAtPoint",
        sol::overload(&element::getElementAtPoint1, &element::getElementAtPoint2),
        "PullDocumentToFront",
        &Rml::Context::PullDocumentToFront,
        "PushDocumentToBack",
        &Rml::Context::PushDocumentToBack,
        "UnfocusDocument",
        &Rml::Context::UnfocusDocument,
        // RemoveEventListener

        // G+S
        "dimensions",
        sol::property(&Rml::Context::GetDimensions, &Rml::Context::SetDimensions),
        "dp_ratio",
        sol::property(&Rml::Context::GetDensityIndependentPixelRatio, &Rml::Context::SetDensityIndependentPixelRatio),

        // G
        "documents",
        sol::readonly_property(&getIndexedTable<
                               SolLuaDocument,
                               Rml::Context,
                               &document::getDocument,
                               &Rml::Context::GetNumDocuments>),
        "focus_element",
        sol::readonly_property(&Rml::Context::GetFocusElement),
        "hover_element",
        sol::readonly_property(&Rml::Context::GetHoverElement),
        "name",
        sol::readonly_property(&Rml::Context::GetName),
        "root_element",
        sol::readonly_property(&Rml::Context::GetRootElement)
    );
}

} // end namespace Rml::SolLua
