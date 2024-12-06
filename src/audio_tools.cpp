//
// Created by 19254 on 24-11-12.
//
#include "GPTSovits/Utils/exception.h"
#include "GPTSovits/AudioTools.h"
#include "sndfile.h"
#include <stdexcept>
#include <format>
#include <samplerate.h>

namespace GPTSovits {

std::unique_ptr<AudioTools>
AudioTools::FromFile(const std::string &file) {
  auto ptr = std::make_unique<AudioTools>();
  ptr->m_infile = sf_open(file.c_str(), SFM_READ, &ptr->m_sfinfo);
  if (!ptr->m_infile) {
    THROW_ERRORN("解析音频文件失败!\nFrom:{}", file);
  }

  return ptr;
}

std::unique_ptr<AudioTools>
AudioTools::FromByte(SF_INFO sfInfo, const std::vector<float> &samples) {
  auto ptr = std::make_unique<AudioTools>();
  ptr->m_sfinfo = sfInfo;
  ptr->m_infile = nullptr;
  ptr->m_samplesCache = samples;
  return ptr;
}

std::unique_ptr<AudioTools>
AudioTools::FromByte(const std::vector<float> &samples, int samplerate, int channels, int format, int sections,
                     int seekable) {
  auto ptr = std::make_unique<AudioTools>();
  auto sinfo = SF_INFO{0};
  sinfo.frames = samples.size();
  sinfo.samplerate = samplerate;
  sinfo.channels = channels;
  sinfo.format = format;
  sinfo.sections = sections;
  sinfo.seekable = seekable;
  ptr->m_sfinfo = sinfo;
  ptr->m_infile = nullptr;
  ptr->m_samplesCache = samples;
  return ptr;
}

AudioTools::~AudioTools() {
  if (m_infile) {
    sf_close(m_infile);
  }
}


void AudioTools::check_init() {
  if (!m_infile && m_samplesCache.empty()) {
    THROW_ERRORN("解析音频对象未初始化!");
  }
};

AudioTools::AudioHeader AudioTools::GetHeader() {
  check_init();
  return {
    m_sfinfo.samplerate,
    m_sfinfo.channels,
    m_sfinfo.frames,
    m_sfinfo.format,
  };
};

const std::vector<float> &AudioTools::ReadSamples() {
  check_init();
  if (!m_samplesCache.empty()) {
    return m_samplesCache;
  }
  m_samplesCache.clear();
  m_samplesCache.resize(m_sfinfo.frames * m_sfinfo.channels);
  auto numFrames = sf_readf_float(m_infile, m_samplesCache.data(), m_sfinfo.frames);
  if (numFrames != m_sfinfo.frames) {
    THROW_ERRORN("Error reading samples");
  }
  return m_samplesCache;
}

size_t AudioTools::SaveToFile(const std::string &path, int format) {
  check_init();
  auto srCache = ReadSamples();
  SF_INFO outputInfo = m_sfinfo;
  SNDFILE *outputFile = sf_open(path.c_str(), SFM_WRITE, &outputInfo);
  outputInfo = m_sfinfo;
  if (!outputFile) {
    THROW_ERRORN("Error opening output file: {}", src_strerror(NULL));
  }
  outputInfo.format = format;
  auto numFrames = sf_writef_float(outputFile, srCache.data(), outputInfo.frames);
  if (numFrames != outputInfo.frames) {
    sf_close(outputFile);
    THROW_ERRORN("Error reading samples");
  }
  sf_close(outputFile);
  return numFrames;
}

std::unique_ptr<AudioTools>
AudioTools::ReSample(int targetSamplerate) {
  check_init();
  auto srCache = ReadSamples();
  // 计算重采样所需的输出样本数
  double srcRatio = static_cast<double>(targetSamplerate) / m_sfinfo.samplerate;
  // 准备输出数据缓冲区
  auto outputFrames = static_cast<size_t>(m_sfinfo.frames * srcRatio);
  std::vector<float> outputData(outputFrames * m_sfinfo.channels);
  // 使用 libsamplerate 进行重采样
  SRC_DATA srcData;
  srcData.data_in = srCache.data();
  srcData.input_frames = m_sfinfo.frames;
  srcData.data_out = outputData.data();
  srcData.output_frames = outputFrames;
  srcData.src_ratio = srcRatio;
  srcData.end_of_input = SF_TRUE;

  if (auto error = src_simple(&srcData, SRC_SINC_MEDIUM_QUALITY, m_sfinfo.channels);error) {
    THROW_ERRORN("Error during resampling: {}", src_strerror(error));
  }
  auto newInfo = m_sfinfo;
  newInfo.samplerate = targetSamplerate;
  newInfo.frames = outputFrames;
  auto resPtr = FromByte(newInfo, outputData);

  return resPtr;
}

AudioTools& AudioTools::Append(AudioTools &other) {
  check_init();

  // 读取两个音频的采样数据
  auto thisData = ReadSamples();
  auto otherData = other.ReadSamples();

  // 如果采样率不同，需要对第二个音频进行重采样
  std::vector<float> resampledData;
  sf_count_t otherFrames = other.m_sfinfo.frames;

  if (other.m_sfinfo.samplerate != m_sfinfo.samplerate) {
    // 重采样处理
    double srcRatio = static_cast<double>(m_sfinfo.samplerate) / other.m_sfinfo.samplerate;
    size_t outputFrames = static_cast<size_t>(other.m_sfinfo.frames * srcRatio);
    resampledData.resize(outputFrames * other.m_sfinfo.channels);

    SRC_DATA srcData;
    srcData.data_in = otherData.data();
    srcData.input_frames = other.m_sfinfo.frames;
    srcData.data_out = resampledData.data();
    srcData.output_frames = outputFrames;
    srcData.src_ratio = srcRatio;
    srcData.end_of_input = SF_TRUE;

    if (auto error = src_simple(&srcData, SRC_SINC_MEDIUM_QUALITY, other.m_sfinfo.channels)) {
      THROW_ERRORN("Error during resampling: {}", src_strerror(error));
    }

    otherData = resampledData;
    otherFrames = outputFrames;
  }

  // 处理声道数不同的情况
  std::vector<float> convertedData;
  if (other.m_sfinfo.channels != m_sfinfo.channels) {
    convertedData.resize(otherFrames * m_sfinfo.channels);

    if (other.m_sfinfo.channels == 1 && m_sfinfo.channels == 2) {
      // 单声道转立体声
      for (sf_count_t i = 0; i < otherFrames; i++) {
        convertedData[i * 2] = otherData[i];
        convertedData[i * 2 + 1] = otherData[i];
      }
    } else if (other.m_sfinfo.channels == 2 && m_sfinfo.channels == 1) {
      // 立体声转单声道（取平均值）
      for (sf_count_t i = 0; i < otherFrames; i++) {
        convertedData[i] = (otherData[i * 2] + otherData[i * 2 + 1]) * 0.5f;
      }
    } else {
      THROW_ERRORN("Unsupported channel conversion");
    }
    otherData = convertedData;
  }

  // 合并音频数据
  m_samplesCache.reserve(thisData.size() + otherData.size());
  m_samplesCache.insert(m_samplesCache.end(), otherData.begin(), otherData.end());

  // 更新帧数信息
  m_sfinfo.frames += otherFrames;

  return *this;
//  check_init();
//  other.check_init();
//
//  // 获取当前音频和待合并音频的头信息
//  auto currentHeader = GetHeader();
//  auto otherHeader = other.GetHeader();
//
//  // 重采样
//  std::unique_ptr<AudioTools> resampledOther;
//  if (currentHeader.SampleRate != otherHeader.SampleRate) {
//    resampledOther = other.ReSample(currentHeader.SampleRate);
//  } else {
//    resampledOther = std::make_unique<AudioTools>(other);
//  }
//
//  // 声道匹配
//  // 如果声道数不同，需要进行转换，这里假设目标是立体声
//  if (currentHeader.Channels != resampledOther->GetHeader().Channels) {
//    THROW_ERRORN("声道数不匹配且转换未实现");
//  }
//
//  // 格式匹配
//  // 如果格式不同，可以在这里进行格式转换，暂时假设格式一致
//
//  // 拼接音频数据
//  auto currentSamples = ReadSamples();
//  auto otherSamples = resampledOther->ReadSamples();
//
//  std::vector<float> concatenatedSamples;
//  concatenatedSamples.reserve(currentSamples.size() + otherSamples.size());
//  concatenatedSamples.insert(concatenatedSamples.end(), currentSamples.begin(), currentSamples.end());
//  concatenatedSamples.insert(concatenatedSamples.end(), otherSamples.begin(), otherSamples.end());
//
//  // 创建新的音频对象
//  SF_INFO newInfo = m_sfinfo;
//  newInfo.frames = currentHeader.Frames + resampledOther->GetHeader().Frames;
//  return FromByte(newInfo, concatenatedSamples);
}

std::unique_ptr<AudioTools> AudioTools::FromEmpty(int samplerate, int channels, int format) {
  auto ptr = std::make_unique<AudioTools>();
  SF_INFO sfinfo = {0};
  sfinfo.samplerate = samplerate;
  sfinfo.channels = channels;
  sfinfo.format = format;
  ptr->m_sfinfo = sfinfo;
  ptr->m_infile = nullptr;
  return ptr;
}

AudioTools& AudioTools::AppendEmpty(uint32_t duration_ms) {
  check_init();

  // 计算需要的空样本数
  sf_count_t emptyFrames = static_cast<sf_count_t>(m_sfinfo.samplerate * duration_ms / 1000.0);
  std::vector<float> emptyData(emptyFrames * m_sfinfo.channels, 0.0f);

  // 在现有样本数据后附加空样本
  m_samplesCache.insert(m_samplesCache.end(), emptyData.begin(), emptyData.end());

  // 更新帧数信息
  m_sfinfo.frames += emptyFrames;

  return *this;
}

}