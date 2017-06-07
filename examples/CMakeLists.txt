include_directories(../include)

set(CMAKE_CXX_FLAGS "-g -Wall")

add_executable(scroll
	scroll.cpp)
target_link_libraries(scroll
	gdemu)

add_executable(palettes
	palettes.cpp)
target_link_libraries(palettes
	gdemu)

add_executable(sprites256
	sprites256.cpp)
target_link_libraries(sprites256
	gdemu)

add_executable(rotate
	rotate.cpp)
target_link_libraries(rotate
	gdemu)

add_executable(collision
	collision.cpp)
target_link_libraries(collision
	gdemu)

add_executable(mario_ufabc
    mario_ufabc_final.cpp)
target_link_libraries(mario_ufabc
    gdemu)

add_executable(RunWalter
    RunWalter.cpp)
target_link_libraries(RunWalter
    gdemu
    ${X11_LIBRARY})
