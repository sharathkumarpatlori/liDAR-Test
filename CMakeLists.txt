include_directories(E:\libLAS-1.8.1\include)
include_directories(E:\libLAS-1.8.1\include\liblas\capi)
set(LASINFO lasinfo)
set(LAS2LAS las2las)
set(LAS2TXT las2txt)

if (NOT WIN32)
    set(LAS2COL las2col )
    set(LAS2PG las2pg )
endif()

set(BIGFILE_TEST bigfile_test)
set(MEMLEAK_TEST memleak_test)
set(LASINDEX_TEST lasindex_test)

if(Boost_IOSTREAMS_FOUND)
  set(BIGFILE_BIO_TEST bigfile_boost_iostreams_test)
endif()

# Set the build type to release if it is not explicitly set by the user and
# isn't in the cache yet
if (NOT CMAKE_BUILD_TYPE )
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Utilities depending on 3rd-pary libraries
if(GDAL_FOUND)
    set(LAS2OGR las2ogr)
endif()


set(LIBLAS_UTILITIES
    ${LASINFO} ${LAS2LAS} ${TXT2LAS} ${LAS2COL}  ${LAS2PG}
    ${LAS2OGR} ${LAS2LAS} ${LASBLOCK} ${TS2LAS}  ${LAS2TXT})

set(VCPROJ_USER_REMOTE_MACHINE_DEBUG ${MACHINE_NAME})
set(VCPROJ_USER_ENVIRONMENT_DEBUG "${ENVIRONMENT_PATH}")

if(MSVC)
    foreach(utility ${LIBLAS_UTILITIES})
        set(USER_FILE ${utility}.vcproj.$ENV{USERDOMAIN}.$ENV{USERNAME}.user)
        set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
        message(STATUS "Generating ${CMAKE_GENERATOR} user-specific settings in ${USER_FILE}")
        configure_file(${CMAKE_SOURCE_DIR}/cmake/libLAS.vcproj.user.template ${OUTPUT_PATH} @ONLY)
    endforeach()
endif()

# Configure build targets

if(WIN32)
    add_definitions("-DLAS_DLL_EXPORT=1")
endif()


set(APPS_CPP_DEPENDENCIES
    las
)



if(LAS2LAS)
    add_executable(${LAS2LAS} las2las.cpp laskernel.cpp)
    target_link_libraries(${LAS2LAS} ${APPS_CPP_DEPENDENCIES} )
endif()

if(LASINFO)
    add_executable(${LASINFO} lasinfo.cpp laskernel.cpp )
    target_link_libraries(${LASINFO} ${APPS_CPP_DEPENDENCIES}  )
endif()

if(LAS2TXT)
    add_executable( ${LAS2TXT}  las2txt.cpp laskernel.cpp )
    target_link_libraries(${LAS2TXT} ${APPS_CPP_DEPENDENCIES}  )
endif()


if(BIGFILE_TEST)
    add_executable(${BIGFILE_TEST} bigtest.c)
    target_link_libraries(${BIGFILE_TEST} las_c)
endif()

if(MEMLEAK_TEST)
    add_executable(${MEMLEAK_TEST} memleak_test.c)
    target_link_libraries(${MEMLEAK_TEST} las_c)
endif()


if (LASINDEX_TEST)
    add_executable(${LASINDEX_TEST} lasindex_test.cpp)
    target_link_libraries(${LASINDEX_TEST} ${APPS_CPP_DEPENDENCIES})
endif()

if(BIGFILE_BIO_TEST)
    add_executable(${BIGFILE_BIO_TEST} bigfile_boost_iostreams_test.cpp)
    target_link_libraries(${BIGFILE_BIO_TEST} ${APPS_CPP_DEPENDENCIES} )
endif()

###############################################################################
# Targets installation

install(TARGETS ${LIBLAS_UTILITIES}
    RUNTIME DESTINATION ${LIBLAS_BIN_DIR}
    LIBRARY DESTINATION ${LIBLAS_LIB_DIR}
    ARCHIVE DESTINATION ${LIBLAS_LIB_DIR})

endif()