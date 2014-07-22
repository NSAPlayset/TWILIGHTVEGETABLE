add_subdirectory(decoder)
########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR} decoder decoder/openbtsstuff)
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND npgsm_sources
    gsm_receiver_cf.cc
    gsm_receiver_config.cc
    viterbi_detector.cc
)

add_library(gnuradio-npgsm SHARED ${npgsm_sources})
target_link_libraries(gnuradio-npgsm ${Boost_LIBRARIES} ${GNURADIO_RUNTIME_LIBRARIES})
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
