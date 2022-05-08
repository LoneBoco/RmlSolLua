#include "bind.h"

#include "plugin/SolLuaEventListener.h"

#include <unordered_map>


namespace Rml::SolLua
{

	namespace functions
	{
		void addEventListener(Rml::Element& self, const Rml::String& event, sol::protected_function func, const bool in_capture_phase = false)
		{
			auto e = new SolLuaEventListener{ func, &self };
			self.AddEventListener(event, e, in_capture_phase);
		}

		void addEventListener(Rml::Element& self, const Rml::String& event, const Rml::String& code, sol::this_state s)
		{
			auto state = sol::state_view{ s };
			auto e = new SolLuaEventListener{ state, code, &self };
			self.AddEventListener(event, e, false);
		}

		void addEventListener(Rml::Element& self, const Rml::String& event, const Rml::String& code, sol::this_state s, const bool in_capture_phase)
		{
			auto state = sol::state_view{ s };
			auto e = new SolLuaEventListener{ state, code, &self };
			self.AddEventListener(event, e, in_capture_phase);
		}

		sol::object makeObjectFromVariant(Rml::Variant* variant, sol::this_state s)
		{
			if (!variant) return sol::make_object(s, sol::nil);

			switch (variant->GetType())
			{
			case Rml::Variant::BOOL:
				return sol::make_object(s, variant->Get<bool>());
			case Rml::Variant::BYTE:
			case Rml::Variant::CHAR:
			case Rml::Variant::INT:
				return sol::make_object(s, variant->Get<int>());
			case Rml::Variant::INT64:
				return sol::make_object(s, variant->Get<int64_t>());
			case Rml::Variant::UINT:
				return sol::make_object(s, variant->Get<unsigned int>());
			case Rml::Variant::UINT64:
				return sol::make_object(s, variant->Get<uint64_t>());
			case Rml::Variant::FLOAT:
			case Rml::Variant::DOUBLE:
				return sol::make_object(s, variant->Get<double>());
			case Rml::Variant::COLOURB:
				return sol::make_object_userdata<Rml::Colourb>(s, variant->Get<Rml::Colourb>());
			case Rml::Variant::COLOURF:
				return sol::make_object_userdata<Rml::Colourf>(s, variant->Get<Rml::Colourf>());
			case Rml::Variant::STRING:
				return sol::make_object(s, variant->GetReference<Rml::String>());
			case Rml::Variant::VECTOR2:
				return sol::make_object_userdata<Rml::Vector2f>(s, variant->Get<Rml::Vector2f>());
			case Rml::Variant::VOIDPTR:
				return sol::make_object(s, variant->Get<void*>());
			default:
				return sol::make_object(s, sol::nil);
			}

			return sol::make_object(s, sol::nil);
		}

		sol::object getAttribute(Rml::Element& self, const Rml::String& name, sol::this_state s)
		{
			sol::state_view state{ s };

			auto attr = self.GetAttribute(name);
			return makeObjectFromVariant(attr, s);
		}

		std::shared_ptr<Rml::ElementList> getElementsByTagName(Rml::Element& self, const Rml::String& tag)
		{
			auto result = std::make_shared<Rml::ElementList>();
			self.GetElementsByTagName(*result, tag);
			return result;
		}

		using SolObjectMap = std::unordered_map<std::string, sol::object>;
		std::shared_ptr<SolObjectMap> getAttributes(Rml::Element& self, sol::this_state s)
		{
			auto result = std::make_shared<SolObjectMap>();

			auto attributes = self.GetAttributes();
			for (auto& [key, value] : attributes)
			{
				auto object = makeObjectFromVariant(&value, s);
				result->insert(std::make_pair(key, object));
			}

			return result;
		}

		std::shared_ptr<Rml::ElementList> getChildNodes(Rml::Element& self, sol::this_state s)
		{
			auto result = std::make_shared<Rml::ElementList>();

			for (int i = 0; i < self.GetNumChildren(); ++i)
			{
				auto child = self.GetChild(i);
				result->push_back(child);
			}

			return result;
		}
	}

	namespace style
	{
		struct StyleProxyIter
		{
			StyleProxyIter(Rml::PropertiesIteratorView&& self) : Iterator(std::move(self)) {}
			Rml::PropertiesIteratorView Iterator;
		};

		auto nextPair(sol::user<StyleProxyIter&> iter_state, sol::this_state s)
		{
			StyleProxyIter& iter = iter_state;
			if (iter.Iterator.AtEnd())
				return std::make_tuple(sol::object(sol::lua_nil), sol::object(sol::lua_nil));

			auto result = std::make_tuple(sol::object(s, sol::in_place, iter.Iterator.GetName()), sol::object(s, sol::in_place, iter.Iterator.GetProperty().ToString()));
			++iter.Iterator;

			return result;
		}

		struct StyleProxy
		{
			StyleProxy(Rml::Element& element) : m_element(element) {}

			std::string Get(const std::string& name)
			{
				auto prop = m_element.GetProperty(name);
				if (prop == nullptr) return {};
				return prop->ToString();
			}

			void Set(const std::string& name, const std::string& value)
			{
				m_element.SetProperty(name, value);
			}

			auto Pairs()
			{
				StyleProxyIter iter{ std::move(m_element.IterateLocalProperties()) };
				return std::make_tuple(&nextPair, std::move(iter), sol::lua_nil);
			}

		private:
			Rml::Element& m_element;
		};

