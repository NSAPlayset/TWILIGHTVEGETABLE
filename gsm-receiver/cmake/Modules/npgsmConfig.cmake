INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NPGSM npGSM)

FIND_PATH(
    NPGSM_INCLUDE_DIRS
    NAMES npgsm/api.h
    HINTS $ENV{NPGSM_DIR}/include
        ${PC_NPGSM_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NPGSM_LIBRARIES
    NAMES gnuradio-npgsm
    HINTS $ENV{ACARS_DIR}/lib
        ${PC_NPGSM_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NPGSM DEFAULT_MSG NPGSM_LIBRARIES NPGSM_INCLUDE_DIRS)
MARK_AS_ADVANCED(NPGSM_LIBRARIES NPGSM_INCLUDE_DIRS)
