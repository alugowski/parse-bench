# Fetch Ryu floating-point formatting library

include(FetchContent)
FetchContent_Declare(
        ryu
        GIT_REPOSITORY https://github.com/ulfjack/ryu
        GIT_TAG master
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(ryu)
