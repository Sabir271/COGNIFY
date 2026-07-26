cmake_minimum_required(VERSION 3.14...3.30)
project(COGNIFY)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# 1. Fetch raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG master 
)
FetchContent_MakeAvailable(raylib)

# 2. Fetch raygui
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG master
)
FetchContent_MakeAvailable(raygui)

# Explicitly list your actual source files (Remove MatchTwo.cpp if it's gone!)
add_executable(COGNIFY 
    src/main.cpp
    src/Tests/TypingTest.cpp
    src/Tests/StroopTest.cpp
    src/Tests/ReactionTime.cpp
    src/Tests/VerbalTest.cpp
    src/Tests/NumberMemory.cpp
    src/UIStyle.cpp
)

target_include_directories(COGNIFY PRIVATE 
    src/
    ${raygui_SOURCE_DIR}/src
)

target_link_libraries(COGNIFY raylib)

# Avoid clashing with a running COGNIFY.exe (locked on Windows):
# write a different runtime output name so linking won't fail if the original
# executable is running. This keeps the target name `COGNIFY` but produces
# `COGNIFY_build.exe` as the actual output binary.
set_target_properties(COGNIFY PROPERTIES RUNTIME_OUTPUT_NAME "COGNIFY_build")