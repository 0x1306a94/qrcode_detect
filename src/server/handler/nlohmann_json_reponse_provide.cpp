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

};  // namespace ns
