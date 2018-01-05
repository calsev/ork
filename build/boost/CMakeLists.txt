
project(boost CXX)

file(GLOB_RECURSE BOOST_INCLUDES "${3P_BOOST_SRC_DIR}/*.[h|hpp|inl]")


add_library(boost INTERFACE
	${BOOST_INCLUDES}
)


target_include_directories(
    boost
    INTERFACE $<BUILD_INTERFACE:${3P_BOOST_SRC_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


install_header_dir("${3P_BOOST_SRC_DIR}/boost" "${ORK_INSTALL_INC_DIR}/boost" "*.[c|cpp]")
