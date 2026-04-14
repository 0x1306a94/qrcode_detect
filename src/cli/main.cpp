#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string_view>
#include <vector>

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <hv/json.hpp>
#include <hv/requests.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <qrcode_detect/core/AutoBuffer.hpp>
#include <qrcode_detect/core/detect_result.hpp>
#include <qrcode_detect/core/detector.hpp>
#include <qrcode_detect/core/detector_factory.hpp>
#include <qrcode_detect/core/detector_type.hpp>
#include <qrcode_detect/core/image_loader.hpp>
#include <qrcode_detect/core/sliding_window_detector.hpp>
#include <qrcode_detect/core/version.hpp>

namespace ns {

void to_json(nlohmann::json &j, const qrcode::detect::Result &result);
void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Result> &results);
void to_json(nlohmann::json &j, const qrcode::detect::Value &value);
void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Value> &values);
void to_json(nlohmann::json &j, const qrcode::detect::Rect &rect);

void to_json(nlohmann::json &j, const qrcode::detect::Result &result) {
    auto values = nlohmann::json::array();
    to_json(values, result.values);
    j = nlohmann::json{
        {"elapsed", result.elapsed},
        {"values", values},
    };
}

void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Result> &results) {
    j = nlohmann::json::array();
    for (const auto &result : results) {
        nlohmann::json obj;
        to_json(obj, result);
        j.push_back(obj);
    }
}

void to_json(nlohmann::json &j, const qrcode::detect::Value &value) {
    auto rect = nlohmann::json::object();
    to_json(rect, value.rect);
    j = nlohmann::json{
        {"content", value.content},
        {"points", rect},
    };
}

void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Value> &values) {
    j = nlohmann::json::array();
    for (const auto &value : values) {
        nlohmann::json obj;
        to_json(obj, value);
        j.push_back(obj);
    }
}

void to_json(nlohmann::json &j, const qrcode::detect::Rect &rect) {
    j = nlohmann::json{
        {"x", rect.x},
        {"y", rect.y},
        {"width", rect.width},
        {"height", rect.height},
    };
}

};  // namespace ns

std::size_t image_from_url(std::string_view url, qrcode::common::AutoBuffer &buffer) {
    requests::Request req(new HttpRequest);
    req->method = HTTP_GET;
    req->url = url;
    req->timeout = 10;  // 1h
    size_t content_length = 0;
    size_t received_bytes = 0;
    req->http_cb = [&buffer, &content_length, &received_bytes](HttpMessage *resp, http_parser_state state, const char *data, size_t size) {
        if (!resp->headers["Location"].empty())
            return;
        if (state == HP_HEADERS_COMPLETE) {
            content_length = hv::from_string<size_t>(resp->GetHeader("Content-Length"));
        } else if (state == HP_BODY) {
            if (data && size) {
                buffer.Append(data, size);
                received_bytes += size;
            }
        }
    };

    auto resp = requests::request(req);
    if (resp == NULL || resp->status_code != 200) {
        return 0;
    }
    return received_bytes;
}

void dispaly_result(const qrcode::detect::Result &result, const cv::Mat &source_img) {

    cv::Mat resultimg = source_img.clone();
    for (const auto &item : result.values) {
        auto rect = item.rect;
        cv::rectangle(resultimg, cv::Rect(rect.x, rect.y, rect.width, rect.height), cv::Scalar(0, 0, 255), 2, cv::LINE_8, 0);
    }
    static const std::string kWinName = "result";
    cv::namedWindow(kWinName, cv::WINDOW_NORMAL);
    cv::imshow(kWinName, resultimg);
    cv::waitKey(0);
    cv::destroyAllWindows();
}

void print_result(const qrcode::detect::Result &result) {
    hv::Json json;
    ns::to_json(json, result);
    std::cout << json.dump(4) << std::endl;
}

void print_opencv_runtime_feature(const std::string &name, int feature) {
    std::cout << name << ": " << (cv::checkHardwareSupport(feature) ? "ON" : "OFF") << std::endl;
}

