
project(catch CXX)

add_library(catch INTERFACE)


target_include_directories(
    catch
    INTERFACE $<BUILD_INTERFACE:${3P_CATCH_SRC_DIR}/single_include>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)


install_headers("${3P_CATCH_SRC_DIR}/single_include/catch.hpp" "${ORK_INSTALL_INC_DIR}")
