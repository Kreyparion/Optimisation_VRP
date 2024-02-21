cmake_minimum_required(VERSION 3.22.1)
project(VRP)

set(CPLEX_DIR "/opt/ibm/ILOG/CPLEX_Studio2211/cplex")
set(CONCERT_DIR "/opt/ibm/ILOG/CPLEX_Studio2211/concert")

set(PROJECT_CFLAGS "-Wall -std=c++17 -O3")

find_package(OpenMP)
if (OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_executable(vrp vrp.cpp)

target_include_directories(vrp PUBLIC ${CPLEX_DIR}/include ${CONCERT_DIR}/include)

set_target_properties(vrp PROPERTIES COMPILE_FLAGS ${PROJECT_CFLAGS})

set_target_properties(vrp PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -static")

target_link_libraries(vrp
        ${CPLEX_DIR}/lib/x86-64_linux/static_pic/libilocplex.a
        ${CPLEX_DIR}/lib/x86-64_linux/static_pic/libcplex.a
        ${CONCERT_DIR}/lib/x86-64_linux/static_pic/libconcert.a
        -lm -lpthread -ldl
)