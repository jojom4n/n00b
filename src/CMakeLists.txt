set(PROJECT_NAME n00b)

################################################################################
# Source groups
################################################################################
set(HEADER_FILES
    "attack.h"
    "bitscan.h"
    "defs.h"
    "display.h"
    "divide.h"
    "enums.h"
    "evaluation.h"
    "fen.h"
    "game.h"
    "globals.h"
    "libpopcnt.h"
    "magic.h"
    "makemove.h"
    "movegen.h"
    "moveorder.h"
    "overloading.h"
    "params.h"
    "pch.h"
    "perft.h"
    "Position.h"
    "psqt.h"
    "search.h"
    "tt.h"
    "zobrist.h"
)
source_group("Header files" FILES ${HEADER_FILES})

set(SRC_FILES
    "attack.cpp"
    "bitscan.cpp"
    "display.cpp"
    "evaluation.cpp"
    "fen.cpp"
    "game.cpp"
    "magic.cpp"
    "main.cpp"
    "makemove.cpp"
    "movegen.cpp"
    "moveorder.cpp"
    "overloading.cpp"
    "pch.cpp"
    "perft.cpp"
    "Position.cpp"
    "search.cpp"
    "tt.cpp"
    "zobrist.cpp"
)
source_group("Source files" FILES ${SRC_FILES})

set(RSRC_FILES
    "../TODO.txt"
	"../COPYING"
)
source_group("Resource files" FILES ${RSRC_FILES})

set(ALL_FILES
    ${HEADER_FILES}
    ${SRC_FILES}
    ${RSRC_FILES}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})