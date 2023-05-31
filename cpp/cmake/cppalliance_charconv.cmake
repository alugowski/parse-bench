# Candidate library for Boost charconv.
# Under heavy development with no official release as of yet.
# https://github.com/cppalliance/charconv


# Boost is a dependency

set(BOOST_INCLUDE_LIBRARIES config assert core)
set(BOOST_ENABLE_CMAKE ON)

message("Fetching Boost. Please be patient.")
include(FetchContent)
FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.80.0
)
FetchContent_MakeAvailable(Boost)

# Get charconv itself

FetchContent_Declare(
        cppalliance_charconv
        GIT_REPOSITORY https://github.com/cppalliance/charconv.git
        GIT_TAG develop
        GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(cppalliance_charconv)