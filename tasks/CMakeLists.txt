# Generated from orogen/lib/orogen/templates/tasks/CMakeLists.txt

find_path(LIBPROC_INCLUDE_DIR proc/readproc.h)
find_library(LIBPROC_LIBRARY NAMES libproc.so)
if (NOT LIBPROC_INCLUDE_DIR)
    message(FATAL_ERROR "cannot find the libproc include files")
elseif (NOT LIBPROC_LIBRARY)
    message(FATAL_ERROR "cannot find the libproc.so library")
endif()
include_directories(${LIBPROC_INCLUDE_DIR})

include(taskmonTaskLib)
ADD_LIBRARY(${TASKMON_TASKLIB_NAME} SHARED 
    ${TASKMON_TASKLIB_SOURCES})

add_dependencies(${TASKMON_TASKLIB_NAME}
    regen-typekit)

TARGET_LINK_LIBRARIES(${TASKMON_TASKLIB_NAME}
    ${OrocosRTT_LIBRARIES}
    ${TASKMON_TASKLIB_DEPENDENT_LIBRARIES}
    ${LIBPROC_LIBRARY})
SET_TARGET_PROPERTIES(${TASKMON_TASKLIB_NAME}
    PROPERTIES LINK_INTERFACE_LIBRARIES "${TASKMON_TASKLIB_INTERFACE_LIBRARIES}")

INSTALL(TARGETS ${TASKMON_TASKLIB_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/orocos)

INSTALL(FILES ${TASKMON_TASKLIB_HEADERS}
    DESTINATION include/orocos/taskmon)

