# Generated from orogen/lib/orogen/templates/tasks/CMakeLists.txt

orogen_pkg_check_modules(LIBPROC libprocps)
if (NOT LIBPROC_FOUND)
    find_path(FIND_LIBPROC_INCLUDE_DIR proc/readproc.h)
    find_library(FIND_LIBPROC_LIBRARIES NAMES libproc.so)
    message("${LIBPROC_LIBRARIES}")
    if (NOT FIND_LIBPROC_INCLUDE_DIR)
	message(FATAL_ERROR "cannot find the libproc include files")
    elseif (NOT FIND_LIBPROC_LIBRARIES)
	message(FATAL_ERROR "cannot find the libproc.so library")
    endif()
    set(LIBPROC_INCLUDE_DIR "${FIND_LIBPROC_INCLUDE_DIR}")
    set(LIBPROC_LIBRARIES "${FIND_LIBPROC_LIBRARIES}")
    include_directories(${FIND_LIBPROC_INCLUDE_DIR})
endif()

include(taskmonTaskLib)
ADD_LIBRARY(${TASKMON_TASKLIB_NAME} SHARED 
    ${TASKMON_TASKLIB_SOURCES})

add_dependencies(${TASKMON_TASKLIB_NAME}
    regen-typekit)

TARGET_LINK_LIBRARIES(${TASKMON_TASKLIB_NAME}
    ${OrocosRTT_LIBRARIES}
    ${TASKMON_TASKLIB_DEPENDENT_LIBRARIES}
    ${LIBPROC_LIBRARIES})
SET_TARGET_PROPERTIES(${TASKMON_TASKLIB_NAME}
    PROPERTIES LINK_INTERFACE_LIBRARIES "${TASKMON_TASKLIB_INTERFACE_LIBRARIES}")

INSTALL(TARGETS ${TASKMON_TASKLIB_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/orocos)

INSTALL(FILES ${TASKMON_TASKLIB_HEADERS}
    DESTINATION include/orocos/taskmon)

