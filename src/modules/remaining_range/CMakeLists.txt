############################################################################
#
#   Copyright (c) 2025 Amal Cherian Jacob. All rights reserved.
#
############################################################################

px4_add_module(
    MODULE modules__remaining_range
    MAIN remaining_range
    SRCS remaining_range.cpp
    MODULE_CONFIG
        module.yaml
    DEPENDS
        platforms__common
)
