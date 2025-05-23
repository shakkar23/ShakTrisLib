﻿# CMakeList.txt : CMake project for ShakTrisImpl, include source and define
# project specific logic here.
#

set(SHAKTRIS_SOURCES
		"engine/Game.cpp"
		"util/rng.cpp"

	"Move.cpp"

	"VersusGame.cpp"
)

set( SHAKTRIS_HEADERS
		"engine/BitPiece.hpp"
		"engine/Board.hpp"
		"engine/Game.hpp"
		"engine/MoveGen.hpp"
		"engine/Piece.hpp"
		"engine/ShaktrisConstants.hpp"
		"engine/RotationSystems.hpp"
		"engine/Utiity.hpp"

		"util/pext.hpp"
		"util/rng.hpp"
	"Move.hpp"
	"VersusGame.hpp"


)
if(NOT MSVC)
	if(NOT DEFINED EMSCRIPTEN)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
	endif()
else()
# add /profile for MSVC
	set(CMAKE_EXE_LINKER_FLAGS_CXX "${CMAKE_EXE_LINKER_FLAGS_CXX} /PROFILE")
endif()

# Add source to this project's executable.
add_library(ShakTris STATIC ${SHAKTRIS_SOURCES})

target_include_directories(ShakTris PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ShakTris PROPERTIES PUBLIC_HEADER "${SHAKTRIS_HEADERS}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShakTris PROPERTY CXX_STANDARD 23)
endif()

set(CMAKE_CXX_STANDARD 23)
add_executable(ShakTrisTest "test.cpp")

target_link_libraries(ShakTrisTest ShakTris)
