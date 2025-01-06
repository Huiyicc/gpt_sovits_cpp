cmake_minimum_required(VERSION 3.25)

set(GPTSOVITS_PROJECT_NAME gpt_sovits)
project(${GPTSOVITS_PROJECT_NAME})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
  else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
  endif ()
  set(CAFFE2_USE_CUDNN ON)
  set(USE_CUSPARSELT OFF)
endif ()

# 引入相关依赖
include(cmake/base.cmake)
include(cmake/JSON.cmake)
include(cmake/fmt.cmake)
include(cmake/cpp-pinyin.cmake)
set(CPPMODULE_BOOSTCMAKE_ENABLE_ALL 1)
include(cmake/boost-cmake.cmake)
include(cmake/tokenizers-cpp.cmake)
include(cmake/utfcpp.cmake)
include(cmake/SRELL.cmake)
include(cmake/libsndfile.cmake)
include(cmake/cppjieba.cmake)
include(cmake/libsamplerate.cmake)
include(cmake/cld2-cmake.cmake)
include(cmake/xtl.cmake)
include(cmake/xtensor.cmake)
include(cmake/xtensor-blas.cmake)


message("Torch_DIR=${Torch_DIR}")
# 检查torch定义
if (DEFINED Torch_DIR)
  set(Torch_Base_Dir ${Torch_DIR})
else ()
  message(FATAL_ERROR "Please specify the torch directory with -DTorch_DIR=<path/to/torch>")
endif ()

if(MSVC AND NOT OFF_ADD_NVTX )
add_library(CUDA::nvToolsExt INTERFACE IMPORTED)
set_property(TARGET CUDA::nvToolsExt APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CUDAToolkit_INCLUDE_DIRS}")
endif ()

add_definitions(-DCHARSET_UTF_8 -DMECAB_CHARSET=utf-8 -DMECAB_UTF8_USE_ONLY)
add_definitions(-DHAVE_CONFIG_H
    -DDIC_VERSION=102
    -DMECAB_DEFAULT_RC="dummy"
    -DMECAB_WITHOUT_SHARE_DIC
    -DPACKAGE="open_jtalk"
    -DVERSION="${OPEN_JTALK_VERSION}")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/g2p/hts_engine ${CPPMODULE_BINARY_SUBDIR}/hts_engine)
set(HTS_ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/g2p/hts_engine/include)
set(HTS_ENGINE_LIB ${CPPMODULE_BINARY_SUBDIR}/hts_engine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/g2p/open_jtalk)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/g2p/open_jtalk)

# 是否为android构建
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(TORCH_LIBRARIES
      #      ${Torch_Base_Dir}/lib/libnnpack.a
      #      -Wl,--gc-sections
      #      -Wl,--whole-archive
      #      ${Torch_Base_Dir}/lib/libtorch.a
      #      ${Torch_Base_Dir}/lib/libtorch_cpu.a
      #      -Wl,--no-whole-archive
      #      ${Torch_Base_Dir}/lib/libc10.a
      #      ${Torch_Base_Dir}/lib/libnnpack.a
      #      ${Torch_Base_Dir}/lib/libXNNPACK.a
      #      ${Torch_Base_Dir}/lib/libcpuinfo.a
      #      ${Torch_Base_Dir}/lib/libclog.a
      #      ${Torch_Base_Dir}/lib/libeigen_blas.a
      #      ${Torch_Base_Dir}/lib/libpytorch_qnnpack.a
      #      ${Torch_Base_Dir}/lib/libpthreadpool.a


      -Wl,--whole-archive
      -lvulkan
      -Wl,--no-whole-archive

      -Wl,--whole-archive
      -ltorch_global_deps
      -lc10
      -lmicrokernels-prod
      -lpytorch_qnnpack
      -lnnpack
      -lXNNPACK
      -ltorch
      -ltorch_cpu

      -Wl,--no-whole-archive
      -lpthreadpool
      -lcpuinfo
      -lclog
      -leigen_blas
  )
  include_directories(${Torch_Base_Dir}/include)
  link_directories(${Torch_Base_Dir}/lib)
else ()
  set(CMAKE_PREFIX_PATH "${Torch_Base_Dir}" ${CMAKE_PREFIX_PATH})
  find_package(Torch REQUIRED)
  include_directories(${TORCH_INCLUDE_DIRS})
  #link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1/lib64")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
  #include_directories(${Tarch_Base_Dir}/include)
endif ()

