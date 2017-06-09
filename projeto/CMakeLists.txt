include_directories(../include)
add_executable(image
	image.cpp)
target_link_libraries(image
	gdemu)
