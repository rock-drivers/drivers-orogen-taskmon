# Generated from orogen/lib/orogen/templates/tasks/CMakeLists.txt

orogen_pkg_check_modules(PKG_PROC2 libproc2)
find_path(PATH_PROC2_INC libproc2/pids.h)
find_library(PATH_PROC2_LIB NAMES libproc2.so libproc2)

orogen_pkg_check_modules(PKG_PROCPS libprocps)
find_path(PATH_PROCPS_INC proc/readproc.h)
find_library(PATH_PROCPS_LIB NAMES procps libproc.so libproc)

if (PKG_PROC2_FOUND OR (PATH_PROC2_INC AND PATH_PROC2_LIB))
    set(LIBPROC_INCLUDE_DIR ${PKG_PROC2_INCLUDE_DIRS} ${PATH_PROC2_INC})
    set(LIBPROC_LIBRARIES ${PKG_PROC2_LIBRARIES} ${PATH_PROC2_LIB})
    add_definitions(-DUSE_LIBPROC2)
    message(STATUS "taskmon: using libproc2")
elseif (PKG_PROCPS_FOUND OR (PATH_PROCPS_INC AND PATH_PROCPS_LIB))
    set(LIBPROC_INCLUDE_DIR ${PKG_PROCPS_INCLUDE_DIRS} ${PATH_PROCPS_INC})
    set(LIBPROC_LIBRARIES ${PKG_PROCPS_LIBRARIES} ${PATH_PROCPS_LIB})
    message(STATUS "taskmon: using libprocps")
else()
    message(FATAL_ERROR
            "Cannot find libproc2 or libprocps headers and libraries.")
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
    ${LIBPROC_LIBRARIES})
SET_TARGET_PROPERTIES(${TASKMON_TASKLIB_NAME} PROPERTIES
                      LINK_INTERFACE_LIBRARIES
                      "${TASKMON_TASKLIB_INTERFACE_LIBRARIES}")

INSTALL(TARGETS ${TASKMON_TASKLIB_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib/orocos)

INSTALL(FILES ${TASKMON_TASKLIB_HEADERS}
    DESTINATION include/orocos/taskmon)
