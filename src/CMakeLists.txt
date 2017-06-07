add_library(gdemu
	SPI.cpp
	Serial.cpp
    Keypad.cpp
	GD.cpp)

set(CMAKE_CXX_FLAGS "-g -Wall")

include_directories(gdemu
	../include
	${SDL_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    )
target_link_libraries(gdemu
	${SDL_LIBRARY}
    ${X11_LIBRARIES}
    )
