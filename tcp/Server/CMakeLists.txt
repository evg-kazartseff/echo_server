set(NAME_PROJECT TCP_Server)

set(SOURCES main.c Server.c Server.h Queue.c Queue.h Set.c Set.h)

add_executable(${NAME_PROJECT} ${SOURCES})
target_link_libraries(${NAME_PROJECT} pthread)