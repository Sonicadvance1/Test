set(SRCS	Src/Main.cpp
			Src/Graphics/Linux.cpp
			Src/Graphics/Graphics.cpp
	)
#Don't have any of these yet


include(FindOpenGL)
include(FindPNG)
include(FindFreetype)
include_directories(${OPENGL_INCLUDE_DIR}, ${PNG_INCLUDE_DIR}, ${FREETYPE_INCLUDE_DIRS})
set(LIBS common
	GLEW
	${OPENGL_LIBRARIES}
	${X11_LIBRARIES}
	${PNG_LIBRARIES}
	${FREETYPE_LIBRARIES}
	ftgl
	)

set(TEST_EXE Client)

add_executable(${TEST_EXE} ${SRCS})
target_link_libraries(${TEST_EXE} ${LIBS})
install(TARGETS ${TEST_EXE} RUNTIME DESTINATION ${bindir})

