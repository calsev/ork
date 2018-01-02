
project(glm CXX)

set(GLM_VERSION "0.9.8")


file(GLOB ROOT_INCLUDES "${3P_GLM_SRC_DIR}/*.[cpp|inl|hpp]")
file(GLOB_RECURSE CORE_INCLUDES "${3P_GLM_SRC_DIR}/detail/*.[cpp|inl|hpp]")
file(GLOB_RECURSE GTC_INCLUDES "${3P_GLM_SRC_DIR}/gtc/*.[cpp|inl|hpp]")
file(GLOB_RECURSE GTX_INCLUDES "${3P_GLM_SRC_DIR}/gtx/*.[cpp|inl|hpp]")
file(GLOB_RECURSE SIMD_INCLUDES "${3P_GLM_SRC_DIR}/simd/*.[cpp|inl|hpp]")


source_group("Core" FILES ${CORE_INCLUDES})
source_group("GTC" FILES ${GTC_INCLUDES})
source_group("GTX" FILES ${GTX_INCLUDES})
source_group("SIMD" FILES ${SIMD_INCLUDES})


add_library(glm INTERFACE
	${ROOT_INCLUDES}
	${CORE_INCLUDES}
	${GTC_INCLUDES}
	${GTX_INCLUDES}
	${SIMD_INCLUDES}
)


target_include_directories(
    glm
    INTERFACE $<BUILD_INTERFACE:${3P_GLM_SRC_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


install_header_dir("${3P_GLM_SRC_DIR}/glm" "${ORK_INSTALL_INC_DIR}/glm" "*.txt")
