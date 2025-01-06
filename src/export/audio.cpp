//
// Created by 19254 on 25-1-2.
//
#include "./global.h"

void APICALL gsv_audio_free(struct gsv_audio *audio) {
  delete audio;
};

void APICALL gsv_audio_save_to_file(struct gsv_audio *audio, const char *path) {
  audio->audio->SaveToFile(path);
}

void APICALL gsv_audio_get_header(struct gsv_audio *audio, struct gsv_audio_header *header) {
  auto h = audio->audio->GetHeader();
  header->Channels = h.Channels;
  header->Format = h.Format;
  header->Frames = h.Frames;
  header->SampleRate = h.SampleRate;
};

void APICALL gsv_audio_samples(struct gsv_audio *audio, const float **samples, size_t *samples_size) {
  const std::vector<float> &sr = audio->audio->ReadSamples();
  *samples = sr.data();
  *samples_size = sr.size();
};

void APICALL gsv_audio_append_empty(struct gsv_audio *audio, uint32_t duration_ms) {
  audio->audio->AppendEmpty(duration_ms);
}

struct gsv_audio *APICALL gsv_audio_from_file(const char *path) {
  auto r = new gsv_audio{GPTSovits::AudioTools::FromFile(path)};
  return r;
};

struct gsv_audio *APICALL gsv_audio_from_byte(const float *samples, size_t samples_size, int samplerate, int channels, int format, int sections, int seekable) {
  auto r = new gsv_audio{GPTSovits::AudioTools::FromByte(std::vector<float>(samples, samples + samples_size), samplerate, channels, format, sections, seekable)};
  return r;
};

struct gsv_audio *APICALL gsv_audio_from_empty(int samplerate, int channels, int format) {
  auto r = new gsv_audio{GPTSovits::AudioTools::FromEmpty(samplerate, channels, format)};
  return r;
};

struct gsv_audio *APICALL gsv_audio_resample(struct gsv_audio *audio, int target_samplerate) {
  auto new_audio = audio->audio->ReSample(target_samplerate);
  return new gsv_audio{std::move(new_audio)};
};

void APICALL gsv_audio_append(struct gsv_audio *audio, struct gsv_audio *target_audio) {
  audio->audio->Append(*target_audio->audio);
};
