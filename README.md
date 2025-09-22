RmlSolLua 6.0
===================

Generate bindings for [RmlUi](https://github.com/mikke89/RmlUi) using [sol3](https://github.com/ThePhD/sol2).

**RmlUi**'s built-in Lua plugin only supports Lua 5.2 and up.  This project leverages the **sol3** library to generate bindings that are compatible with Lua versions 5.1+ (and LuaJIT 2.1+ and MoonJIT).  **RmlSolLua** also extends the base Lua implementation with many new bindings.

This project requires C++20.

## Usage

```c++
#include <RmlUi/RmlUi.h>
#include <sol/sol.hpp>
#include <RmlSolLua/RmlSolLua.h>

int main()
{
  // Open the sol3 Lua state.
  sol::state lua;
  lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math);

  // Initialize RmlUi.
  Backend::Initialize(...);
  Rml::Initialise();

  // Initialize RmlSolLua.
  Rml::SolLua::Initialise(&lua);

  // Do stuff.
  {
    // ...
  }

  // Collect garbage before shutdown.
  // This is needed in RmlUi 5.0 and below to prevent a crash.
  // Alternatively, make sure initialize the RmlSolLua plugin LAST.
  lua.collect_garbage();

  // Shutdown RmlUi.
  // This will also close RmlSolLua.
  Rml::Shutdown();
  Backend::Shutdown();

  return 0;
}

```

> ##### Note
> **RmlSolLua** does not manage your **sol3** `sol::state`.  You must ensure it stays in scope until after the call to `Rml::Shutdown`.

## Coverage

**RmlSolLua** contains all the **RmlUi 6.0** Lua bindings EXCEPT:
- custom element instancing
- datagrid sources and formatters

**RmlSolLua** contains many extra Lua bindings not covered by **RmlUi**.  There are too many to list, but they can be found in the bindings under `src/bind/*.cpp` below any `//--` comments.  Any binding not covered by **RmlUi**'s base Lua bindings are kept separate to be easily identified.

## Previous Versions

**RmlSolLua** supports **RmlUi 5.0** and up.
Check the branches for support for older versions of **RmlUi**.

## CMake Example

This is just an example of how to consume **RmlSolLua** in your own CMake project as a static library.

```cmake
include(FetchContent)
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)

# Dependency: Lua.
find_package(Lua REQUIRED)
target_include_directories(main PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${LUA_LIBRARIES})

# Dependency: sol3.
set(SOL2_LUA_VERSION "5.4")
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(main PRIVATE sol2)

# Dependency: RmlUi.
add_compile_definitions(RMLUI_STATIC_LIB)
find_package(RmlUi CONFIG REQUIRED)
target_link_libraries(main PRIVATE RmlUi::RmlUi)

# Dependency: RmlSolLua.
FetchContent_Declare(RmlSolLua
	GIT_REPOSITORY https://github.com/LoneBoco/RmlSolLua.git
	GIT_TAG rmlui-6.0
	GIT_SHALLOW TRUE
	OVERRIDE_FIND_PACKAGE)
FetchContent_MakeAvailable(RmlSolLua)
target_include_directories(main PUBLIC "${RmlSolLua_SOURCE_DIR}/include")
target_link_libraries(main PRIVATE RmlSolLua)
```

## License

**RmlSolLua** is published under the [MIT license](LICENSE).
