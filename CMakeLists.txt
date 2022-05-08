# CMakeList.txt : CMake project for RmlSolLua, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

project ("RmlSolLua")

set (CMAKE_CXX_STANDARD 17)

# Add source to this project's executable.
add_library (RmlSolLua STATIC)

target_sources (RmlSolLua
	PRIVATE
		"src/RmlSolLua.h"
		"src/RmlSolLua.cpp"
		"src/plugin/SolLuaDocument.cpp"
		"src/plugin/SolLuaDocument.h"
		"src/plugin/SolLuaEventListener.cpp"
		"src/plugin/SolLuaEventListener.h"
		"src/plugin/SolLuaInstancer.cpp"
		"src/plugin/SolLuaInstancer.h"
		"src/plugin/SolLuaPlugin.cpp"
		"src/plugin/SolLuaPlugin.h"
		"src/bind/bind.h"
		"src/bind/Colour.cpp"
		"src/bind/Context.cpp"
		"src/bind/Document.cpp"
		"src/bind/Element.cpp"
		"src/bind/Event.cpp"
		"src/bind/Vector.cpp"
	PUBLIC
		"src/RmlSolLua.h"
)

target_include_directories (RmlSolLua
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		"H:/Projects/CPP/2drp/dependencies/RmlUi/Include/"
		"H:/Projects/CPP/2drp/dependencies/sol2/include/"
		"H:/Projects/CPP/2drp/dependencies/luajit-2.0/src/"

	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)


# TODO: Add tests and install targets if needed.
