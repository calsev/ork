
if(ORK_BUILD_OCE)
	install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_PREFIX}/cmake\")")
	install(CODE "file(RENAME \"${CMAKE_INSTALL_PREFIX}/include/oce\" \"${INSTALL_INC_DIR}\")")
	install(CODE "file(REMOVE \"${CMAKE_INSTALL_PREFIX}/include\")")#NOT recursive so we detect other files
	install(CODE "file(REMOVE_RECURSE \"${INSTALL_BIN_DIR}/share/oce\")")
	install(CODE "file(REMOVE \"${INSTALL_BIN_DIR}/share\")")#NOT recursive so we detect other files
	
	file(GLOB OCE_DLLS
		"${CMAKE_INSTALL_PREFIX}/Win64/bin/*.dll"
	)
	install(CODE "file(RENAME \"${OCE_DLLS}\" \"${INSTALL_BIN_DIR}\")")
	
	file(GLOB OCE_PDBS
		"${CMAKE_INSTALL_PREFIX}/Win64/lib/*.pdb"
	)
	install(CODE "file(RENAME \"${OCE_PDBS}\" \"${INSTALL_BIN_DIR}\")")
	
	file(GLOB OCE_LIBS
		"${CMAKE_INSTALL_PREFIX}/Win64/lib/*.lib"
	)
	install(CODE "file(RENAME \"${OCE_LIBS}\" \"${INSTALL_LIB_DIR}\")")
	
	install(CODE "file(REMOVE_RECURSE \"${CMAKE_INSTALL_PREFIX}/Win64\")")
endif()
