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

#ifndef SNDFILE_H


/* The following file types can be read and written.
** A file type would consist of a major type (ie SF_FORMAT_WAV) bitwise
** ORed with a minor type (ie SF_FORMAT_PCM). SF_FORMAT_TYPEMASK and
** SF_FORMAT_SUBMASK can be used to separate the major and minor file
** types.
*/

enum {                             /* Major formats. */
       SF_FORMAT_WAV = 0x010000,   /* Microsoft WAV format (little endian default). */
       SF_FORMAT_AIFF = 0x020000,  /* Apple/SGI AIFF format (big endian). */
       SF_FORMAT_AU = 0x030000,    /* Sun/NeXT AU format (big endian). */
       SF_FORMAT_RAW = 0x040000,   /* RAW PCM data. */
       SF_FORMAT_PAF = 0x050000,   /* Ensoniq PARIS file format. */
       SF_FORMAT_SVX = 0x060000,   /* Amiga IFF / SVX8 / SV16 format. */
       SF_FORMAT_NIST = 0x070000,  /* Sphere NIST format. */
       SF_FORMAT_VOC = 0x080000,   /* VOC files. */
       SF_FORMAT_IRCAM = 0x0A0000, /* Berkeley/IRCAM/CARL */
       SF_FORMAT_W64 = 0x0B0000,   /* Sonic Foundry's 64 bit RIFF/WAV */
       SF_FORMAT_MAT4 = 0x0C0000,  /* Matlab (tm) V4.2 / GNU Octave 2.0 */
       SF_FORMAT_MAT5 = 0x0D0000,  /* Matlab (tm) V5.0 / GNU Octave 2.1 */
       SF_FORMAT_PVF = 0x0E0000,   /* Portable Voice Format */
       SF_FORMAT_XI = 0x0F0000,    /* Fasttracker 2 Extended Instrument */
       SF_FORMAT_HTK = 0x100000,   /* HMM Tool Kit format */
       SF_FORMAT_SDS = 0x110000,   /* Midi Sample Dump Standard */
       SF_FORMAT_AVR = 0x120000,   /* Audio Visual Research */
       SF_FORMAT_WAVEX = 0x130000, /* MS WAVE with WAVEFORMATEX */
       SF_FORMAT_SD2 = 0x160000,   /* Sound Designer 2 */
       SF_FORMAT_FLAC = 0x170000,  /* FLAC lossless file format */
       SF_FORMAT_CAF = 0x180000,   /* Core Audio File format */
       SF_FORMAT_WVE = 0x190000,   /* Psion WVE format */
       SF_FORMAT_OGG = 0x200000,   /* Xiph OGG container */
       SF_FORMAT_MPC2K = 0x210000, /* Akai MPC 2000 sampler */
       SF_FORMAT_RF64 = 0x220000,  /* RF64 WAV file */
       SF_FORMAT_MPEG = 0x230000,  /* MPEG-1/2 audio stream */

       /* Subtypes from here on. */

       SF_FORMAT_PCM_S8 = 0x0001, /* Signed 8 bit data */
       SF_FORMAT_PCM_16 = 0x0002, /* Signed 16 bit data */
       SF_FORMAT_PCM_24 = 0x0003, /* Signed 24 bit data */
       SF_FORMAT_PCM_32 = 0x0004, /* Signed 32 bit data */

       SF_FORMAT_PCM_U8 = 0x0005, /* Unsigned 8 bit data (WAV and RAW only) */

       SF_FORMAT_FLOAT = 0x0006,  /* 32 bit float data */
       SF_FORMAT_DOUBLE = 0x0007, /* 64 bit float data */

       SF_FORMAT_ULAW = 0x0010,      /* U-Law encoded. */
       SF_FORMAT_ALAW = 0x0011,      /* A-Law encoded. */
       SF_FORMAT_IMA_ADPCM = 0x0012, /* IMA ADPCM. */
       SF_FORMAT_MS_ADPCM = 0x0013,  /* Microsoft ADPCM. */

       SF_FORMAT_GSM610 = 0x0020,    /* GSM 6.10 encoding. */
       SF_FORMAT_VOX_ADPCM = 0x0021, /* OKI / Dialogix ADPCM */

