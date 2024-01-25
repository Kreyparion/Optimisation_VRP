cmake_minimum_required (VERSION 2.8.11)
project (test)

# import CPLEX


set(CPLEX_DIR "/home/lucien/Bureau/projet_opti_appli/cplex")
set(CONCERT_DIR "/home/lucien/Bureau/projet_opti_appli/concert")


set(PROJECT_CFLAGS "-Wall -std=c++11 -O3")

add_executable (test test.cpp)

target_include_directories(test PUBLIC ${CPLEX_DIR}/include ${CONCERT_DIR}/include)


set_target_properties(test PROPERTIES COMPILE_FLAGS ${PROJECT_CFLAGS})


set_target_properties(test PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -static")


target_link_libraries(test 
    ${CPLEX_DIR}/lib/x86-64_linux/static_pic/libilocplex.a
    ${CPLEX_DIR}/lib/x86-64_linux/static_pic/libcplex.a
    ${CONCERT_DIR}/lib/x86-64_linux/static_pic/libconcert.a
    -lm -lpthread -ldl
)




