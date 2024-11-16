cmake_minimum_required(VERSION 3.29)
project(gpt_sovits_cpp)


# 依赖库引入
set(CMAKE_CXX_STANDARD 20)
set(CPPMODULES $ENV{CPPMODULES})
set(CPPMODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CPPMODULE_JSON 1)
set(CPPMODULE_CPPPINYIN 1)
set(CPPMODULE_BOOSTCMAKE 1)
set(CPPMODULE_BOOSTCMAKE_ENABLE_ALL 1)
set(CPPMODULE_TOKENIZERS 1)
set(CPPMODULE_UTFCPP 1)
set(CPPMODULE_SRELL 1)
set(CPPMODULE_LIBSNDFILE 1)
set(CPPMODULE_CPPJIEBA 1)
set(CPPMODULE_LIBSAMPLERATE 1)
include(${CPPMODULES}/all.cmake)

# torch
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(Tarch_Base_Dir "F:/Engcode/c_c++/modules/libtorch/debug")
else ()
  set(Tarch_Base_Dir "F:/Engcode/c_c++/modules/libtorch/release")
endif ()
set(CMAKE_PREFIX_PATH "${Tarch_Base_Dir}" ${CMAKE_PREFIX_PATH})
find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})
link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1/lib64")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
#include_directories(${Tarch_Base_Dir}/include)

include_directories(./include)
AUX_SOURCE_DIRECTORY(src GPT_SOVITS_CPP_SOURCE)

add_library(gpt_sovits_cpp_static STATIC ${GPT_SOVITS_CPP_SOURCE})
target_link_libraries(gpt_sovits_cpp_static PRIVATE ${TORCH_LIBRARIES} ${CPPMODULE_LINK_LIBRARIES_ALL})
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET gpt_sovits_cpp_static
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${TORCH_DLLS}
      $<TARGET_FILE_DIR:gpt_sovits_cpp_static>)
endif (MSVC)

# copy res
add_custom_command(TARGET gpt_sovits_cpp_static POST_BUILD
    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    ${CMAKE_BINARY_DIR}/res
)

AUX_SOURCE_DIRECTORY(test/cli GPT_SOVITS_TEST_CLI_SOURCE)
add_executable(gpt_sovits_test_cli ${GPT_SOVITS_TEST_CLI_SOURCE})
target_link_libraries(gpt_sovits_test_cli PRIVATE gpt_sovits_cpp_static)

AUX_SOURCE_DIRECTORY(test/clean_text GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE)
add_executable(gpt_sovits_test_clean_text ${GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE})
target_link_libraries(gpt_sovits_test_clean_text PRIVATE gpt_sovits_cpp_static)