void print_opencv_info(bool verbose) {
    std::cout << "opencv_version: " << CV_VERSION << std::endl;
    std::cout << "opencv_version_string: " << cv::getVersionString() << std::endl;
    std::cout << "opencv_optimized: " << (cv::useOptimized() ? "ON" : "OFF") << std::endl;
    std::cout << "opencv_threads: " << cv::getNumThreads() << std::endl;

    print_opencv_runtime_feature("cpu_sse3", CV_CPU_SSE3);
    print_opencv_runtime_feature("cpu_ssse3", CV_CPU_SSSE3);
    print_opencv_runtime_feature("cpu_sse4_1", CV_CPU_SSE4_1);
    print_opencv_runtime_feature("cpu_sse4_2", CV_CPU_SSE4_2);
    print_opencv_runtime_feature("cpu_avx", CV_CPU_AVX);
    print_opencv_runtime_feature("cpu_fp16", CV_CPU_FP16);
    print_opencv_runtime_feature("cpu_avx2", CV_CPU_AVX2);
    print_opencv_runtime_feature("cpu_avx512_skx", CV_CPU_AVX512_SKX);
    print_opencv_runtime_feature("cpu_neon", CV_CPU_NEON);
    print_opencv_runtime_feature("cpu_neon_dotprod", CV_CPU_NEON_DOTPROD);
    print_opencv_runtime_feature("cpu_neon_fp16", CV_CPU_NEON_FP16);
    print_opencv_runtime_feature("cpu_neon_bf16", CV_CPU_NEON_BF16);

    if (verbose) {
        std::cout << "\n"
                  << cv::getBuildInformation() << std::endl;
    }
}

static int LOG_verbosity = SPDLOG_LEVEL_OFF;

