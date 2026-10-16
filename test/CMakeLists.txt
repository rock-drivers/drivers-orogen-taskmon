rock_gtest(test_suite
    suite.cpp
    test_task.cpp
    DEPS_PKGCONFIG base-types orocos-rtt-${OROCOS_TARGET}
)

orogen_pkg_check_modules(LIBPROC2 libproc2)
if (LIBPROC2_FOUND)
    target_compile_definitions(test_suite PUBLIC USE_LIBPROC2)
    set(LIBPROC_LIBRARIES ${LIBPROC2_LIBRARIES})
else()
    find_path(FIND_LIBPROC2_INCLUDE_DIR libproc2/pids.h)
    find_library(FIND_LIBPROC2_LIBRARIES NAMES libproc2.so)
    if (FIND_LIBPROC2_INCLUDE_DIR AND FIND_LIBPROC2_LIBRARIES)
        target_compile_definitions(test_suite PUBLIC USE_LIBPROC2)
    endif()
endif()

target_link_libraries(test_suite 
    ${PROJECT_NAME}-tasks-${OROCOS_TARGET}
    ${LIBPROC_LIBRARIES}
)
