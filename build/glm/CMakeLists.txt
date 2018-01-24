
project(glm CXX)

add_library(glm INTERFACE)


target_include_directories(
    glm
    INTERFACE $<BUILD_INTERFACE:${3P_GLM_SRC_DIR}>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


install_header_dir("${3P_GLM_SRC_DIR}/glm" "${ORK_INSTALL_INC_DIR}/glm" "*.txt")