		auto getElementStyleProxy(Rml::Element& self)
		{
			StyleProxy proxy{ self };
			return proxy;
		}
	}

	void bind_element(sol::state& lua)
	{
		lua.new_usertype<Rml::EventListener>("EventListener", sol::no_constructor,
			// M
			"OnAttach", &Rml::EventListener::OnAttach,
			"OnDetach", &Rml::EventListener::OnDetach,
			"ProcessEvent", &Rml::EventListener::ProcessEvent
			);

		lua.new_usertype<style::StyleProxy>("StyleProxy", sol::no_constructor,
			sol::meta_function::index, &style::StyleProxy::Get,
			sol::meta_function::new_index, &style::StyleProxy::Set,
			sol::meta_function::pairs, &style::StyleProxy::Pairs
			);

		lua.new_usertype<Rml::Element>("Element", sol::no_constructor,
			// M
			"AddEventListener", sol::overload(
				[](Rml::Element& s, const Rml::String& e, sol::protected_function f) { functions::addEventListener(s, e, f, false); },
				sol::resolve<void(Rml::Element&, const Rml::String&, sol::protected_function, bool)>(&functions::addEventListener),
				sol::resolve<void(Rml::Element&, const Rml::String&, const Rml::String&, sol::this_state)>(&functions::addEventListener),
				sol::resolve<void(Rml::Element&, const Rml::String&, const Rml::String&, sol::this_state, bool)>(&functions::addEventListener)
			),
			"AppendChild", [](Rml::Element& self, Rml::ElementPtr& e) { self.AppendChild(std::move(e)); },
			"Blur", &Rml::Element::Blur,
			"Click", &Rml::Element::Click,
			"DispatchEvent", sol::resolve<bool(const Rml::String&, const Rml::Dictionary&)>(&Rml::Element::DispatchEvent),
			"Focus", &Rml::Element::Focus,
			"GetAttribute", &functions::getAttribute,
			"GetElementById", &Rml::Element::GetElementById,
			"GetElementsByTagName", &functions::getElementsByTagName,
			"HasAttribute", &Rml::Element::HasAttribute,
			"HasChildNodes", &Rml::Element::HasChildNodes,
			"InsertBefore", [](Rml::Element& self, Rml::ElementPtr& element, Rml::Element* adjacent_element) { self.InsertBefore(std::move(element), adjacent_element); },
			"IsClassSet", &Rml::Element::IsClassSet,
			"RemoveAttribute", &Rml::Element::RemoveAttribute,
			"RemoveChild", &Rml::Element::RemoveChild,
			"ReplaceChild", [](Rml::Element& self, Rml::ElementPtr& inserted_element, Rml::Element* replaced_element) { self.ReplaceChild(std::move(inserted_element), replaced_element); },
			"ScrollIntoView", [](Rml::Element& self, sol::variadic_args va) { if (va.size() == 0) self.ScrollIntoView(true); else self.ScrollIntoView(va[0].as<bool>()); },
			"SetAttribute", static_cast<void(Rml::Element::*)(const Rml::String&, const Rml::String&)>(&Rml::Element::SetAttribute),
			"SetClass", &Rml::Element::SetClass,

			// G+S
			"class_name", sol::property(&Rml::Element::GetClassNames, &Rml::Element::SetClassNames),
			"id", sol::property(&Rml::Element::GetId, &Rml::Element::SetId),
			"inner_rml", sol::property(sol::resolve<Rml::String() const>(&Rml::Element::GetInnerRML), &Rml::Element::SetInnerRML),
			"scroll_left", sol::property(&Rml::Element::GetScrollLeft, &Rml::Element::SetScrollLeft),
			"scroll_top", sol::property(&Rml::Element::GetScrollTop, &Rml::Element::SetScrollTop),

			// G
			"attributes", sol::readonly_property(&functions::getAttributes),
			"child_nodes", sol::readonly_property(&functions::getChildNodes),
			"client_left", sol::readonly_property(&Rml::Element::GetClientLeft),
			"client_height", sol::readonly_property(&Rml::Element::GetClientHeight),
			"client_top", sol::readonly_property(&Rml::Element::GetClientTop),
			"client_width", sol::readonly_property(&Rml::Element::GetClientWidth),
			"first_child", sol::readonly_property(&Rml::Element::GetFirstChild),
			"last_child", sol::readonly_property(&Rml::Element::GetLastChild),
			"next_sibling", sol::readonly_property(&Rml::Element::GetNextSibling),
			"offset_height", sol::readonly_property(&Rml::Element::GetOffsetHeight),
			"offset_left", sol::readonly_property(&Rml::Element::GetOffsetLeft),
			"offset_parent", sol::readonly_property(&Rml::Element::GetOffsetParent),
			"offset_top", sol::readonly_property(&Rml::Element::GetOffsetTop),
			"offset_width", sol::readonly_property(&Rml::Element::GetOffsetWidth),
			"owner_document", sol::readonly_property(&Rml::Element::GetOwnerDocument),
			"parent_node", sol::readonly_property(&Rml::Element::GetParentNode),
			"previous_sibling", sol::readonly_property(&Rml::Element::GetPreviousSibling),
			"scroll_height", sol::readonly_property(&Rml::Element::GetScrollHeight),
			"scroll_width", sol::readonly_property(&Rml::Element::GetScrollWidth),
			"style", sol::readonly_property(&style::getElementStyleProxy),
			"tag_name", sol::readonly_property(&Rml::Element::GetTagName)
			);
	}

} // end namespace Rml::SolLua
