get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/mvbluefox2-targets.cmake)
get_filename_component(mvbluefox2_INCLUDE_DIRS "${SELF_DIR}/../../include" ABSOLUTE)
get_filename_component(mvbluefox2_LIBRARY_DIRS "${SELF_DIR}/../../lib" ABSOLUTE)
