cmake_minimum_required(VERSION 3.29)
project(gpt_sovits_cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
  else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  endif ()
  set(CAFFE2_USE_CUDNN ON)
  set(USE_CUSPARSELT OFF)
endif ()
message("Torch_DIR=${Torch_DIR}")
# 检查torch定义
if (DEFINED Torch_DIR)
  set(Torch_Base_Dir ${Torch_DIR})
else ()
  message(FATAL_ERROR "Please specify the torch directory with -DTorch_DIR=<path/to/torch>")
endif ()
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

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GPT_SOVITS_CPP_SOURCE "src/*.cpp")

add_library(gpt_sovits_cpp_static ${GPT_SOVITS_CPP_SOURCE})
#target_compile_definitions(gpt_sovits_cpp_static PRIVATE _ISLIBRARY_)
target_link_libraries(gpt_sovits_cpp_static PRIVATE ${TORCH_LIBRARIES} ${CPPMODULE_LINK_LIBRARIES_ALL})
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET gpt_sovits_cpp_static
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${TORCH_DLLS}
      "${CMAKE_BINARY_DIR}")
endif (MSVC)

# copy res
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  add_custom_command(TARGET gpt_sovits_cpp_static POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/res
      ${CMAKE_SOURCE_DIR}/../assets/res
  )
else ()
  add_custom_command(TARGET gpt_sovits_cpp_static POST_BUILD
      COMMAND
      ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_CURRENT_SOURCE_DIR}/res
      ${CMAKE_BINARY_DIR}/res
  )
endif ()

