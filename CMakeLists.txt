cmake_minimum_required(VERSION 3.20)

find_program(CLANG   clang   NAMES clang   cc )
find_program(CLANGXX clang++ NAMES clang++ c++)
set(CMAKE_C_COMPILER   ${CLANG}   CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER ${CLANGXX} CACHE STRING "" FORCE)

project(scull LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##########
#  LLVM  #
##########

# Auto-detect brew-installed LLVM on macOS when LLVM_DIR is not explicitly set
if(APPLE AND NOT DEFINED LLVM_DIR)
  find_program(BREW brew)
  if(BREW)
    execute_process(
      COMMAND ${BREW} --prefix llvm
      OUTPUT_VARIABLE _BREW_LLVM_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(_BREW_LLVM_PREFIX AND EXISTS "${_BREW_LLVM_PREFIX}/lib/cmake/llvm")
      set(LLVM_DIR "${_BREW_LLVM_PREFIX}/lib/cmake/llvm" CACHE PATH "LLVM CMake directory")
    endif()
  endif()
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "[LLVM] Found ${LLVM_PACKAGE_VERSION} at ${LLVM_DIR}")

include(AddLLVM)

################
# Build Types  #
################

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(_DEBUG_FLAGS  "-g -O0 -fsanitize=address,undefined -fsanitize-address-use-after-scope -fno-omit-frame-pointer -DDEBUG -Wall -Wextra")
set(_RELEASE_FLAGS "-O2 -DNDEBUG -Wall -Werror -Wextra")

set(CMAKE_C_FLAGS_DEBUG   "${_DEBUG_FLAGS}"   CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${_RELEASE_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG   "${_DEBUG_FLAGS}"  CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${_RELEASE_FLAGS}" CACHE STRING "" FORCE)

###########
#  core   #
###########

set(CORE_INC_DIR ${CMAKE_SOURCE_DIR}/core/include)
set(CORE_SRC_DIR ${CMAKE_SOURCE_DIR}/core/src)

file(GLOB_RECURSE CORE_C_SRCS   CONFIGURE_DEPENDS ${CORE_SRC_DIR}/*.c)
file(GLOB_RECURSE CORE_CXX_SRCS CONFIGURE_DEPENDS ${CORE_SRC_DIR}/*.cpp)

add_library(core STATIC ${CORE_C_SRCS} ${CORE_CXX_SRCS})

set_target_properties(core PROPERTIES
  C_STANDARD            23
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(core PUBLIC ${CORE_INC_DIR})

target_compile_options(core PRIVATE
  $<$<COMPILE_LANGUAGE:C>:   ${CMAKE_C_FLAGS}>
  $<$<COMPILE_LANGUAGE:CXX>: ${CMAKE_CXX_FLAGS}>
)

###############
#  frontend   #
###############

set(FRONTEND_INC_DIR ${CMAKE_SOURCE_DIR}/frontend/include)
set(FRONTEND_SRC_DIR ${CMAKE_SOURCE_DIR}/frontend/src)

file(GLOB_RECURSE FRONTEND_C_SRCS   CONFIGURE_DEPENDS ${FRONTEND_SRC_DIR}/*.c)
file(GLOB_RECURSE FRONTEND_CXX_SRCS CONFIGURE_DEPENDS ${FRONTEND_SRC_DIR}/*.cpp)

add_library(frontend STATIC ${FRONTEND_C_SRCS} ${FRONTEND_CXX_SRCS})

set_target_properties(frontend PROPERTIES
  C_STANDARD            23
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
)

target_include_directories(frontend PUBLIC ${FRONTEND_INC_DIR})
target_link_libraries(frontend PRIVATE core)

target_compile_options(frontend PRIVATE
  $<$<COMPILE_LANGUAGE:C>:   ${CMAKE_C_FLAGS}>
  $<$<COMPILE_LANGUAGE:CXX>: ${CMAKE_CXX_FLAGS}>
)

###########
#  sclc   #
###########

set(INC_DIR ${CMAKE_SOURCE_DIR}/sclc/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/sclc/src)

file(GLOB_RECURSE C_SRCS   CONFIGURE_DEPENDS ${SRC_DIR}/*.c)
file(GLOB_RECURSE CXX_SRCS CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)

add_executable(sclc ${C_SRCS} ${CXX_SRCS})

set_target_properties(sclc PROPERTIES
  C_STANDARD               23
  CXX_STANDARD             17
  CXX_STANDARD_REQUIRED    ON
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/sclc-bin
  BUILD_RPATH              "${LLVM_LIBRARY_DIR}"
  INSTALL_RPATH            "${LLVM_LIBRARY_DIR}"
)

target_include_directories(sclc PRIVATE
  ${INC_DIR}
  ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(sclc PRIVATE ${LLVM_DEFINITIONS})

target_compile_options(sclc PRIVATE
  $<$<COMPILE_LANGUAGE:C>:   ${CMAKE_C_FLAGS}>
  $<$<COMPILE_LANGUAGE:CXX>: ${CMAKE_CXX_FLAGS} -Wno-unused-parameter>
)

target_link_directories(sclc PRIVATE ${LLVM_LIBRARY_DIR})
target_link_libraries(sclc PRIVATE core frontend LLVM m)

add_custom_target(do-install
  COMMAND sudo cp $<TARGET_FILE:sclc> /usr/local/bin/sclc
  DEPENDS sclc
  COMMENT "[INSTALL] $<TARGET_FILE:sclc> -> /usr/local/bin/sclc"
)

############
# Examples #
############

set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
file(GLOB_RECURSE EXAMPLE_SRCS CONFIGURE_DEPENDS ${EXAMPLES_DIR}/*.scl)

foreach(example_src ${EXAMPLE_SRCS})
  file(RELATIVE_PATH _rel ${EXAMPLES_DIR} ${example_src})
  string(REPLACE "/" "_" _tname "${_rel}")
  string(REPLACE ".scl" "" _tname "${_tname}")

  add_custom_target(example_${_tname}
    COMMAND $<TARGET_FILE:sclc> -i ${CMAKE_SOURCE_DIR}/lib ${example_src}
    DEPENDS sclc
    COMMENT "[SCLC] ${example_src}"
  )
  list(APPEND _EXAMPLE_TARGETS example_${_tname})
endforeach()

add_custom_target(examples
  DEPENDS ${_EXAMPLE_TARGETS}
  COMMENT "[INFO] Examples compiled successfully"
)
