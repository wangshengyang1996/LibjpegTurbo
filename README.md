# LibjpegTurbo demo
## 1. 编译选择
- 编译libjpeg-turbo：
    1. build.gradle中的CMakeLists.txt脚本使用`path "src/main/cpp/libjpeg_turbo/CMakeLists.txt"`
    2. 执行externalBuildNative[Debug|Release]
- 编译工程：
    1. build.gradle中的CMakeLists.txt脚本使用`path "src/main/cpp/CMakeLists.txt"`
    2. run

## 2. demo功能
  分别以libjpeg-turbo的方式和Bitmap API自带的方式对图像进行编解码