       SF_FORMAT_NMS_ADPCM_16 = 0x0022, /* 16kbs NMS G721-variant encoding. */
       SF_FORMAT_NMS_ADPCM_24 = 0x0023, /* 24kbs NMS G721-variant encoding. */
       SF_FORMAT_NMS_ADPCM_32 = 0x0024, /* 32kbs NMS G721-variant encoding. */

       SF_FORMAT_G721_32 = 0x0030, /* 32kbs G721 ADPCM encoding. */
       SF_FORMAT_G723_24 = 0x0031, /* 24kbs G723 ADPCM encoding. */
       SF_FORMAT_G723_40 = 0x0032, /* 40kbs G723 ADPCM encoding. */

       SF_FORMAT_DWVW_12 = 0x0040, /* 12 bit Delta Width Variable Word encoding. */
       SF_FORMAT_DWVW_16 = 0x0041, /* 16 bit Delta Width Variable Word encoding. */
       SF_FORMAT_DWVW_24 = 0x0042, /* 24 bit Delta Width Variable Word encoding. */
       SF_FORMAT_DWVW_N = 0x0043,  /* N bit Delta Width Variable Word encoding. */

       SF_FORMAT_DPCM_8 = 0x0050,  /* 8 bit differential PCM (XI only) */
       SF_FORMAT_DPCM_16 = 0x0051, /* 16 bit differential PCM (XI only) */

       SF_FORMAT_VORBIS = 0x0060, /* Xiph Vorbis encoding. */
       SF_FORMAT_OPUS = 0x0064,   /* Xiph/Skype Opus encoding. */

       SF_FORMAT_ALAC_16 = 0x0070, /* Apple Lossless Audio Codec (16 bit). */
       SF_FORMAT_ALAC_20 = 0x0071, /* Apple Lossless Audio Codec (20 bit). */
       SF_FORMAT_ALAC_24 = 0x0072, /* Apple Lossless Audio Codec (24 bit). */
       SF_FORMAT_ALAC_32 = 0x0073, /* Apple Lossless Audio Codec (32 bit). */

       SF_FORMAT_MPEG_LAYER_I = 0x0080,   /* MPEG-1 Audio Layer I */
       SF_FORMAT_MPEG_LAYER_II = 0x0081,  /* MPEG-1 Audio Layer II */
       SF_FORMAT_MPEG_LAYER_III = 0x0082, /* MPEG-2 Audio Layer III */

       /* Endian-ness options. */

       SF_ENDIAN_FILE = 0x00000000,   /* Default file endian-ness. */
       SF_ENDIAN_LITTLE = 0x10000000, /* Force little endian-ness. */
       SF_ENDIAN_BIG = 0x20000000,    /* Force big endian-ness. */
       SF_ENDIAN_CPU = 0x30000000,    /* Force CPU endian-ness. */

       SF_FORMAT_SUBMASK = 0x0000FFFF,
       SF_FORMAT_TYPEMASK = 0x0FFF0000,
       SF_FORMAT_ENDMASK = 0x30000000
};


typedef int64_t sf_count_t;
struct SF_INFO {
  sf_count_t frames; /* Used to be called samples.  Changed to avoid confusion. */
  int samplerate;
  int channels;
  int format;
  int sections;
  int seekable;
};
typedef struct SF_INFO SF_INFO;

#endif

struct device_info;
struct gsv_cli;
struct gsv_audio;
struct gsv_sentence;

/// @brief 音频格式信息
struct gsv_audio_header {
  /// 音频采样率
  int SampleRate;
  /// 音频通道数
  int Channels;
  /// 音频采样位数
  sf_count_t Frames;
  /// 音频格式
  int Format;
};

/// @brief 切分方式枚举
typedef enum {
  /// @brief 按标点符号切分
  Punctuation,
  /// @brief 按标点符号切分(严格模式,逗号等符号也会切分)
  StrictPunctuation,
  /// @brief 按最大长度切分
  Length,
} SentenceSplitMethod;

typedef bool (*GSVSentenceAppendCallbackFunction)(const char *sentence, void *user_data);

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
/// Sentence工具相关函数
/// ****************************
//

/**
  * @brief 创建Sentence分句工具实例
  *
  * @param cut_method 切分方式，推荐为按标点符号切分( Punctuation )
  * @param max_length 当切分方式为Length时有效，表示最大长度，推荐为50，不推荐过长
  *
  * @return Sentence分句工具实例
 */
