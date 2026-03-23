# qrcode_detect Agent Guide

This file provides guidance to AI coding agents working inside this repository.

## Project Overview

A QR code detection library and service that supports multiple detection backends:
- **wechat_qrcode**: OpenCV wechat_qrcode module
- **YOLOv3**: YOLOv3-based QR code detection (yolo-qrcode-opencv)
- **OpenCV**: Built-in QRCodeDetector
- **ZBar**: ZBar library

The project provides both a CLI tool (`qrcode_detect_cli`) and an HTTP server (`qrcode_detect_server`).

## Build Commands

```bash
# Download third-party dependencies
make download_dependencies

# Build third-party dependencies
make build_dependencies

# Build macOS with Xcode
mkdir build_dir && cd build_dir && cmake -G Xcode ../ && cmake --build . --config Release

# Or use Makefile (Linux)
make -f Makefile build_dependencies

# Build DEB package (Linux)
make build_deb

# Build Docker image
make build_docker
```

## Project Architecture

### Executables

| Target | Description |
|--------|-------------|
| `qrcode_detect_cli` | Command-line QR detection tool |
| `qrcode_detect_server` | HTTP server with REST API |

### Source Structure

- `src/cli/main.cpp` - CLI entry point
- `src/server/main.cpp` - Server entry point
- `src/libs/core/detector.cpp` - Base detector interface
- `src/libs/core/detector_factory.cpp` - Factory for creating detectors
- `src/libs/core/wechat_detector.cpp` - WeChat QR detector implementation
- `src/libs/core/yolov3_detector.cpp` - YOLOv3 detector implementation
- `src/libs/core/opencv_detector.cpp` - OpenCV built-in detector
- `src/libs/core/zbar_detector.cpp` - ZBar detector implementation

### Key Classes

- `Detector` (interface) - Base class for all detectors in `include/qrcode_detect/core/detector.hpp`
- `DetectorFactory` - Creates detector instances based on type
- `DetectResult` - Contains detection results with coordinates and content

### Dependencies

- **opencv/opencv_contrib** - Image processing and wechat_qrcode module
- **zbar** - Barcode detection
- **libhv** - HTTP server framework
- **spdlog** - Logging
- **argparse** - CLI argument parsing

## Running

### CLI
```bash
qrcode_detect_cli --model ./install/models --input <image_path> --type 1 --display
```

### Server
```bash
qrcode_detect_server --port 9999 --model ./install/models
```

### API Endpoints
- `POST /detect` - Detect QR codes (accepts URL or base64)
- `POST /detect_file` - Detect from uploaded file
- `GET /health` - Health check