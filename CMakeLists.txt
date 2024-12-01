cmake_minimum_required(VERSION 3.17)

project(RmlSolLua VERSION 1.0.0 DESCRIPTION "" LANGUAGES CXX)
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



# TODO: Add tests and install targets if needed.
