
#Save some context
install(CODE "set(CMAKE_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")

install(CODE "set(INSTALL_BIN_DIR \"${INSTALL_BIN_DIR}\")")
install(CODE "set(INSTALL_INC_DIR \"${INSTALL_INC_DIR}\")")
install(CODE "set(INSTALL_LIB_DIR \"${INSTALL_LIB_DIR}\")")

install(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/cleanup.cmake")
