RmlSolLua
===================

Generate bindings for [RmlUi](https://github.com/mikke89/RmlUi) using [sol3](https://github.com/ThePhD/sol2).

**RmlUi**'s built-in Lua plugin only supports Lua 5.2 and up.  This project leverages the **sol3** library to generate bindings that are compatible with Lua versions 5.1+ (and LuaJIT 2.1+ and MoonJIT).

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
  Rml::Initialise();

  // Initialize RmlSolLua.
  Rml::SolLua::Initialise(&lua);

  // Do stuff.
  {
    // ...
  }

  Rml::Shutdown();

  return 0;
}

```

> ##### Note
> **RmlSolLua** does not manage your **sol3** `sol::state`.  You must ensure it stays in scope until after the call to `Rml::Shutdown`.

## Coverage

**RmlSolLua** contains all the **RmlUi** Lua bindings EXCEPT for data sources.  Those are still a work in progress.

## License

**RmlSolLua** is published under the [MIT license](LICENSE).
