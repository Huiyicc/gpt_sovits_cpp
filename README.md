# 环境配置

- 安装CUDA
    - > 这个可以参考我的文章  
      [cmake配置cuda12.4+libtorch开发环境](https://www.hyiy.top/archives/53/)
- 克隆[CppModels](https://github.com/Huiyicc/CppModels)并按照仓库README配置
- 下载[Libtorch](https://pytorch.org/)
    - 根据你的环境选择CUDA版本,Debug/Release


# 资源消耗对比

> 文档暂时施工中

# 编译/运行例子

## 编译例子

- 准备好一个参考音频和对应文本
- 修改 `test/cli/gsv_cli.cpp` 的第27行为参考音频文本

```shell
# 仓库已合并到 https://github.com/Huiyicc/CppModels 全家桶
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

# 感谢以下项目做的前置代码工作

[GPT-SoVITS](https://github.com/RVC-Boss/GPT-SoVITS) **GPT Sovits原项目**  
[gpt_sovits_rs](https://github.com/second-state/gpt_sovits_rs) **GPT Sovits的TorchScript模型导出与Rust推理实现参考**  

