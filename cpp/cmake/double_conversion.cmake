# Google's double-conversion library
# https://github.com/google/double-conversion

FetchContent_Declare(
        double_conversion
        GIT_REPOSITORY https://github.com/google/double-conversion.git
        GIT_TAG tags/v3.3.0
        GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(double_conversion)