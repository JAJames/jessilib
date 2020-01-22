# Clang-specific compiler settings

# Enable all/extra warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Treat all warnings as errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# Enable address sanitizer on debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Enable std::filesystem
set(JESSILIB_ADDITOINAL_LIBS "stdc++fs")
