# qrcode_detect

### 本项目基于下列开源项目构建

- [opencv](https://github.com/opencv/opencv)
- [opencv_contrib wechat_qrcode](https://github.com/opencv/opencv_contrib)
- [zbar](https://github.com/mchehab/zbar)
- [yolo-qrcode-opencv](https://github.com/hpc203/yolo-qrcode-opencv)
- [libhv](https://github.com/ithewei/libhv)
- [spdlog](https://github.com/gabime/spdlog)

### Build

- `Linux`平台需要安装`libeigen3-dev` `libjpeg-turbo8-dev` `libeigen3-dev` `autopoint`

```bash
git clone https://github.com/0x1306a94/qrcode_detect.git
cd qrcode_detect

# 下载三方库依赖
make download_dependencies

# 编译三方库依赖
make build_dependencies

# 构建deb安装包, 可选
make build_deb

# 构建Docker镜像, 可选
make build_docker

# macOS Xcode编译
mkdir build_dir
cd build_dir
cmake -G Xcode ../
cmake --build . --config Release
cmake --install . --config Release

```

### 运行命令行

CLI 使用子命令模式，分为 `detect` 和 `draw` 两个子命令：

#### detect 子命令 - 检测二维码

```bash
qrcode_detect_cli detect --model VAR --type VAR --input VAR [--display] [--maxWindowSize] [--overlapRatio]
```

| 参数 | 说明 |
| --- | --- |
| --model | 模型文件目录 (必填) |
| --type | 检测器类型: 1 wechat, 2 yolov3, 3 opencv, 4 zbar (默认: 1) |
| --input | 本地文件路径或远程图片链接 (必填) |
| --display | 显示检测结果框 |
| --maxWindowSize | 滑动窗口最大边长，> 0 启用滑动窗口模式 |
| --overlapRatio | 滑动窗口重叠比例，默认 0.2 |

```bash
# 检测本地图片
qrcode_detect_cli detect --model ./install/models --input ./test.jpg --type 1

# 检测远程图片
qrcode_detect_cli detect --model ./install/models --input "https://example.com/qr.jpg" --type 1

# 使用滑动窗口检测大图
qrcode_detect_cli detect --model ./install/models --input ./large_image.jpg --maxWindowSize 1280 --overlapRatio 0.2
```

#### visualize 子命令 - 可视化滑动窗口预览

```bash
qrcode_detect_cli visualize -i VAR -o VAR [--maxWindowSize] [--overlapRatio]
```

| 参数 | 说明 |
| --- | --- |
| -i, --input | 输入图片路径 (必填) |
| -o, --output | 输出图片路径 (必填) |
| --maxWindowSize | 滑动窗口最大边长 (默认: 1280) |
| --overlapRatio | 滑动窗口重叠比例 (默认: 0.2) |

```bash
# 可视化滑动窗口预览
qrcode_detect_cli visualize -i ./large_image.jpg -o ./windows_preview.jpg



### 运行服务端

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

- 运行示例

```bash
# 运行
./install/bin/qrcode_detect_server --port 9999 --model ./install/models
```

### 调用接口

- `type` 参数控制使用哪种检测器


| 值   | 检测器                                                                             |
| --- | ------------------------------------------------------------------------------- |
| 1   | [wechat_qrcode](https://github.com/opencv/opencv_contrib/modules/wechat_qrcode) |
| 2   | [YOLOV3](https://github.com/hpc203/yolo-qrcode-opencv)                          |
| 3   | [OpenCV QRCodeDetector](https://github.com/opencv/opencv)                       |
| 4   | [ZBar](https://github.com/mchehab/zbar)                                         |


- 请求结果示例,当`values`不为空时则表示原图上包含了二维码.

```json5
{
    "state": 0,
    "msg": "错误信息",
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

- 通过图片链接

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

- 通过图片base64

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

- 上传文件

```bash
curl --location 'http://127.0.0.1:9999/detect_file' \
--form 'type="1"' \
--form 'file=@"/Users/king/Documents/lanhu_icon/IMG_7562.jpg"'
```

- 健康检查

```bash
curl --location 'http://127.0.0.1:9999/health'
```

### 滑动窗口检测

对于超宽超长的图片，直接检测可能会出现漏检。可以使用滑动窗口模式将大图分割成多个重叠的小窗口进行检测，提高检出率。

#### 参数说明

| 参数 | 类型 | 默认值 | 说明 |
| --- | --- | --- | --- |
| maxWindowSize | int | 0 | 滑动窗口的最大边长。设置为 > 0 时启用滑动窗口模式，0 或不设置则禁用 |
| overlapRatio | float | 0.2 | 相邻窗口之间的重叠比例，范围 0.0-1.0，推荐 0.2 (20%) |

#### 使用示例

- JSON 请求（启用滑动窗口）

```bash
curl --location 'http://127.0.0.1:9999/detect' \
--header 'Content-Type: application/json' \
--data-raw '{
    "type" : 1,
    "url" : ["https://example.com/large_image.jpg"],
    "maxWindowSize": 1280,
    "overlapRatio": 0.2
}'
```

- 上传文件（启用滑动窗口）

```bash
curl --location 'http://127.0.0.1:9999/detect_file' \
--form 'type="1"' \
--form 'maxWindowSize="1280"' \
--form 'overlapRatio="0.2"' \
--form 'file=@"/path/to/large_image.jpg"'
```

#### 原理示意图

```
原图 (超宽超长图片，例如 4000x2000)
+--------------------------------------------------+
|                                                  |
|     [二维码 A]                    [二维码 B]    |
|       □□□                       ■■■■           |
|       □□□                       ■■■■           |
|                                                  |
+--------------------------------------------------+

启用滑动窗口 (maxWindowSize=1280, overlapRatio=0.2)
步长 = 1280 * (1 - 0.2) = 1024

窗口 1 (0,0)        窗口 2 (1024,0)    窗口 3 (2048,0)   窗口 4 (3072,0)
+------------+    +------------+      +------------+    +------------+
|            |    |            |      |            |    |            |
|  □□□       |    |            |      |      ■■■■  |    |      ■■■■  |
|  □□□       |    |            |      |      ■■■■  |    |      ■■■■  |
|            |    |            |      |            |    |            |
+------------+    +------------+      +------------+    +------------+
      ↓                ↓                 ↓                 ↓
   检测到 A         无二维码          检测到 B          检测到 B (重复)
                                                             ↓
                                                    使用 IoU 去重
                                                             ↓
最终结果: [二维码 A], [二维码 B]
```

**参数计算：**

| 参数 | 值 | 说明 |
| --- | --- | --- |
| maxWindowSize | 1280 | 每个窗口最大边长 1280 像素 |
| overlapRatio | 0.2 | 窗口之间重叠 20% |
| 步长 | 1024 | 1280 × (1 - 0.2) = 1024 |
| 水平窗口数 | ceil(4000 / 1024) = 4 | |
| 垂直窗口数 | ceil(2000 / 1024) = 2 | |
| 总窗口数 | 4 × 2 = 8 | |

**去重逻辑：**

```
判断两个检测结果是否为同一个二维码：
1. 内容相同 (content 相等)
2. 位置重叠 > 50% (IoU > 0.5)

保留第一个，去除重复
```

