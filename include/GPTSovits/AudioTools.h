//
// Created by 19254 on 24-11-12.
//

#ifndef GPT_SOVITS_CPP_AUDIOTOOLS_H
#define GPT_SOVITS_CPP_AUDIOTOOLS_H

#include <string>
#include <memory>
#include <vector>
#include <sndfile.h>

namespace GPTSovits {

class AudioTools {
  SF_INFO m_sfinfo;
  SNDFILE *m_infile;

  void check_init();

  std::vector<float> m_samplesCache;
public:

  struct AudioHeader {
    int SampleRate;
    int Channels;
    sf_count_t Frames;
    int Format;
  };

  ~AudioTools();

  AudioHeader GetHeader();

  const std::vector<float> &ReadSamples();

  /**
   * @brief 从音频文件创建对象
   * */
  static std::unique_ptr<AudioTools>
  FromFile(const std::string &file);

  static std::unique_ptr<AudioTools>
  FromByte(SF_INFO sfInfo, const std::vector<float> &samples);

  static std::unique_ptr<AudioTools>
  FromByte(const std::vector<float> &samples, int samplerate, int channels = 1,
           int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16, int sections = 1, int seekable = 1);

  static std::unique_ptr<AudioTools>
  FromEmpty(int samplerate, int channels = 1,
            int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16);


  AudioTools &AppendEmpty(uint32_t duration_ms);

  size_t SaveToFile(const std::string &path, int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16);

  std::unique_ptr<AudioTools> ReSample(int targetSamplerate);

  AudioTools &Append(AudioTools &other);

};


}

#endif //GPT_SOVITS_CPP_AUDIOTOOLS_H
