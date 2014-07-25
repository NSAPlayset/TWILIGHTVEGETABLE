add_subdirectory(decoder)
add_subdirectory(libbase64cc)
add_subdirectory(libnetwork)


########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
include_directories(${Boost_INCLUDE_DIR} ${GNURADIO_RUNTIME_INCLUDE_DIRS} ${GNURADIO_UHD_INCLUDE_DIRS} ${OSMOCORE_INCLUDES} ${ZeroMQ_INCLUDE_DIR} decoder decoder/openbtsdecoder)
link_directories(${Boost_LIBRARY_DIRS} ${GNURADIO_RUNTIME_LIBRARY_DIRS} ${OSMOCOM_LIBRARY_DIRS})


list(APPEND npgsm_sources
    base64.c
    gsm_receiver_cf.cc
    gsm_receiver_cf2.cc
    gsm_receiver_config.cc
#    gsm_receiver_top_block.cc
    SDRconfiguration.cc
    SDRini.c
    SDRlog.cc
    viterbi_detector.cc
    ${PROTO_SRC}
)

add_library(gnuradio-npgsm SHARED ${npgsm_sources})
target_link_libraries(gnuradio-npgsm ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES} ${OSMOCORE_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ZeroMQ_LIBRARIES} decoder openbtsdecoder libnetwork libbase64cc)
set_target_properties(gnuradio-npgsm PROPERTIES DEFINE_SYMBOL "gnuradio_npgsm_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-npgsm
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
#include(GrTest)
#
#include_directories(${CPPUNIT_INCLUDE_DIRS})
#
#list(APPEND test_npgsm_sources
#    ${CMAKE_CURRENT_SOURCE_DIR}/test_npgsm.cc
#    ${CMAKE_CURRENT_SOURCE_DIR}/qa_npgsm.cc
#)
#
#add_executable(test-acars ${test_npgsm_sources})
#
#target_link_libraries(
#  test-npgsm
#  ${GNURADIO_RUNTIME_LIBRARIES}
#  ${Boost_LIBRARIES}
#  ${CPPUNIT_LIBRARIES}
#  gnuradio-npgsm
#)
#
#GR_ADD_TEST(test_npgsm test-npgsm)
