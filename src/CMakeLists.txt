# Dongshin Kim, 2018-05-07 23:12
set(common_lib /home/dongshin/Shares/lib)
set(common_include 
    /home/dongshin/Shares/include
    /home/dongshin/Shares/mvIMPACT
)

include_directories (
    ${mvbluefox2_drive_SOURCE_DIR}/src
    ${mvbluefox2_drive_SOURCE_DIR}/include    
    ${common_include}
    )
link_directories (
    ${mvbluefox2_drive_BINARY_DIR}/src
    ${common_lib}
)


add_library(mvbluefox2 mvbluefox2_io.cpp
)
target_link_libraries(mvbluefox2 
    opencv_core
    opencv_highgui
    mvBlueFOX
    mvDeviceManager
    mvPropHandling
    pthread
)

add_executable (mvbluefox2_drv mvbluefox2_drv.cpp)

target_link_libraries(mvbluefox2_drv
    mvbluefox2

)

install (TARGETS mvbluefox2 EXPORT mvbluefox2-targets DESTINATION lib)
install (FILES 
    ../include/mvbluefox2_common.h  
    ../include/mvbluefox2_io.h   
    DESTINATION include)
install(EXPORT mvbluefox2-targets DESTINATION lib/mvbluefox2)
install(FILES mvbluefox2Config.cmake DESTINATION lib/mvbluefox2)
