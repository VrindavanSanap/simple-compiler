cmake_minimum_required(VERSION 3.20)

find_program(CLANG   clang   NAMES clang   cc )
find_program(CLANGXX clang++ NAMES clang++ c++)
set(CMAKE_C_COMPILER   ${CLANG}   CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER ${CLANGXX} CACHE STRING "" FORCE)

project(scull LANGUAGES C CXX)

include(ExternalProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER   clang  CACHE STRING "")
set(CMAKE_CXX_COMPILER clang++ CACHE STRING "")

##########
#  LLVM  #
##########

set(LLVM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/llvm-project/llvm)
set(LLVM_BUILD_DIR  ${CMAKE_SOURCE_DIR}/external/llvm-project/build)
set(LLVM_LIB        ${LLVM_BUILD_DIR}/lib)
set(LLVM_INC        ${LLVM_BUILD_DIR}/include)
set(LLVM_CONFIG     ${LLVM_BUILD_DIR}/bin/llvm-config)
set(LLVM_SENTINEL   ${LLVM_LIB}/libLLVM.so)

add_custom_target(llvm-sync
  COMMAND git submodule sync --recursive
  COMMAND git submodule update --init --recursive --depth 1
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "[LLVM] Submodule synced"
)

cmake_host_system_information(RESULT NPROC QUERY NUMBER_OF_LOGICAL_CORES)

ExternalProject_Add(llvm_project
  SOURCE_DIR       ${LLVM_SOURCE_DIR}
  BINARY_DIR       ${LLVM_BUILD_DIR}
  INSTALL_COMMAND  ""
  CMAKE_GENERATOR  Ninja
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=Release
    -DLLVM_TARGETS_TO_BUILD=all
    -DLLVM_ENABLE_RTTI=ON
    -DLLVM_ENABLE_EH=ON
    -DLLVM_ENABLE_TERMINFO=OFF
    -DLLVM_ENABLE_ZLIB=OFF
    -DLLVM_ENABLE_ZSTD=OFF
    -DLLVM_ENABLE_LIBXML2=OFF
    -DLLVM_INCLUDE_TESTS=OFF
    -DLLVM_INCLUDE_EXAMPLES=OFF
    -DLLVM_INCLUDE_BENCHMARKS=OFF
    -DLLVM_BUILD_LLVM_DYLIB=ON
    -DLLVM_LINK_LLVM_DYLIB=ON
    -DBUILD_SHARED_LIBS=OFF
  BUILD_COMMAND    ninja -j ${NPROC}
  BUILD_BYPRODUCTS ${LLVM_SENTINEL}
  STAMP_DIR        ${CMAKE_BINARY_DIR}/llvm-stamps
)

if(NOT EXISTS ${LLVM_SENTINEL})
  message(STATUS "[INFO] LLVM not built yet — only llvm-sync and llvm_project are available.")
  message(STATUS "[INFO] Build LLVM first, then re-run cmake to configure sclc targets.")
  return()
endif()

execute_process(COMMAND ${LLVM_CONFIG} --cxxflags
  OUTPUT_VARIABLE _LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --system-libs
  OUTPUT_VARIABLE _LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

separate_arguments(LLVM_CXXFLAGS    UNIX_COMMAND ${_LLVM_CXXFLAGS})
separate_arguments(LLVM_SYSTEM_LIBS UNIX_COMMAND ${_LLVM_SYSTEM_LIBS})

###########
#  sclc   #
###########

set(INC_DIR ${CMAKE_SOURCE_DIR}/sclc/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/sclc/src)

file(GLOB_RECURSE C_SRCS   CONFIGURE_DEPENDS ${SRC_DIR}/*.c)
file(GLOB_RECURSE CXX_SRCS CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)

function(add_sclc_target NAME OUTDIR)
  cmake_parse_arguments(A "" "" "CFLAGS" ${ARGN})

  add_executable(${NAME} ${C_SRCS} ${CXX_SRCS})

  set_target_properties(${NAME} PROPERTIES
    C_STANDARD               23
    CXX_STANDARD             17
    CXX_STANDARD_REQUIRED    ON
    RUNTIME_OUTPUT_DIRECTORY ${OUTDIR}
    BUILD_RPATH              "$ORIGIN/../external/llvm-project/build/lib"
  )

  target_include_directories(${NAME} PRIVATE
    ${INC_DIR}
    ${LLVM_INC}
  )

  target_compile_options(${NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:   ${A_CFLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>: ${A_CFLAGS} ${LLVM_CXXFLAGS} -Wno-unused-parameter>
  )

  target_link_directories(${NAME} PRIVATE ${LLVM_LIB})
  target_link_libraries(${NAME} PRIVATE LLVM ${LLVM_SYSTEM_LIBS} m)

  add_dependencies(${NAME} llvm_project)
endfunction()

add_sclc_target(sclc ${CMAKE_BINARY_DIR}/sclc-bin
  CFLAGS -g -O0 -Wall -Wextra
)

add_sclc_target(sclc_release ${CMAKE_BINARY_DIR}/sclc-bin-release
  CFLAGS -O2 -DNDEBUG -Wall -Werror -Wextra
)

add_custom_target(do-install
  COMMAND sudo cp $<TARGET_FILE:sclc_release> /usr/local/bin/sclc
  DEPENDS sclc_release
  COMMENT "[INSTALL] $<TARGET_FILE:sclc_release> -> /usr/local/bin/sclc"
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
