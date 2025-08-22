cmake_minimum_required(VERSION 3.20)
project(YESS LANGUAGES CXX)

# Match your flags: -std=c++11 -Wall -O0 -g
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(yess
        yess.cpp
        PipeReg.cpp
        PipeRegField.cpp
        Simulate.cpp
        FetchStage.cpp
        DecodeStage.cpp
        ExecuteStage.cpp
        MemoryStage.cpp
        WritebackStage.cpp
        F.cpp
        D.cpp
        E.cpp
        M.cpp
        W.cpp
        Memory.cpp
        Tools.cpp
        RegisterFile.cpp
        Loader.cpp
        ConditionCodes.cpp
)

target_compile_options(yess PRIVATE -Wall -O0 -g)

# If you keep headers in subdirs like include/, uncomment:
# target_include_directories(yess PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Recreate your `run` target (builds yess, then runs run.sh)
add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E env ./run.sh
        DEPENDS yess
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        USES_TERMINAL
)

# Optional: remove Outputs like your make clean did
add_custom_target(clean_outputs
        COMMAND ${CMAKE_COMMAND} -E rm -rf Outputs
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)