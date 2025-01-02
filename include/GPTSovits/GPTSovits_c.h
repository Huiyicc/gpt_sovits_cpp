//
// Created by 19254 on 24-12-28.
//

#ifndef GPTSOVITS_C_H
#define GPTSOVITS_C_H

#if defined(_MSC_VER_) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#ifdef GPTSOVITS_API_EXPORTS
#pragma message("Building DLL")
#define LIBAPI __declspec(dllexport)
#else
#define LIBAPI __declspec(dllimport)
#endif
#define APICALL __stdcall

#else
#define LIBAPI __attribute__((visibility("default")))
#define APICALL
#endif

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// ****************************
/// 类型定义
/// ****************************
//

struct device_info;
struct gsv_cli;
struct gsv_audio;

/// ****************************
/// 全局函数
/// ****************************
//

/**
 * @brief 设置错误信息
 * @param error 错误信息
 */
LIBAPI void APICALL gsv_set_last_error(const char *error);
/**
 * @brief 获取错误信息
 * @return 错误信息
 */
LIBAPI const char *APICALL gsv_get_last_error();
/**
 * @brief 设置全局资源路径
 * @param path 路径, 如: /home/user/resources
 */
LIBAPI void APICALL gsv_global_resources_path_set(const char *path);

/// ****************************
/// 设备函数
/// ****************************
//

/**
 * @brief 获取设备实例
 * @param device 设备名称
 * @return 设备信息
 */
LIBAPI struct device_info *APICALL gsv_device_create(const char *device);
/**
 * @brief 获取设备名称
 * @param device 设备信息
 * @return 设备名称
 */
LIBAPI const char *APICALL gsv_device_get_name(const struct device_info *device);
/**
 * @brief 释放设备实例
 * @param device 设备信息
 */
LIBAPI void APICALL gsv_device_free(struct device_info *device);

/// ****************************
/// 主体模型函数
/// ****************************
//

/**
 * @brief 创建GPT-Sovits模型实例
 * @param device 设备实例
 * @param default_lang 默认语言
 * @param ssl_path_path ssl模型路径
 */
LIBAPI struct gsv_cli *APICALL gsv_cli_create(struct device_info *device, const char *default_lang, const char *ssl_path_path);
/**
 * @brief 释放GPT-Sovits模型实例
 * @param gsv GPT-Sovits模型实例
 */
LIBAPI void APICALL gsv_cli_free(struct gsv_cli *gsv);

/**
 * @brief 设置随机种子
 *
 * 种子影响最终效果,相同种子生成的音频效果是相同的
 *
 * @param gsv GPT-Sovits模型实例
 * @param seed 随机种子
 */
LIBAPI void APICALL gsv_cli_manual_seed(struct gsv_cli *gsv, int64_t seed);

/**
 * @brief 推理
 * @param gsv GPT-Sovits模型实例
 * @param speaker_name 说话人名称
 * @param target_text 推理文本
 * @return 音频实例
 */
LIBAPI struct gsv_audio *APICALL gsv_cli_infer(struct gsv_cli *gsv, const char *speaker_name, const char *target_text);

/**
  * @brief 创建说话人
  *
  * @param gsv GPT-Sovits模型实例
  * @param speaker_name 说话人名称
  * @param model_path 说话人模型路径(gsv权重路径)
  * @param audio_path 说话人参考音频路径
  * @param ref_text 参考文本
  * @param lang 默认语言
  * @param warm_up 是否预热
  * @return 是否成功,失败可参考 gsv_get_last_error()
  * */
LIBAPI bool APICALL gsv_cli_create_speaker(struct gsv_cli *gsv,
                                           const char *speaker_name, const char *model_path,
                                           const char *audio_path, const char *ref_text,
                                           const char *lang, bool warm_up);

/// ****************************
/// Bert相关函数
/// ****************************
//


/**
 * @brief 注册Bert模型的中文语言支持
 *
 * @param gsv GPT-Sovits模型实例
 * @param model_path zh bert模型路径
 * @param tokenizer_path zh tokenizer路径,可前往: https://huggingface.co/lj1995/GPT-SoVITS/tree/main/chinese-roberta-wwm-ext-large 下载 tokenizer.json
 * @param warm_up 是否预热
 */
LIBAPI void APICALL gsv_bert_register_chinese(struct gsv_cli *gsv, const char *model_path, const char *tokenizer_path, bool warm_up);

/**
 * @brief 注册Bert模型的英文语言支持
 *
 *  默认bert模型路径为:
 *
 * @param gsv GPT-Sovits模型实例
 * @param warm_up 是否预热
 */
LIBAPI void APICALL gsv_bert_register_english(struct gsv_cli *gsv, bool warm_up);

/**
 * @brief 注册Bert模型的日语语言支持
 *
 *  默认bert模型路径为:
 *
 * @param gsv GPT-Sovits模型实例
 * @param warm_up 是否预热
 */
LIBAPI void APICALL gsv_bert_register_japanese(struct gsv_cli *gsv, bool warm_up);


/// ****************************
/// Audio工具相关函数
/// ****************************
//

/**
 * @brief 释放Audio实例
 * @param audio Audio实例
 */
LIBAPI void APICALL gsv_audio_free(struct gsv_audio* audio);
/**
 * @brief 将音频写入文件
 * @param audio Audio实例
 * @param path 文件路径
 */
LIBAPI void APICALL gsv_audio_save_to_file(struct gsv_audio* audio, const char* path);


#ifdef __cplusplus
}
#endif

#endif//GPTSOVITS_C_H
