set( dir ${SOURCE_CODE_BASE_DIR}/comms/pcan/applications )
file( GLOB source   ${dir}/*.cpp)
file( GLOB includes ${dir}/*.h)
source_group( ${TARGET_NAME} FILES ${source} ${includes} )
add_executable( pcan-cat pcan-cat.cpp )
target_link_libraries( pcan-cat ${snark_ALL_EXTERNAL_LIBRARIES} ) #pcan )
install( TARGETS pcan-cat RUNTIME DESTINATION ${snark_INSTALL_BIN_DIR} COMPONENT Runtime )
