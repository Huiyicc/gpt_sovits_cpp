> 此版本效率过低且已弃置,后续在 [GPT-SoVITS-cpp](https://github.com/GPT-SoVITS-Devel/GPT-SoVITS-cpp) 发布onnx/trt的c++绑定,onnx速度在保持python原生质量的同时生成速度持平甚至略超python原生,trt更是在onnx基础上吞吐几乎翻倍~🎉


# 环境配置

- 安装CUDA
    - > 这个可以参考我的文章  
      [cmake配置cuda12.4+libtorch开发环境](https://www.hyiy.top/archives/53/)
- 下载[Libtorch](https://pytorch.org/)
    - 根据你的环境选择CUDA版本,Debug/Release
      - > 推荐版本: 2.5.0+
- 安装rust
- 安装依赖库
     ```shell
        # ubuntu
        sudo apt install autoconf autogen automake build-essential libasound2-dev \
          libflac-dev libogg-dev libtool libvorbis-dev libopus-dev libmp3lame-dev \
          libmpg123-dev pkg-config 
    ```


# 推理成品样本与资源消耗对比

> 以下列表如无特殊标注均为同一硬件/系统  
> 系统: Windows11  
> CPU: Intel 12700  
> GPU: 2080ti 22G

| 项目             | 内存消耗(最高) | 显存消耗             | (无cache)输出时长/消耗时长 |
|----------------|----------|------------------|-------------------|
| gpt_sovits_cpp | 1444MB   | 2.7GB(1xSpeaker) | 46.8s/14.1s       |
| GPTSovits      | 3321MB   | 1.8GB(1xSpeaker) | 40.1s/17.6s       |

> 推理测试文本:  
> 今天是2021年11月23日,天气晴,气温32°C."天下之大内有乾坤。  
> 这个project的schedule有些问题，尤其是buffer不多。另外，cost也偏高。目前我们没法confirm手上的
> resource能完全take得了。Anyway我们还是先pilot一下，再follow
> up最终的output，看能不能run的比较smoothly，更重要的是evaluate所有的cost能不能完全被cover掉……  
> The C++ support in Jupyter is powered by the xeus-cling C++ kernel.This function is used to find the length (in code
> points) of a UTF-8 encoded string.

## 音频样本

**gpt_sovits_cpp:**
  
https://github.com/user-attachments/assets/7fad6765-c30c-4b86-8d49-8187c8c0d66b

**GPTSovits:**

https://github.com/user-attachments/assets/ec50ba61-3a47-479e-8ad9-d49e55147252

# 三语混合推理

> 皆さん、我在インターネット上看到someone把几国language混在一起speak。我看到之后be like：それは我じゃないか！私もtry一tryです。  
> 虽然是混乱している句子ですけど、中文日本語プラスEnglish、挑戦スタート！  
> 我study日本語的时候，もし有汉字，我会很happy。  
> Bueause中国人として、when I see汉字，すぐに那个汉字がわかります。  
> But 我hate外来語、什么マクドナルド、スターバックス、グーグル、ディズニーランド、根本记不住カタカナhow to写、太難しい。  
> 2021年6月25日,今天32°C。以上です，byebye！  

https://github.com/user-attachments/assets/0c1e4079-d048-451f-a17b-c0bfea552d0c

> 已知问题: 混合有日语时会使英语带有日本口音(🌿)

# 开发计划

- [x] 中文推理
- [x] 英文推理
- [x] 日语推理
- [ ] ~~韩语推理~~
  - > 由于韩语g2p项目许可证为LGPL与未知许,暂时搁置.
- [x] 多语种混合推理
- [ ] 优化模型
- [ ] ~~端测推理 (编译已通过,正在解决转换后torchscript部分类型不支持的问题)~~
  - > 已确认部分算子libtorch不支持

# 编译/运行例子

## 编译例子

- 准备好一个参考音频和对应文本
- 修改 `test/cli/gsv_cli.cpp` 的第27行为参考音频文本

```shell
git clone --recursive https://github.com/Huiyicc/gpt_sovits_cpp
cd gpt_sovits_cpp/test
mkdir build
cd build
cmake .. -DTorch_DIR=<libtorch 路径(不要将release与debug搞混了,有概率会出现无法推理的问题)>  -DCMAKE_BUILD_TYPE=Release
make
# windows会生成sln
# 将 gpt_sovits_test_cli 设置为启动项目在编译即可
```

> 有时候第一次cmake会失败,没有关系,在执行一次 `cmake .. -DTORCH_DIR=xxx`就行了

## 模型导出

- 准备好 [原项目](https://github.com/RVC-Boss/GPT-SoVITS/) 训练的模型
- 执行导出命令

```shell
#!/bin/bash
python export_torch_script.py --gpt_model "<gpt权重>.ckpt" \
--sovits_model "<sovits权重>.pth" \
--ref_audio "<参考音频>" \
--ref_text "<参考音频的文本>" \
--output_path "<模型导出路径>" \
--export_common_model 
```

## 运行例子

- 在build的上级目录创建 `model_base` 和 `model` 文件夹
- 将导出的 `bert_model.pt` `ssl_model.pt` `tokenizer.json` 复制到`model_base`
- 将导出的 `gpt_sovits_model.pt` 复制到`model`
- 将参考音频重命名为`ref.wav`放入`model`
- 运行 `gpt_sovits_test_cli`

# 代码使用

> 文档暂时施工中

# 其他

- windows下只支持MSVC(libtorch)
- 继承到UE需要使用(UBT?)自编译libtorch(或使用社区libtorch插件) [TODO This](https://github.com/Huiyicc/gpt_sovits_cpp/issues/6)

# 感谢以下项目做的前置代码工作

[GPT-SoVITS](https://github.com/RVC-Boss/GPT-SoVITS) **GPT Sovits原项目**  
[gpt_sovits_rs](https://github.com/second-state/gpt_sovits_rs) **GPT Sovits的TorchScript模型导出与Rust推理实现参考**  

