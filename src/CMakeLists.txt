cmake_minimum_required(VERSION 2.8.11)

file(GLOB_RECURSE HW_H *.h)
file(GLOB_RECURSE HW_SRC *.c)

if(MSVC)
	add_executable(HW ${HW_SRC} ${HW_H})
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../res" DESTINATION "${CMAKE_BINARY_DIR}/src/VKL-Test")
else()
	add_executable(HW ${HW_SRC} ${HW_H})
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../res" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

target_link_libraries (HW LINK_PUBLIC VKL)
target_link_libraries (HW glfw ${GLFW_LIBRARIES})
