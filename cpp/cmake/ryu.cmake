# Fetch Dragonbox floating-point formatting library

include(FetchContent)
message("fetching ryu")
FetchContent_Declare(
        ryu
        GIT_REPOSITORY https://github.com/ulfjack/ryu
        GIT_TAG master
        GIT_SHALLOW TRUE
)

# Ryu does not include a CMakeLists.txt, so define the library manually:

FetchContent_Populate(ryu)

message("${ryu_SOURCE_DIR}")

# ryu library
add_library(ryu
        ${ryu_SOURCE_DIR}/ryu/f2s.c
        ${ryu_SOURCE_DIR}/ryu/f2s_full_table.h
        ${ryu_SOURCE_DIR}/ryu/f2s_intrinsics.h
        ${ryu_SOURCE_DIR}/ryu/d2s.c
        ${ryu_SOURCE_DIR}/ryu/d2fixed.c
        ${ryu_SOURCE_DIR}/ryu/d2fixed_full_table.h
        ${ryu_SOURCE_DIR}/ryu/d2s_full_table.h
        ${ryu_SOURCE_DIR}/ryu/d2s_small_table.h
        ${ryu_SOURCE_DIR}/ryu/d2s_intrinsics.h
        ${ryu_SOURCE_DIR}/ryu/digit_table.h
        ${ryu_SOURCE_DIR}/ryu/common.h
        ${ryu_SOURCE_DIR}/ryu/ryu.h)

# This directory is the include root because the headers are in ryu/ and are included as "ryu/*.h"
target_include_directories(ryu PUBLIC ${ryu_SOURCE_DIR})

# add alias so the project can be used with add_subdirectory
add_library(ryu::ryu ALIAS ryu)
