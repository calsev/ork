
project(yamlcpp CXX)

set(VERSION 0.5.3)

set_as_internal(CMAKE_CXX_STANDARD "14") # YAML has become dated
if(MSVC)
	append_all_compiler_flag("/wd4127") #Conditional expression is constant
	append_all_compiler_flag("/wd4251") #Inconsistent DLL linkage
	append_all_compiler_flag("/wd4267") #Narrowing conversion
	append_all_compiler_flag("/wd4355") #'this' used in base member initializer list
endif()
add_definitions(-DYAML_CPP_NO_CONTRIB)
add_definitions(-DYAML_CPP_DLL)
add_definitions(-Dyaml_cpp_EXPORTS)


file(GLOB YAML_PUBLIC_HDRS
	"${3P_YAML_SRC_DIR}/src/[a-zA-Z]*.cpp"
)
file(GLOB_RECURSE YAML_PRIVATE_HDRS
	"${3P_YAML_SRC_DIR}/src/[a-zA-Z]*.h"
)
file(GLOB YAML_SRCS
	"${3P_YAML_SRC_DIR}/include/yaml-cpp/[a-zA-Z]*.h"
)


link_directories("${3P_BOOST_LIB_DIR}") #Must be before target
add_library(yamlcpp SHARED ${YAML_PUBLIC_HDRS} ${YAML_PRIVATE_HDRS} ${YAML_SRCS})

target_include_directories(
    yamlcpp
    PUBLIC $<BUILD_INTERFACE:${3P_YAML_SRC_DIR}/include>
    PUBLIC "${3P_BOOST_SRC_DIR}"
    PRIVATE $<BUILD_INTERFACE:${3P_YAML_SRC_DIR}/src>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)

install_bin(yamlcpp)
install_header_dir("${3P_YAML_SRC_DIR}/include/yaml-cpp" "${ORK_INSTALL_INC_DIR}/yaml-cpp" "contrib")
