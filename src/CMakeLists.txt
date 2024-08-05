# CMakeList.txt : CMake project for ShakTrisImpl, include source and define
# project specific logic here.
#

set(SHAKTRIS_SOURCES
		"engine/Game.cpp"
		"util/rng.cpp"

	"Move.cpp"

	"VersusGame.cpp"
)

set( SHAKTRIS_HEADERS
		"engine/Board.hpp"
		"engine/Game.hpp"
		"engine/Piece.hpp"
		#"engine/BitPiece.hpp"
		"engine/ShaktrisConstants.hpp"
		"util/pext.hpp"
		"util/rng.hpp"
	"Chance.hpp"
	"Move.hpp"
	"VersusGame.hpp"


)
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
# Add source to this project's executable.
add_library(ShakTris STATIC ${SHAKTRIS_SOURCES})

target_include_directories(ShakTris PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(ShakTris PROPERTIES PUBLIC_HEADER "${SHAKTRIS_HEADERS}")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShakTris PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
