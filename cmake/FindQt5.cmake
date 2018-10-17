# - Try to find QT5
# -------------------------
# Once done this will define
#
# Qt5_FOUND       - system has Qt5 library
# Qt5_VERSION     - Qt5 version
# Qt5_INCLUDE_DIR - Qt5 include directory
# Qt5_LIBRARY     - Qt5 library directory

find_package(PkgConfig QUIET)

if (PkgConfig_FOUND)
    pkg_check_modules(QT5 Qt5Core REQUIRED)

    if (QT5_VERSION)
        set(Qt5_VERSION ${_QT5_VERSION})
        message("Found Qt5, version: ${QT5_VERSION}")
        message("           include: ${QT5_INCLUDEDIR}")
        message("           library: ${QT5_LIBDIR}")
        message("           libraries: ${QT5_LIBRARIES}")

    else(QT5_VERSION)
        set(Qt5_VERSION "")
        message("Couldn't determine Qt5 version.")
    endif(QT5_VERSION)
else(PkgConfig_FOUND)
        message("Couldn't find Qt5Core. pkg-config required but NOT installed.")
endif(PkgConfig_FOUND)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Qt5
    FOUND_VAR QT5_FOUND
    REQUIRED_VARS QT5_LIBDIR QT5_INCLUDEDIR
    VERSION_VAR QT5_VERSION
    )

mark_as_advanced(
    QT5_INCLUDE_DIR
    QT5_LIBDIR
    QT5_INCLUDE_DIR
    )
