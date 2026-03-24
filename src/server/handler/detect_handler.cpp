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
#include <hv/json.hpp>

#include "common_handler.hpp"

#include "../MD5.h"
#include "../image_downloader.hpp"
#include "../reqparmas.hpp"
#include "../server_context.hpp"

#include <qrcode_detect/core/AutoBuffer.hpp>
#include <qrcode_detect/core/detect_result.hpp>
#include <qrcode_detect/core/detector.hpp>
#include <qrcode_detect/core/detector_factory.hpp>
#include <qrcode_detect/core/image_loader.hpp>
#include <qrcode_detect/core/sliding_window_detector.hpp>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using std::chrono::duration_cast;
using std::chrono::milliseconds;

namespace handler {

// Simple hash function for cache key
static std::string computeCacheKey(const std::string &url, int type, int maxWindowSize, float overlapRatio) {
    std::string data = url + "|" + std::to_string(type) + "|" + std::to_string(maxWindowSize) + "|" + std::to_string(overlapRatio);

    // Use MD5 for cache key
    MD5 md5;
    MD5_CTX ctx;
    md5.MD5Init(&ctx);
    md5.MD5Update(&ctx, (unsigned char *)data.data(), static_cast<unsigned int>(data.size()));
    unsigned char digest[16];
    md5.MD5Final(digest, &ctx);

    char hex[33];
    for (int i = 0; i < 16; i++) {
        snprintf(hex + i * 2, 3, "%02x", digest[i]);
    }
    hex[32] = '\0';
    return std::string(hex);
}

static std::string getCacheDir() {
    auto ctx = context::Context::Current();
    if (!ctx) {
        return "";
    }
    return ctx->GetCacheDir();
}

static bool loadCache(const std::string &cacheKey, qrcode::detect::Result &result) {
    std::string cacheDir = getCacheDir();
    if (cacheDir.empty()) {
        return false;
    }

    auto cachePath = fs::path(cacheDir) / (cacheKey + ".json");
    if (!fs::exists(cachePath)) {
        return false;
    }

    try {
        std::ifstream file(cachePath);
        if (!file.is_open()) {
            return false;
        }
        nlohmann::json j;
        file >> j;
        ns::from_json(j, result);
        SPDLOG_TRACE("cache hit: {}", cacheKey);
        return true;
    } catch (const std::exception &err) {
        SPDLOG_WARN("load cache failed: {} - {}", cachePath.string(), err.what());
        return false;
    }
}

static bool saveCache(const std::string &cacheKey, const qrcode::detect::Result &result) {
    std::string cacheDir = getCacheDir();
    if (cacheDir.empty()) {
        return false;
    }

    try {
        // Create cache directory if not exists
        if (!fs::exists(cacheDir)) {
            fs::create_directories(cacheDir);
        }

        auto cachePath = fs::path(cacheDir) / (cacheKey + ".json");
        nlohmann::json j;
        ns::to_json(j, result);
        std::ofstream file(cachePath);
        file << j.dump(2);
        SPDLOG_TRACE("cache saved: {}", cacheKey);
        return true;
    } catch (const std::exception &err) {
        SPDLOG_WARN("save cache failed: {} - {}", cacheKey, err.what());
        return false;
    }
}

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
        std::string cacheDir = getCacheDir();
        bool useCache = !cacheDir.empty() && params.cache;

        for (const auto &url : params.url) {
            if (!(url.find("http://") == 0 || url.find("https://") == 0)) {
                //                return SendFail(ctx, 400, fmt::format("{} Invalid url", url));
                results.emplace_back(0u);
                continue;
            }

            // Try to load from cache
            if (useCache) {
                std::string cacheKey = computeCacheKey(url, static_cast<int>(params.type), params.maxWindowSize, params.overlapRatio);
                qrcode::detect::Result cachedResult{0};
                if (loadCache(cacheKey, cachedResult)) {
                    results.push_back(cachedResult);
                    continue;
                }
            }

            qrcode::common::AutoBuffer buffer;
            spdlog::stopwatch sw;
            auto size = qrcode::download::image_from_url(url, buffer);
            SPDLOG_TRACE("加载网络图片: url: {} elapsed: {}", url, duration_cast<milliseconds>(sw.elapsed()));
            if (size == 0) {
                results.emplace_back(0u);
                continue;
            }
            sw.reset();

            auto image = qrcode::detect::ImageLoader::fromBuffer(buffer);
            auto result = detector->detect(image);
            if (result) {
                results.push_back(result.value());
                // Save to cache
                if (useCache) {
                    std::string cacheKey = computeCacheKey(url, static_cast<int>(params.type), params.maxWindowSize, params.overlapRatio);
                    saveCache(cacheKey, result.value());
                }
            } else {
                results.emplace_back(0u);
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