LIBAPI struct gsv_sentence *APICALL gsv_sentence_create(SentenceSplitMethod cut_method, int max_length);
/**
  * @brief 释放Sentence分句工具实例
  *
  * @param sentence_cli Sentence分句工具实例
  *
  * @return Sentence分句工具实例
 */
LIBAPI void APICALL gsv_sentence_free(struct gsv_sentence *sentence_cli);

/**
  * @brief 设置Sentence分句工具实例的回调函数
  *
  * @param sentence_cli Sentence分句工具实例
  * @param callback 回调函数
  * @param user_data 附加用户数据
  *
  * @return Sentence分句工具实例
 */
LIBAPI void APICALL gsv_sentence_set_append_callback(struct gsv_sentence *sentence_cli, GSVSentenceAppendCallbackFunction callback, void *user_data);

/**
  * @brief 填入句子
  *
  * @param sentence_cli Sentence分句工具实例
  * @param sentence 待分句文本
  *
  * @return Sentence分句工具实例
 */
LIBAPI void APICALL gsv_sentence_append(struct gsv_sentence *sentence_cli, const char *sentence);
/**
  * @brief 处理剩余文本,将剩余文本交于callback
  *
  * @param sentence_cli Sentence分句工具实例
  *
  * @return Sentence分句工具实例
 */
LIBAPI void APICALL gsv_sentence_flush(struct gsv_sentence *sentence_cli);

/// ****************************
/// Audio工具相关函数
/// ****************************
//

/**
 * @brief 从文件创建Audio实例
 * @param path 文件路径
 * @return Audio实例
 */
LIBAPI struct gsv_audio *APICALL gsv_audio_from_file(const char *path);

/**
 * @brief 从字节创建Audio实例
 * @param samples 音频数据
 * @param samples_size 音频数据大小
 * @param samplerate 采样率, 例如: 32000
 * @param channels 通道数
 * @param format 格式, 例如: SF_FORMAT_WAV | SF_FORMAT_PCM_16
 * @param sections 段数
 * @param seekable 是否支持seek
 * @return Audio实例
 */
LIBAPI struct gsv_audio *APICALL gsv_audio_from_byte(const float *samples, size_t samples_size, int samplerate, int channels, int format, int sections, int seekable);

/**
 * @brief 从空创建Audio实例
 * @param samplerate 采样率, 例如: 32000
 * @param channels 通道数
 * @param format 格式, 例如: SF_FORMAT_WAV | SF_FORMAT_PCM_16
 * @return Audio实例
 */
LIBAPI struct gsv_audio *APICALL gsv_audio_from_empty(int samplerate, int channels, int format);

/**
 * @brief 释放Audio实例
 * @param audio Audio实例
 */
LIBAPI void APICALL gsv_audio_free(struct gsv_audio *audio);

/**
 * @brief 获取Audio实例的音频头
 * @param audio Audio实例
 * @param header 音频头
 */
LIBAPI void APICALL gsv_audio_get_header(struct gsv_audio *audio, struct gsv_audio_header *header);

/**
 * @brief 获取Audio实例的音频数据
 * @param audio Audio实例
 * @param samples 音频数据,禁止修改/释放
 * @param samples_size 音频数据大小
 * @code
 * const float* samples;
 * size_t samples_size;
 * gsv_audio_samples(audio, &samples, &samples_size);
 * @endcode
 */
LIBAPI void APICALL gsv_audio_samples(struct gsv_audio *audio, const float **samples, size_t *samples_size);

/**
 * @brief 追加空音频
 * @param audio Audio实例
 * @param duration_ms 追加的空持续时间
 */
LIBAPI void APICALL gsv_audio_append_empty(struct gsv_audio* audio, uint32_t duration_ms);

/**
 * @brief 音频重采样
 * @param audio Audio实例
 * @param target_samplerate 目标采样率
 */
LIBAPI struct gsv_audio* APICALL gsv_audio_resample(struct gsv_audio* audio, int target_samplerate);

/**
 * @brief 音频追加
 * @param audio Audio实例
 * @param target_audio 追加的音频
 */
LIBAPI void APICALL gsv_audio_append(struct gsv_audio* audio, struct gsv_audio* target_audio);

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
