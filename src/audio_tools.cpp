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

}