//
//  detect_handler.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#include "detect_handler.hpp"

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <hv/HttpContext.h>
#include <hv/HttpService.h>

#include "common_handler.hpp"

#include "../image_downloader.hpp"
#include "../reqparmas.hpp"

#include "../server_context.hpp"

#include <qrcode_detect/core/AutoBuffer.hpp>
#include <qrcode_detect/core/detect_result.hpp>
#include <qrcode_detect/core/detector.hpp>
#include <qrcode_detect/core/detector_factory.hpp>
#include <qrcode_detect/core/image_loader.hpp>
#include <qrcode_detect/core/sliding_window_detector.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace handler {

std::shared_ptr<qrcode::detect::Detector> DetectorFactory(qrcode::detect::DetectorType type,
                                                          const context::Context &ctx,
                                                          int maxWindowSize = 0,
                                                          float overlapRatio = 0.2f) {
    std::shared_ptr<qrcode::detect::Detector> detector = nullptr;
    if (type >= qrcode::detect::DetectorType::Wechat && type <= qrcode::detect::DetectorType::ZBar) {
        detector = qrcode::detect::DetectorFactory::Create(type, ctx.GetModelDir());
    } else {
        return nullptr;
    }

    // Wrap with sliding window detector if maxWindowSize > 0
    if (maxWindowSize > 0) {
        detector = std::make_shared<qrcode::detect::SlidingWindowDetector>(std::move(detector), maxWindowSize, overlapRatio);
    }

    return detector;
}

int Detect::detect(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    reqparmas::DetectRequest params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
    }

    auto detector = DetectorFactory(params.type, *context::Context::Current(), params.maxWindowSize, params.overlapRatio);
    if (!detector) {
        SPDLOG_ERROR("create detector fial type: {}", static_cast<int>(params.type));
        auto msg = fmt::format("create detector fial type: {}", static_cast<int>(params.type));
        return SendFail(ctx, 400, msg);
    }

    do {
        if (params.url.empty()) {
            break;
        }

        std::vector<qrcode::detect::Result> results;
        for (const auto &url : params.url) {
            if (!(url.find("http://") == 0 || url.find("https://") == 0)) {
                //                return SendFail(ctx, 400, fmt::format("{} Invalid url", url));
                results.emplace_back(0);
                continue;
            }
            qrcode::common::AutoBuffer buffer;
            spdlog::stopwatch sw;
            auto size = qrcode::download::image_from_url(url, buffer);
            SPDLOG_TRACE("加载网络图片: url: {} elapsed: {}", url, duration_cast<milliseconds>(sw.elapsed()));
            if (size == 0) {
                results.emplace_back(0);
                continue;
            }
            sw.reset();

            auto image = qrcode::detect::ImageLoader::fromBuffer(buffer);
            auto result = detector->detect(image);
            if (result) {
                results.push_back(result.value());
            } else {
                results.emplace_back(0);
            }
        }
        return SendSuccess(ctx, results);
    } while (0);

    do {
        if (params.base64.empty()) {
            break;
        }
        std::vector<qrcode::detect::Result> results;
        for (const auto &item : params.base64) {
            auto image = qrcode::detect::ImageLoader::fromBase64(item);
            auto result = detector->detect(image);
            if (result) {
                results.push_back(result.value());
            } else {
                results.emplace_back(0);
            }
        }
        return SendSuccess(ctx, results);
    } while (0);

    return SendFail(ctx, 400, "base64 and url empty");
}

int Detect::detectFile(const HttpContextPtr &ctx) {
    int status_code = HTTP_STATUS_UNFINISHED;
    std::string content = ctx->request->GetFormData("file");
    qrcode::detect::DetectorType type = static_cast<qrcode::detect::DetectorType>(ctx->request->Get<int>("type", 1));
    int maxWindowSize = ctx->request->Get<int>("maxWindowSize", 0);
    float overlapRatio = ctx->request->Get<float>("overlapRatio", 0.2f);
    if (content.empty()) {
        return SendFail(ctx, 400, "file empty");
    }
    auto detector = DetectorFactory(type, *context::Context::Current(), maxWindowSize, overlapRatio);
    if (!detector) {
        SPDLOG_ERROR("create detector fial type: {}", static_cast<int>(type));
        auto msg = fmt::format("create detector fial type: {}", static_cast<int>(type));
        return SendFail(ctx, 400, msg);
    }
    auto image = qrcode::detect::ImageLoader::fromBytes((const unsigned char *)content.data(), content.size());
    auto result = detector->detect(image);
    if (result) {
        return SendSuccess(ctx, result.value());
    }
    return SendSuccess(ctx, qrcode::detect::Result(0));
}
};  // namespace handler
