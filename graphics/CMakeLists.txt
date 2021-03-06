SET( PROJECT "graphics" )

IF( WIN32 )
    FIND_PACKAGE( OpenGL )
ELSE( WIN32 )
    FIND_LIBRARY( OPENGL_LIBRARY NAMES GL REQUIRED )
ENDIF( WIN32 )

FIND_PACKAGE(Qt5Widgets QUIET)
IF(Qt5Widgets_FOUND)
  FIND_LIBRARY( Qt3D_LIB NAMES Qt53D REQUIRED )
ELSE(Qt5Widgets_FOUND)
  FIND_PACKAGE( Qt4 COMPONENTS QtCore QtGui QtOpenGl QtXml REQUIRED )
  INCLUDE( ${QT_USE_FILE} )
FIND_LIBRARY( Qt3D_LIB NAMES Qt3D REQUIRED )
ENDIF(Qt5Widgets_FOUND)


SET( dir ${SOURCE_CODE_BASE_DIR}/${PROJECT} )
FILE( GLOB includes ${dir}/*.h )
INSTALL( FILES ${includes} DESTINATION ${snark_INSTALL_INCLUDE_DIR}/${PROJECT} )

IF( snark_BUILD_APPLICATIONS )
    ADD_SUBDIRECTORY( applications )
ENDIF( snark_BUILD_APPLICATIONS )

ADD_SUBDIRECTORY( qt3d )
