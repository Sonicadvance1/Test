set(SRCS	Src/Main.cpp)
#Don't have any of these yet
set(LIBS common)



set(TEST_EXE Client)

add_executable(${TEST_EXE} ${SRCS})
target_link_libraries(${TEST_EXE} ${LIBS})
install(TARGETS ${TEST_EXE} RUNTIME DESTINATION ${bindir})

