# qrcode_detect

### 本项目基于下列开源项目构建
* [opencv 4.9.0](https://github.com/opencv/opencv/tree/4.9.0)
* [wechat_qrcode 4.9.0](https://github.com/opencv/opencv_contrib/tree/4.9.0/modules/wechat_qrcode)
* [zbar 0.23.93](https://github.com/mchehab/zbar/tree/0.23.93)
* [yolo-qrcode-opencv](https://github.com/hpc203/yolo-qrcode-opencv)
* [libhv 1.3.0](https://github.com/ithewei/libhv/tree/v1.3.0)
* [spdlog 1.11.0](https://github.com/gabime/spdlog/tree/v1.11.0)

### Build
* `Linux`平台需要安装`libeigen3-dev`
```bash
git clone https://github.com/0x1306a94/qrcode_detect.git
cd qrcode_detect

# 下载三方库依赖
./script/download_libopencv.sh
./script/download_libzbar.sh
./script/download_libhv.sh
./script/download_libspdlog.sh

# 编译三方库依赖
./script/build_libopencv.sh
./script/build_libzbar.sh
./script/build_libhv.sh
./script/build_libspdlog.sh

# 构建deb安装包, 可选
./script/build_deb.sh

# 编译
mkdir build_dir
cd build_dir
cmake -G Xcode ../
cmake --build . --config Release
cmake --install . --config Release

```
* 运行
```bash
Usage: qrcode_detect_server [--help] [--version] --port VAR --model VAR [--log VAR] [--daemon] [--pid VAR] [-V] [--verbose]

Optional arguments:
  -h, --help     shows help message and exits
  -v, --version  prints version information and exits
  -p, --port     service port. [required]
  -m, --model    model file directory. [required]
  -l, --log      log file directory.
  -d, --daemon   run daemon.
  --pid          pidfile path.
  -V             Set log level to critical, err, warn, info, debug, trace.
  --verbose      Set log level to trace.
```
* 运行示例
```bash
# 运行
./install/bin/qrcode_detect_server --port 9999 --model ./install/models
```

### 调用接口
* `type` 参数控制使用哪种检测器

|  值   | 检测器  |
|  ----  | ----  |
| 1  | [wechat_qrcode](https://github.com/opencv/opencv_contrib/modules/wechat_qrcode) |
| 2  | [YOLOV3](https://github.com/hpc203/yolo-qrcode-opencv) |
| 3  | [OpenCV QRCodeDetector](https://github.com/opencv/opencv) |
| 4  | [ZBar](https://github.com/mchehab/zbar) |

* 请求结果示例,当`values`不为空时则表示原图上包含了二维码.
```json5
{
    "code": 0,
    "result": [
        {
            "elapsed": 25, # 检测耗时，毫秒
            "values": [
                {
                    "content": "哈哈哈哈哈", # 二维码内容
                    "points": { 
                        "height": 351.76385498046875, # 二维码在原图上的位置
                        "width": 0.221099853515625,
                        "x": 23.492340087890625,
                        "y": 23.492340087890625
                    }
                }
            ]
        }
    ]
}
```
* 通过图片链接
```bash
curl --location 'http://127.0.0.1:9999/detect' \
--header 'Content-Type: application/json' \
--data-raw '{
    "type" : 1,
    "url" : [
        "https://img.zcool.cn/community/01ba0f5b041fc8a801209a8543e9a8.jpg@1280w_1l_2o_100sh.jpg"
    ]
}'
```
* 通过图片base64
```bash
curl --location 'http://127.0.0.1:9999/detect' \
--header 'Content-Type: application/json' \
--data '{
    "type": 1,
    "base64": [
        "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"
    ]
}'
```
* 上传文件
```bash
curl --location 'http://127.0.0.1:9999/detect_file' \
--form 'type="1"' \
--form 'file=@"/Users/king/Documents/lanhu_icon/IMG_7562.jpg"'
```
* 健康检查
```bash
curl --location 'http://127.0.0.1:9999/health'
```