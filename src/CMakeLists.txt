# Dongshin Kim, 2018-05-07 23:12
include_directories (
    ${mvbluefox2_drive_SOURCE_DIR}/src
    ${mvbluefox2_drive_SOURCE_DIR}/include    
    )
link_directories (${mvbluefox2_drive_BINARY_DIR}/src)


add_library(mvbluefox2 mvbluefox2_io.cpp)

add_executable (mvbluefox2_drv mvbluefox2_drv.cpp)