int main(int argc, char *argv[]) {

    std::stringstream ss;
    ss << "version: " << QRCODE_DETECT_VERSION_FULL << "\n"
       << "build_date: " << QRCODE_DETECT_BUILD_TIMESTAMP << "\n"
       << "git_url: " << QRCODE_DETECT_GIT_URL << "\n"
       << "git_branch: " << QRCODE_DETECT_GIT_BRANCH << "\n"
       << "git_hash: " << QRCODE_DETECT_GIT_HASH;

    argparse::ArgumentParser program("qrcode_detect_cli", ss.str(), argparse::default_arguments::help, true, std::cerr);

    // Add subcommands
    argparse::ArgumentParser detect_cmd("detect", ss.str(), argparse::default_arguments::help, true, std::cerr);
    argparse::ArgumentParser visualize_cmd("visualize", ss.str(), argparse::default_arguments::help, true, std::cerr);
    argparse::ArgumentParser opencv_info_cmd("opencv_info", ss.str(), argparse::default_arguments::help, true, std::cerr);

    // detect subcommand arguments
    detect_cmd.add_argument("-m", "--model")
        .required()
        .help("model file directory.");

    detect_cmd.add_argument("--type")
        .scan<'i', int>()
        .default_value(1)
        .help("1 wechat 2 yolov3 3 opencv 4 zbar.");

    detect_cmd.add_argument("-i", "--input")
        .required()
        .help("local file path or remote image link.");

    detect_cmd.add_argument("--display")
        .default_value(false)
        .implicit_value(true)
        .help("display rectangular area.");

    detect_cmd.add_argument("--maxWindowSize")
        .scan<'i', int>()
        .default_value(0)
        .help("sliding window max size, > 0 to enable sliding window mode.");

    detect_cmd.add_argument("--overlapRatio")
        .scan<'g', float>()
        .default_value(0.2f)
        .help("sliding window overlap ratio, 0.0-1.0, default 0.2.");

    auto &detect_verbosity_group = detect_cmd.add_mutually_exclusive_group();
    detect_verbosity_group.add_argument("-V")
        .action([&](const auto &) {
            --LOG_verbosity;
        })
        .append()
        .implicit_value(true)
        .nargs(0)
        .help("Set log level to critical, err, warn, info, debug, trace.");

    detect_verbosity_group.add_argument("--verbose")
        .action([&](const auto &) {
            LOG_verbosity = SPDLOG_LEVEL_TRACE;
        })
        .default_value(false)
        .implicit_value(true)
        .help("Set log level to trace.");

    // visualize subcommand arguments
    visualize_cmd.add_argument("-i", "--input")
        .required()
        .help("local file path or remote image link.");

    visualize_cmd.add_argument("--maxWindowSize")
        .scan<'i', int>()
        .default_value(1280)
        .help("sliding window max size, default 1280.");

    visualize_cmd.add_argument("--overlapRatio")
        .scan<'g', float>()
        .default_value(0.2f)
        .help("sliding window overlap ratio, 0.0-1.0, default 0.2.");

    visualize_cmd.add_argument("-o", "--output")
        .required()
        .help("output image path.");

    opencv_info_cmd.add_argument("--verbose")
        .default_value(false)
        .implicit_value(true)
        .help("print full OpenCV build information.");

    program.add_subparser(detect_cmd);
    program.add_subparser(visualize_cmd);
    program.add_subparser(opencv_info_cmd);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    // Handle detect subcommand
    if (program.is_subcommand_used("detect")) {
        if (LOG_verbosity < 0) {
            spdlog::set_level(spdlog::level::off);
        } else {
            spdlog::set_level(static_cast<spdlog::level::level_enum>(LOG_verbosity));
        }

        std::string input = detect_cmd.get<std::string>("--input");
        std::string model_dir = detect_cmd.get<std::string>("--model");
        int argType = detect_cmd.get<int>("--type");
        bool display = detect_cmd.is_used("--display");
        int maxWindowSize = detect_cmd.get<int>("--maxWindowSize");
        float overlapRatio = detect_cmd.get<float>("--overlapRatio");

        if (argType < static_cast<int>(qrcode::detect::DetectorType::Wechat) || argType > static_cast<int>(qrcode::detect::DetectorType::ZBar)) {
            std::cerr << "type parameter is invalid." << std::endl;
            return EXIT_SUCCESS;
        }

        auto detector = qrcode::detect::DetectorFactory::Create(static_cast<qrcode::detect::DetectorType>(argType), model_dir);
        // Wrap with sliding window detector if maxWindowSize > 0
        if (maxWindowSize > 0) {
            detector = std::make_shared<qrcode::detect::SlidingWindowDetector>(std::move(detector), maxWindowSize, overlapRatio);
        }

        if (input.find("https://") != std::string::npos || input.find("http://") != std::string::npos) {
            qrcode::common::AutoBuffer buffer;
            auto size = image_from_url(input, buffer);
            if (size == 0) {
                std::cerr << "failed to load the network image" << std::endl;
                return EXIT_SUCCESS;
            }
            auto image = qrcode::detect::ImageLoader::fromBuffer(buffer);
            auto result = detector->detect(image);
            if (result) {
                print_result(result.value());
                if (display) {
                    std::vector<uchar> data(buffer.data(), buffer.data() + buffer.size());
                    cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
                    dispaly_result(result.value(), img);
                }
            }
            return EXIT_SUCCESS;
        }

        try {
            auto image = qrcode::detect::ImageLoader::fromPath(input);
            auto result = detector->detect(image);
            if (result) {
                print_result(result.value());
                if (display) {
                    cv::Mat img = cv::imread(input);
                    dispaly_result(result.value(), img);
                }
            }
        } catch (const std::exception &err) {
            std::cerr << err.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    // Handle visualize subcommand
    if (program.is_subcommand_used("visualize")) {
        std::string input = visualize_cmd.get<std::string>("--input");
        std::string output = visualize_cmd.get<std::string>("--output");
        int maxWindowSize = visualize_cmd.get<int>("--maxWindowSize");
        float overlapRatio = visualize_cmd.get<float>("--overlapRatio");

        // Initialize random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 255);

        try {
            auto image = qrcode::detect::ImageLoader::fromPath(input);
            if (!image || image->empty()) {
                std::cerr << "failed to load image: " << input << std::endl;
                return EXIT_FAILURE;
            }
            auto windows = qrcode::detect::SlidingWindowDetector::CalculateWindows(
                image->cols, image->rows, maxWindowSize, overlapRatio);
            cv::Mat img = cv::imread(input);

            int index = 0;
            for (const auto &rect : windows) {
                // Generate random color
                cv::Scalar color(dist(gen), dist(gen), dist(gen));

                // Draw rectangle
                cv::rectangle(img, cv::Rect(rect.x, rect.y, rect.width, rect.height),
                              color, 2, cv::LINE_8, 0);

                // Draw index number at center
                int centerX = rect.x + rect.width / 2;
                int centerY = rect.y + rect.height / 2;
                cv::putText(img, std::to_string(index),
                            cv::Point(centerX - 20, centerY + 20),
                            cv::FONT_HERSHEY_SIMPLEX, 2.0, color, 3);

                index++;
            }
            if (!cv::imwrite(output, img)) {
                std::cerr << "failed to save image: " << output << std::endl;
                return EXIT_FAILURE;
            }
            std::cout << "Saved sliding windows image to: " << output << std::endl;
            std::cout << "Total windows: " << windows.size() << std::endl;
            return EXIT_SUCCESS;
        } catch (const std::exception &err) {
            std::cerr << err.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    if (program.is_subcommand_used("opencv_info")) {
        bool verbose = opencv_info_cmd.get<bool>("--verbose");
        print_opencv_info(verbose);
        return EXIT_SUCCESS;
    }

    // No subcommand specified, show help
    std::cerr << program;
    return EXIT_FAILURE;
}
