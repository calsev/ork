
set_as_internal(BUILD_DEFINES "")

set_as_internal(BUILD_PKGCONFIG "OFF")
set_as_internal(BUILD_SHARED_LIBS "ON")
set_as_internal(BUILD_TESTS "OFF")


include(GNUInstallDirs)
set_as_internal(CMAKE_INSTALL_BINDIR "${INSTALL_BIN_DIR}")
set_as_internal(CMAKE_INSTALL_LIBDIR "${INSTALL_LIB_DIR}")
set_as_internal(CMAKE_INSTALL_INCLUDEDIR "${INSTALL_INC_DIR}")

mark_as_internal(CMAKE_INSTALL_DATADIR)
mark_as_internal(CMAKE_INSTALL_DATAROOTDIR)
mark_as_internal(CMAKE_INSTALL_DOCDIR)
mark_as_internal(CMAKE_INSTALL_INFODIR)
mark_as_internal(CMAKE_INSTALL_LIBEXECDIR)
mark_as_internal(CMAKE_INSTALL_LOCALEDIR)
mark_as_internal(CMAKE_INSTALL_LOCALSTATEDIR)
mark_as_internal(CMAKE_INSTALL_MANDIR)
mark_as_internal(CMAKE_INSTALL_OLDINCLUDEDIR)
mark_as_internal(CMAKE_INSTALL_SBINDIR)
mark_as_internal(CMAKE_INSTALL_SHAREDSTATEDIR)
mark_as_internal(CMAKE_INSTALL_SYSCONFDIR)


set_as_internal(STATIC_CRT "OFF")
set_as_internal(SKIP_INSTALL_EXPORT "ON")


add_subdirectory("${3P_PUGI_SRC_DIR}" "${3P_PUGI_BIN_DIR}/pugi")
