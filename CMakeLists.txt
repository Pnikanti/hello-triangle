cmake_minimum_required (VERSION 3.8)
project ("opengl-hello-triangle" VERSION 0.1)

add_executable (opengl-hello-triangle "hello-triangle.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_definitions(-DGLEW_STATIC)

find_package( OpenGL REQUIRED )

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME}
	PUBLIC ${OPENGL_INCLUDE_DIRS}
	PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glfw/include
	PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glew/include
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC ${OPENGL_LIBRARIES}
	PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glfw/lib/glfw3.lib
	PUBLIC ${PROJECT_SOURCE_DIR}/dependencies/glew/lib/glew32s.lib
)