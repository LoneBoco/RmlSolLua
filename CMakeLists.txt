cmake_minimum_required(VERSION 3.24)

cmake_policy(SET CMP0054 NEW)	# CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0074 NEW)	# CMP0074: find_package uses PackageName_ROOT variables.
cmake_policy(SET CMP0077 NEW)	# CMP0077: option() honors normal variables.
cmake_policy(SET CMP0091 NEW)	# CMP0091: MSVC runtime library flags are selected by an abstraction.
cmake_policy(SET CMP0144 NEW)	# CMP0144: Also search <PACKAGENAME>_ROOT as well as <PackageName>_ROOT.

project(RmlSolLua VERSION 5.1.0 DESCRIPTION "" LANGUAGES CXX)
add_library(RmlSolLua STATIC)

# C++20 mode.
target_compile_features(RmlSolLua PUBLIC cxx_std_20)
set_target_properties(RmlSolLua PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
	if(MSVC_VERSION GREATER_EQUAL 1910)
		target_compile_options(RmlSolLua PUBLIC "/permissive-")
	endif()
	if(MSVC_VERSION GREATER_EQUAL 1914)
		target_compile_options(RmlSolLua PUBLIC "/Zc:__cplusplus")
	endif()
	if(MSVC_VERSION GREATER_EQUAL 1925)
		target_compile_options(RmlSolLua PUBLIC "/Zc:preprocessor")
	endif()
endif()

# Add source to this project's executable.
target_sources(RmlSolLua
	PRIVATE
		"include/RmlSolLua/RmlSolLua.h"
		"src/RmlSolLua.cpp"
		"src/bind/bind.cpp"
		"src/bind/bind.h"
		"src/bind/Colour.cpp"
		"src/bind/Context.cpp"
		"src/bind/Convert.cpp"
		"src/bind/DataModel.cpp"
		"src/bind/Document.cpp"
		"src/bind/Element.cpp"
		"src/bind/ElementDerived.cpp"
		"src/bind/ElementForm.cpp"
		"src/bind/Event.cpp"
		"src/bind/Global.cpp"
		"src/bind/Log.cpp"
		"src/bind/Vector.cpp"
		"src/plugin/SolLuaDataModel.cpp"
		"src/plugin/SolLuaDataModel.h"
		"src/plugin/SolLuaDocument.cpp"
		"src/plugin/SolLuaDocument.h"
		"src/plugin/SolLuaEventListener.cpp"
		"src/plugin/SolLuaEventListener.h"
		"src/plugin/SolLuaInstancer.cpp"
		"src/plugin/SolLuaInstancer.h"
		"src/plugin/SolLuaPlugin.cpp"
		"src/plugin/SolLuaPlugin.h"
	PUBLIC
		"include/RmlSolLua/RmlSolLua.h"
)

target_sources(RmlSolLua PUBLIC FILE_SET HEADERS BASE_DIRS .)
target_include_directories(RmlSolLua
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/"
	PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_definitions(RmlSolLua PRIVATE "RmlLua_EXPORTS")

# RmlUI
find_package(RmlUi CONFIG REQUIRED)
target_link_libraries(RmlSolLua PUBLIC RmlCore)

# Sol2
find_package(sol2 CONFIG REQUIRED)
target_link_libraries(RmlSolLua PUBLIC sol2)

# Try to find a Lua.
set(RmlSolLua_FoundLua FALSE)
find_package(Lua)
if (Lua_FOUND OR LUA_FOUND)
	set(RmlSolLua_FoundLua "Lua")
	target_include_directories(RmlSolLua PRIVATE ${LUA_INCLUDE_DIR})
	target_link_libraries(RmlSolLua PRIVATE ${LUA_LIBRARIES})
endif()

# Try LuaJIT.
if (NOT RmlSolLua_FoundLua)
	# Try to find LuaJIT.
	find_package(LuaJIT)

	# Success.
	if (LuaJIT_FOUND OR LUAJIT_FOUND)
		set(RmlSolLua_FoundLua "LuaJIT")
		target_include_directories(RmlSolLua PRIVATE ${LUAJIT_INCLUDE_DIR})
		target_link_libraries(RmlSolLua PRIVATE ${LUAJIT_LIBRARIES})
	else()
		# If not found, check if pkg-config can find it.
		# vcpkg requires use of pkg-config with LuaJIT.
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(LuaJIT REQUIRED IMPORTED_TARGET luajit)
		if (LuaJIT_FOUND OR LUAJIT_FOUND)
			set(RmlSolLua_FoundLua "LuaJIT")
			target_link_libraries(RmlSolLua PRIVATE PkgConfig::LuaJIT)
		endif()
	endif()
endif()

# We need a Lua.
if (NOT RmlSolLua_FoundLua)
	message(FATAL_ERROR "Could not find a Lua installation. Please install Lua 5.1, 5.2, 5.3, 5.4, or LuaJIT.")
else()
	message(STATUS "Found ${RmlSolLua_FoundLua}.")
endif()
