#include <memory>
#include <sstream>
#include <string_view>

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

#include <qrcode_detect_cli/version.hpp>

#include <core/AutoBuffer.hpp>
#include <core/detect_result.hpp>
#include <core/detector.hpp>
#include <core/detector_factory.hpp>
#include <core/detector_type.hpp>

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
        if (!resp->headers["Location"].empty()) return;
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

int main(int argc, char *argv[]) {

    spdlog::set_level(spdlog::level::off);

    std::stringstream ss;
    ss << "qrcode_detect_cli: " << QRCODE_DETECT_CLI_VERSION << " " << QRCODE_DETECT_CLI_GIT_BRANCH << " " << QRCODE_DETECT_CLI_GIT_HASH;
    argparse::ArgumentParser program("qrcode_detect_cli", ss.str(), argparse::default_arguments::all, true, std::cerr);

    program.add_argument("--model")
        .required()
        .help("model file directory.");

    program.add_argument("--type")
        .required()
        .scan<'i', int>()
        .default_value(1)
        .help("1 wechat 2 yolov3 3 opencv 4 zbar.");

    program.add_argument("--input")
        .required()
        .help("local file path or remote image link.");

    program.add_argument("--display")
        .default_value(false)
        .implicit_value(true)
        .help("display rectangular area.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    int argType = program.get<int>("--type");
    if (argType < static_cast<int>(qrcode::detect::DetectorType::Wechat) || argType > static_cast<int>(qrcode::detect::DetectorType::ZBar)) {
        std::cerr << "type parameter is invalid." << std::endl;
        return EXIT_SUCCESS;
    }

    std::string model_dir = program.get<std::string>("--model");
    std::string input = program.get<std::string>("--input");
    bool display = program.is_used("--display");

    qrcode::detect::DetectorFactory detectorFactory{};
    std::shared_ptr<qrcode::detect::Detector> detector{detectorFactory.Create(static_cast<qrcode::detect::DetectorType>(argType), model_dir)};

    if (input.find("https://") != std::string::npos || input.find("http://") != std::string::npos) {
        qrcode::common::AutoBuffer buffer;
        auto size = image_from_url(input, buffer);
        if (size == 0) {
            std::cerr << "failed to load the network image" << std::endl;
            return EXIT_SUCCESS;
        }
        auto result = detector->DetectFromBuffer(buffer);
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
        auto result = detector->DetectFromPath(input);
        if (result) {
            print_result(result.value());
            if (display) {
                cv::Mat img = cv::imread(input);
                dispaly_result(result.value(), img);
            }
        }
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
