
#Save some context
install(CODE "set(CMAKE_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")

install(CODE "set(ORK_INSTALL_BIN_DIR \"${ORK_INSTALL_BIN_DIR}\")")
install(CODE "set(ORK_INSTALL_INC_DIR \"${ORK_INSTALL_INC_DIR}\")")
install(CODE "set(ORK_INSTALL_LIB_DIR \"${ORK_INSTALL_LIB_DIR}\")")

install(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cleanup.cmake")
