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
```cmake
# 21行与23行替换为你的LibTorch目录
set(Tarch_Base_Dir "...")
# 28行替换为cuda的lib64路径
# 例如
link_directories("C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.1/lib64")
```

准备完成后就能编译了
```shell
mkdir build
cd build
cmake ..
make build
```