cmake_minimum_required(VERSION 3.29)
project(gpt_sovits_cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
  else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  endif ()
  set(CAFFE2_USE_CUDNN ON)
  set(USE_CUSPARSELT OFF)
endif ()

# torch
if (NOT DEFINED Torch_DIR)
  message(FATAL_ERROR "Torch_DIR not exist")
endif ()
set(Torch_Base_Dir ${Torch_DIR})
message("torch base dir: ${Torch_Base_Dir}")

set(CMAKE_PREFIX_PATH "${Torch_Base_Dir}" ${CMAKE_PREFIX_PATH})
find_package(Torch REQUIRED)
include_directories(${TORCH_INCLUDE_DIRS})
#link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1/lib64")

# 依赖库引入
set(CMAKE_CXX_STANDARD 17)
set(CPPMODULES $ENV{CPPMODULES})
set(CPPMODULE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CPPMODULE_JSON 1)
set(CPPMODULE_FMT 1)
set(CPPMODULE_CPPPINYIN 1)
set(CPPMODULE_BOOSTCMAKE 1)
set(CPPMODULE_BOOSTCMAKE_ENABLE_ALL 1)
set(CPPMODULE_TOKENIZERS 1)
set(CPPMODULE_UTFCPP 1)
set(CPPMODULE_SRELL 1)
set(CPPMODULE_LIBSNDFILE 1)
set(CPPMODULE_CPPJIEBA 1)
set(CPPMODULE_LIBSAMPLERATE 1)
set(CPPMODULE_CLD2 1)
set(CPPMODULE_XTL 1)
set(CPPMODULE_XTENSOR_BLAS 1)
set(CPPMODULE_XTENSOR 1)
set(CPPMODULE_GPTSOVITSCPP 1)
include(${CPPMODULES}/all.cmake)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

AUX_SOURCE_DIRECTORY(cli GPT_SOVITS_TEST_CLI_SOURCE)
add_executable(gpt_sovits_test_cli ${GPT_SOVITS_TEST_CLI_SOURCE})
target_link_libraries(gpt_sovits_test_cli PRIVATE gpt_sovits_cpp_static)

AUX_SOURCE_DIRECTORY(clean_text GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE)
add_executable(gpt_sovits_test_clean_text ${GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE})
target_link_libraries(gpt_sovits_test_clean_text PRIVATE gpt_sovits_cpp_static)


AUX_SOURCE_DIRECTORY(detect_list_lang GPT_SOVITS_TEST_DETECT_LIST_LANG_SOURCE)
add_executable(gpt_sovits_test_detect_list_lang ${GPT_SOVITS_TEST_DETECT_LIST_LANG_SOURCE})
target_link_libraries(gpt_sovits_test_detect_list_lang PRIVATE gpt_sovits_cpp_static)


AUX_SOURCE_DIRECTORY(g2p GPT_SOVITS_TEST_G2P_SOURCE)
add_executable(gpt_sovits_test_g2p ${GPT_SOVITS_TEST_G2P_SOURCE})
target_link_libraries(gpt_sovits_test_g2p PRIVATE gpt_sovits_cpp_static)

