//
//  nlohmann_json_reponse_provide.cpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#include "nlohmann_json_reponse_provide.hpp"

#include "../reqparmas.hpp"

#include <qrcode_detect/core/detect_result.hpp>

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

namespace ns {
void from_json(const nlohmann::json &j, reqparmas::DetectRequest &request) {
    if (j.contains("type")) {
        request.type = static_cast<qrcode::detect::DetectorType>(j["type"].get<int>());
    } else {
        request.type = qrcode::detect::DetectorType::Wechat;
    }

    if (j.contains("base64") && j["base64"].is_array()) {
        request.base64 = j["base64"].get<std::vector<std::string>>();
    }

    if (j.contains("url") && j["url"].is_array()) {
        request.url = j["url"].get<std::vector<std::string>>();
    }

    // Sliding window parameters
    if (j.contains("maxWindowSize") && j["maxWindowSize"].is_number_integer()) {
        request.maxWindowSize = j["maxWindowSize"].get<int>();
    }

    if (j.contains("overlapRatio") && j["overlapRatio"].is_number()) {
        request.overlapRatio = j["overlapRatio"].get<float>();
    }

    // Cache control
    if (j.contains("cache") && j["cache"].is_boolean()) {
        request.cache = j["cache"].get<bool>();
    }
}

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

void from_json(const nlohmann::json &j, qrcode::detect::Rect &rect) {
    if (j.contains("x")) {
        rect.x = j["x"].get<int>();
    }
    if (j.contains("y")) {
        rect.y = j["y"].get<int>();
    }
    if (j.contains("width")) {
        rect.width = j["width"].get<int>();
    }
    if (j.contains("height")) {
        rect.height = j["height"].get<int>();
    }
}

void from_json(const nlohmann::json &j, qrcode::detect::Value &value) {
    if (j.contains("content")) {
        value.content = j["content"].get<std::string>();
    }
    if (j.contains("points")) {
        from_json(j["points"], value.rect);
    }
}

void from_json(const nlohmann::json &j, qrcode::detect::Result &result) {
    if (j.contains("elapsed")) {
        result.elapsed = j["elapsed"].get<uint64_t>();
    }
    if (j.contains("values") && j["values"].is_array()) {
        result.values.clear();
        for (const auto &item : j["values"]) {
            qrcode::detect::Value value("");
            from_json(item, value);
            result.values.push_back(value);
        }
    }
}

};  // namespace ns
