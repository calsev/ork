
project(yaml CXX)

set(VERSION 0.5.3)

set_as_internal(CMAKE_CXX_STANDARD "14") # YAML has become dated
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127") #Conditional expression is constant
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") #Inconsistent DLL linkage
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267") #Narrowing conversion
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4355") #'this' used in base member initializer list
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
add_library(yaml SHARED ${YAML_PUBLIC_HDRS} ${YAML_PRIVATE_HDRS} ${YAML_SRCS})

target_include_directories(
    yaml
    PUBLIC $<BUILD_INTERFACE:${3P_YAML_SRC_DIR}/include>
    PUBLIC "${3P_BOOST_SRC_DIR}"
    PRIVATE $<BUILD_INTERFACE:${3P_YAML_SRC_DIR}/src>
    INTERFACE $<INSTALL_INTERFACE:${ORK_INSTALL_INC_DIR}>
)

install_bin(yaml)
install_header_dir("${3P_YAML_SRC_DIR}/include/yaml-cpp" "${ORK_INSTALL_INC_DIR}/yaml-cpp" "contrib")
