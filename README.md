# 注意
本项目还在开发阶段

# 编译

- 安装CUDA
  - > 这个可以参考我的文章  
        [cmake配置cuda12.4+libtorch开发环境](https://www.hyiy.top/archives/53/)
- 克隆[CppModels](https://github.com/Huiyicc/CppModels)并按照仓库README配置
- 下载[Libtorch](https://pytorch.org/)
  - 根据你的环境选择CUDA版本,Debug/Release
- 修改[CMakeLists.txt](CMakeLists.txt)

```shell
git clone https://github.com/Huiyicc/gpt_sovits_cpp
cd gpt_sovits_cpp
mkdir build
cd build
cmake .. -DTORCH_DIR=<libtorch 路径(不要将release与debug搞混了,会出现无法推理的问题)>
make
# windows会生成sln
# 将 gpt_sovits_test_cli 设置为启动项目在编译即可

```
> 有时候第一次cmake会失败,没有关系,在执行一次 `cmake .. -DTORCH_DIR=xxx`就行了

# 其他

# 感谢以下项目做的前置代码工作

[GPT-SoVITS](https://github.com/RVC-Boss/GPT-SoVITS) **GPT Sovits原项目**  
[gpt_sovits_rs](https://github.com/second-state/gpt_sovits_rs) **GPT Sovits的TorchScript模型导出与Rust推理实现参考**  

