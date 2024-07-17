# CMakeList.txt : CMake project for ShakTrisImpl, include source and define
# project specific logic here.
#

set(SHAKTRIS_SOURCES
		"engine/Board.hpp"
		"engine/Game.hpp"
		"engine/Game.cpp"
		"engine/Piece.hpp"
		"engine/TetrisConstants.hpp"

		"util/Distribution.cpp"
		"util/Distribution.hpp"
		"util/fasthash.cpp"
		"util/fasthash.h"
		"util/MPSC.hpp"
		"util/pext.hpp"
		"util/rng.cpp"
		"util/rng.hpp"
		"util/SPSC.hpp"

	"Chance.hpp"
	"Move.cpp"
	"Move.hpp"

	"VersusGame.cpp"
	"VersusGame.hpp"
)

# Add source to this project's executable.
add_library(ShakTris STATIC ${SHAKTRIS_SOURCES})

target_include_directories(ShakTris PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ShakTris PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