file(GLOB_RECURSE GPT_SOVITS_CPP_SOURCE
    "src/*.cpp"
    "src/bert/*.cpp"
    "src/g2p/*.cpp"
    "src/text/*.cpp"
    "src/export/*.cpp"
)
IF (OS_IS_LINUX)
  # 把这段注释掉编译就会出现奇奇怪怪的符号未定义问题
  include_directories(third_party/cpp-pinyin/src/toneUtil)
  include_directories(third_party/tokenizers-cpp/msgpack/include)
  include_directories(third_party/tokenizers-cpp/sentencepiece/src)
  file(GLOB_RECURSE GPT_SOVITS_CPP_ADDER_SOURCE
      "third_party/cpp-pinyin/src/*.cpp"
      "third_party/tokenizers-cpp/src/*.cc"
  )
  set(GPT_SOVITS_CPP_SOURCE
      ${GPT_SOVITS_CPP_ADDER_SOURCE}
      ${GPT_SOVITS_CPP_SOURCE}
      third_party/fmt/src/format.cc
  )
endif ()

if (GPTSOVITS_BUILD_SHARED)
  add_library(${GPTSOVITS_PROJECT_NAME} SHARED ${GPT_SOVITS_CPP_SOURCE})
  target_compile_definitions(${GPTSOVITS_PROJECT_NAME} PRIVATE GPTSOVITS_API_EXPORTS)
  set_target_properties(${GPTSOVITS_PROJECT_NAME} PROPERTIES
      LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/src/export/gpt_sovits_cpp.def\""
  )

else()
  add_library(${GPTSOVITS_PROJECT_NAME} STATIC ${GPT_SOVITS_CPP_SOURCE})
endif()

target_include_directories(${GPTSOVITS_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_compile_definitions(${GPTSOVITS_PROJECT_NAME} PRIVATE _ISLIBRARY_)
target_include_directories(${GPTSOVITS_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(GPTSOVITS_BUILD_SHARED)
  target_link_libraries(${GPTSOVITS_PROJECT_NAME} PRIVATE
      ${TORCH_LIBRARIES}
      ${CPPMODULE_LINK_LIBRARIES_ALL}
      openjtalk
      #        _CPPMODULE_FMT_LIB_
      #        ${CPPMODULE_BINARY_SUBDIR}/cpp-pinyin/libcpp-pinyin.a
  )
else ()
  target_link_libraries(${GPTSOVITS_PROJECT_NAME} PUBLIC
      ${TORCH_LIBRARIES}
      ${CPPMODULE_LINK_LIBRARIES_ALL}
      openjtalk
      #        _CPPMODULE_FMT_LIB_
      #        ${CPPMODULE_BINARY_SUBDIR}/cpp-pinyin/libcpp-pinyin.a
  )
endif ()

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${TORCH_DLLS}
      "${CMAKE_BINARY_DIR}")
  if(GPTSOVITS_BUILD_SHARED)
    set(GPTSOVITS_BINAYNAME "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
    add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${GPTSOVITS_BINAYNAME}/${GPTSOVITS_PROJECT_NAME}.dll"
        "${CMAKE_BINARY_DIR}/${GPTSOVITS_PROJECT_NAME}.dll"
        COMMENT "Copying ${GPTSOVITS_PROJECT_NAME} Binary to Target"
    )
    # 如果有.lib和.pdb的话
    if (EXISTS "${GPTSOVITS_BINAYNAME}/${GPTSOVITS_PROJECT_NAME}.lib")
      add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
          "${GPTSOVITS_BINAYNAME}/${GPTSOVITS_PROJECT_NAME}.lib"
          "${CMAKE_BINARY_DIR}/${GPTSOVITS_PROJECT_NAME}.lib"
          COMMENT "Copying ${GPTSOVITS_PROJECT_NAME} Lib to Target"
      )
    endif ()
    if (EXISTS "${GPTSOVITS_BINAYNAME}/${GPTSOVITS_PROJECT_NAME}.pdb")
      add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME}
          POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
          "${GPTSOVITS_BINAYNAME}/${GPTSOVITS_PROJECT_NAME}.pdb"
          "${CMAKE_BINARY_DIR}/${GPTSOVITS_PROJECT_NAME}.pdb"
          COMMENT "Copying ${GPTSOVITS_PROJECT_NAME} pdb to Target"
      )
    endif ()
  endif ()
endif (MSVC)

# copy res
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME} POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/res
      ${CMAKE_SOURCE_DIR}/../assets/res
  )
else ()
  add_custom_command(TARGET ${GPTSOVITS_PROJECT_NAME} POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/res
      ${CMAKE_BINARY_DIR}/res
  )
endif ()


