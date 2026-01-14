px4_add_module(
    MODULE modules__remaining_range
    MAIN remaining_range
    STACK_MAIN 1200
    STACK_PRIORITY SCHED_PRIORITY_DEFAULT
    SRCS remaining_range.cpp
    DEPENDS
        platforms__common
)
