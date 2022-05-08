#pragma once

#include <RmlUi/Core.h>
#include <sol/sol.hpp>


#ifndef RMLUI_NO_THIRDPARTY_CONTAINERS
template <typename Key, typename Value>
struct sol::is_container<Rml::UnorderedMap<Key, Value>> : std::true_type {};

template <typename Key, typename Value>
struct sol::is_container<Rml::SmallUnorderedMap<Key, Value>> : std::true_type {};

template <typename T>
struct sol::is_container<Rml::UnorderedSet<T>> : std::true_type {};

template <typename T>
struct sol::is_container<Rml::SmallUnorderedSet<T>> : std::true_type {};

//template <typename T>
//struct sol::is_container<Rml::SmallOrderedSet<T>> : std::true_type {};
#endif


namespace Rml::SolLua
{

	void bind_color(sol::state& lua);
	void bind_context(sol::state& lua);
	void bind_document(sol::state& lua);
	void bind_element(sol::state& lua);
	void bind_event(sol::state& lua);
	void bind_vector(sol::state& lua);

} // end namespace Rml::SolLua
