set(SRCS	Src/Main.cpp
			Src/Database.cpp
	)

set(LIBS	
	common
	pthread
	sqlite3
	)



set(TEST_EXE Server)

add_executable(${TEST_EXE} ${SRCS})
target_link_libraries(${TEST_EXE} ${LIBS})
install(TARGETS ${TEST_EXE} RUNTIME DESTINATION ${bindir})

