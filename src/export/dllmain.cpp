//
// Created by 19254 on 24-12-28.
//
#include "GPTSovits/GPTSovits_c.h"
#if defined(_HOST_WINDOWS_) || defined(_WIN32)

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved) {

  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      // 初始化与进程相关的数据或资源
      // 例如：分配内存、打开文件等
      break;
    case DLL_THREAD_ATTACH:
      // 初始化与线程相关的数据或资源
      // 例如：分配线程本地存储（TLS）
      break;
    case DLL_THREAD_DETACH:
      // 清理与线程相关的数据或资源
      // 例如：释放线程本地存储（TLS）
      break;
    case DLL_PROCESS_DETACH:
      // 清理与进程相关的数据或资源
        // 例如：释放内存、关闭文件等
          break;
  }
  return TRUE;

};


#endif