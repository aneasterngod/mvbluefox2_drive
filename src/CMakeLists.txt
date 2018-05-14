# Dongshin Kim, 2018-05-07 23:12
set(mvIMPACT_INC_DIR /home/dongshin/Shares/mvIMPACT
    CACHE PATH "the root include directory that contains mvIMPACT header files" )
set(mvIMPACT_LIB_DIR /home/dongshin/Shares/lib
    CACHE PATH "the root lib directory that contains mvIMPACT libraries" )

set(INCLUDE_INSTALL_DIR include/mvbluefox2 )
set(LIB_INSTALL_DIR lib/ )



include_directories (
    ${mvbluefox2_drive_SOURCE_DIR}/src
    ${mvbluefox2_drive_SOURCE_DIR}/include    
    ${mvIMPACT_INC_DIR}
    )
link_directories (
    ${mvbluefox2_drive_BINARY_DIR}/src
    ${mvIMPACT_LIB_DIR}
)


add_library(mvbluefox2 SHARED mvbluefox2_io.cpp
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
install (TARGETS mvbluefox2 EXPORT mvbluefox2-targets LIBRARY DESTINATION lib)


include(CMakePackageConfigHelpers)
configure_package_config_file(mvbluefox2Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mvbluefox2Config.cmake
  INSTALL_DESTINATION ${LIB_INSTALL_DIR}/mvbluefox2/cmake
  PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/mvbluefox2ConfigVersion.cmake
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mvbluefox2Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/mvbluefox2ConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/mvbluefox2/cmake )

