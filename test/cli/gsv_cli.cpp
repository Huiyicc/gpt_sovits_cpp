//
// Created by 19254 on 24-11-10.
//
#include <GPTSovits/GPTSovits.h>
#include <filesystem>
#include <iostream>

auto text_data = R"(先帝创业未半而中道崩殂，今天下三分，益州疲弊，此诚危急存亡之秋也。然侍卫之臣不懈于内，忠志之士忘身于外者，盖追先帝之殊遇，欲报之于陛下也。诚宜开张圣听，以光先帝遗德，恢弘志士之气，不宜妄自菲薄，引喻失义，以塞忠谏之路也。
　　宫中府中，俱为一体；陟罚臧否，不宜异同。若有作奸犯科及为忠善者，宜付有司论其刑赏，以昭陛下平明之理，不宜偏私，使内外异法也。
　　侍中、侍郎郭攸之、费祎、董允等，此皆良实，志虑忠纯，是以先帝简拔以遗陛下。愚以为宫中之事，事无大小，悉以咨之，然后施行，必能裨补阙漏，有所广益。
　　将军向宠，性行淑均，晓畅军事，试用于昔日，先帝称之曰能，是以众议举宠为督。愚以为营中之事，悉以咨之，必能使行阵和睦，优劣得所。
　　亲贤臣，远小人，此先汉所以兴隆也；亲小人，远贤臣，此后汉所以倾颓也。先帝在时，每与臣论此事，未尝不叹息痛恨于桓、灵也。侍中、尚书、长史、参军，此悉贞良死节之臣，愿陛下亲之信之，则汉室之隆，可计日而待也。
　　臣本布衣，躬耕于南阳，苟全性命于乱世，不求闻达于诸侯。先帝不以臣卑鄙，猥自枉屈，三顾臣于草庐之中，咨臣以当世之事，由是感激，遂许先帝以驱驰。后值倾覆，受任于败军之际，奉命于危难之间，尔来二十有一年矣。
　　先帝知臣谨慎，故临崩寄臣以大事也。受命以来，夙夜忧叹，恐托付不效，以伤先帝之明；故五月渡泸，深入不毛。今南方已定，兵甲已足，当奖率三军，北定中原，庶竭驽钝，攘除奸凶，兴复汉室，还于旧都。此臣所以报先帝而忠陛下之职分也。至于斟酌损益，进尽忠言，则攸之、祎、允之任也。
　　愿陛下托臣以讨贼兴复之效，不效，则治臣之罪，以告先帝之灵。若无兴德之言，则责攸之、祎、允等之慢，以彰其咎；陛下亦宜自谋，以咨诹善道，察纳雅言，深追先帝遗诏。臣不胜受恩感激。今当远离，临表涕零，不知所言。)";

int main() {
  try {
    auto currentPath = std::filesystem::current_path().string();
    PrintDebug("Current Path:{}", currentPath);
    auto device = GPTSovits::GetDevice("cuda:1");
    PrintDebug("device: {}", GPTSovits::DeviceToString(*device));

    auto gpt_config = GPTSovits::GPTSovitsConfig::Make("zh",
                                                       currentPath + R"(/../model_base/ssl_model.pt)");
    auto gpt_sovits = gpt_config->Build(device);

    GPTSovits::Bert::RegisterChinese(*gpt_sovits,
                                     currentPath + R"(/../model_base/bert_model.pt)",
                                     currentPath + R"(/../model_base/tokenizer.json)", true);
    GPTSovits::Bert::RegisterEnglish(*gpt_sovits, true);
    GPTSovits::Bert::RegisterJapanese(*gpt_sovits, true);

    gpt_sovits->CreateSpeaker("firefly",
                              // currentPath + "/../model/gpt_sovits_model.pt",
                              R"(G:\models\tts\gpt_sovits\firefly\tmp\gpt_sovits_model.pt)",
                              currentPath + "/../model/ref.wav",
                              "虽然我也没太搞清楚状况,但他说，似乎只有直率、纯真、有童心的小孩子才能看见它.", "zh",true);

    std::vector<std::string> texts = {
      // text_data,
      "今天是2021年11月23日,天气:晴,气温32°C.",
      //     "天下之大内有乾坤。",
      //     "这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的 resource能完全take得了。Anyway我们还是先pilot一下，再follow up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……",
      //     "The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code points) of a UTF-8 encoded string.",
      //     "皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。\n"
      //     "虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！\n"
      //     "我study日本語的时候，もし有汉字，我会很happy。\n"
      //     "Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。\n"
      //     "But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。\n"
      //     "2021年6月25日,今天32°C。以上です，byebye！\n"
    };
    gpt_sovits->ManualSeed(1733476914);
    GPTSovits::Text::Sentence sentence;
    std::shared_ptr<GPTSovits::AudioTools> outaudio;
    auto start = std::chrono::high_resolution_clock::now();
    sentence.AppendCallBack([&](const std::string &text) -> bool {
      auto resAudio = gpt_sovits->Infer("firefly", text);
//      PrintDebug("Audio Size:{}", resAudio->ReadSamples().size());
      if (!outaudio) {
        outaudio = std::make_shared<GPTSovits::AudioTools>(*resAudio);
      } else {
        outaudio->AppendEmpty(300);
        outaudio->Append(*resAudio);
      }
      return true;
    });
    for (auto &text: texts) {
      sentence.Append(text);
    }
    sentence.Flush();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    auto outPath = std::filesystem::current_path() / fmt::format("cpp_out.wav");
    outaudio->SaveToFile(outPath.string());

    std::cout << "音频写入: " << outPath << std::endl;
    std::cout << "推理耗时: " << duration.count() << " ms" << std::endl;

  } catch (const GPTSovits::Exception &e) {
    PrintError("[{}:{}] {}", e.getFile(), e.getLine(), e.what());
  } catch (const std::exception &e) {
    PrintError("{}", e.what());
  }
}