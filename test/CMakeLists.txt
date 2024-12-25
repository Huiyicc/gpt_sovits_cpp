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
add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/third_party)

AUX_SOURCE_DIRECTORY(cli GPT_SOVITS_TEST_CLI_SOURCE)
add_executable(gpt_sovits_test_cli ${GPT_SOVITS_TEST_CLI_SOURCE} ${CPPMODULE_LINK_SOURCES})
target_link_libraries(gpt_sovits_test_cli PRIVATE gpt_sovits_cpp_static)

AUX_SOURCE_DIRECTORY(clean_text GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE)
add_executable(gpt_sovits_test_clean_text ${GPT_SOVITS_TEST_CLEAN_TEXT_SOURCE} ${CPPMODULE_LINK_SOURCES})
target_link_libraries(gpt_sovits_test_clean_text PRIVATE gpt_sovits_cpp_static)


AUX_SOURCE_DIRECTORY(detect_list_lang GPT_SOVITS_TEST_DETECT_LIST_LANG_SOURCE)
add_executable(gpt_sovits_test_detect_list_lang ${GPT_SOVITS_TEST_DETECT_LIST_LANG_SOURCE})
target_link_libraries(gpt_sovits_test_detect_list_lang PRIVATE gpt_sovits_cpp_static)


AUX_SOURCE_DIRECTORY(g2p GPT_SOVITS_TEST_G2P_SOURCE)
add_executable(gpt_sovits_test_g2p ${GPT_SOVITS_TEST_G2P_SOURCE})
target_link_libraries(gpt_sovits_test_g2p PRIVATE gpt_sovits_cpp_static)

