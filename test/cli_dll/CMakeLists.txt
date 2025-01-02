cmake_minimum_required(VERSION 3.25)
project(gpt_sovits_cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
  else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
  endif ()
  set(CAFFE2_USE_CUDNN ON)
  set(USE_CUSPARSELT OFF)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(GPTSOVITS_BUILD_SHARED 1)
include_directories(../../include)
add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/third_party)

AUX_SOURCE_DIRECTORY(. GPT_SOVITS_TEST_CLI_DLL_SOURCE)
add_executable(gpt_sovits_test_cli_dll ${GPT_SOVITS_TEST_CLI_DLL_SOURCE})
set_target_properties(gpt_sovits_test_cli_dll PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(gpt_sovits_test_cli_dll PRIVATE gpt_sovits)
target_compile_options(gpt_sovits_test_cli_dll PRIVATE /utf-8)

if(MSVC)
  set_target_properties(gpt_sovits_test_cli_dll PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
  )
  add_custom_command(TARGET gpt_sovits_test_cli_dll POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_BINARY_DIR}/res
      ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE_INIT}/res
  )
  file(GLOB COPY_DLLS "${CMAKE_CURRENT_BINARY_DIR}/*.dll")
  add_custom_command(TARGET gpt_sovits_test_cli_dll POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${COPY_DLLS}
      ${CMAKE_CURRENT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE_INIT}
      COMMENT "MV Libs to MSVC Runtime"
  )
endif ()

