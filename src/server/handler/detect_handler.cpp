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
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <hv/HttpContext.h>
#include <hv/HttpService.h>

#include "common_handler.hpp"

#include "../image_downloader.hpp"
#include "../reqparmas.hpp"

#include "../detector_factory.hpp"
#include "../server_context.hpp"

#include <core/AutoBuffer.hpp>
#include <core/detect_result.hpp>
#include <core/detector.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace handler {

int Detect::detect(const HttpContextPtr &ctx) {
    auto json = ctx->request->GetJson();
    reqparmas::DetectRequest params;
    try {
        ns::from_json(json, params);
    } catch (const std::exception &err) {
        SPDLOG_ERROR("{}", err.what());
        return SendFail(ctx, 400, err.what());
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
                //                return SendFail(ctx, 400, fmt::format("{} 无法加载", url));
            }
            sw.reset();
            qrcode::detect::DetectorFactory detectorFactory{};
            auto detector = detectorFactory.Create(params.type, *context::Context::Current());
            auto result = detector->DetectFromBuffer(buffer);
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
        qrcode::detect::DetectorFactory detectorFactory{};
        auto detector = detectorFactory.Create(params.type, *context::Context::Current());
        for (const auto &item : params.base64) {
            auto result = detector->DetectFromBase64(item);
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
    reqparmas::DetectorType type = static_cast<reqparmas::DetectorType>(ctx->request->Get<int>("type"));
    if (content.empty()) {
        return SendFail(ctx, 400, "file empty");
    }
    qrcode::detect::DetectorFactory detectorFactory{};
    auto detector = detectorFactory.Create(type, *context::Context::Current());
    auto result = detector->DetectFromBytes((const unsigned char *)content.data(), content.size());
    if (result) {
        return SendSuccess(ctx, result.value());
    }
    return SendSuccess(ctx, qrcode::detect::Result(0));
}
};  // namespace handler
