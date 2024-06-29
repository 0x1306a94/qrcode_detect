//
//  common_handler.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#include "common_handler.hpp"

#include <hv/HttpContext.h>

namespace handler {
int SendSuccess(const HttpContextPtr &ctx) {
    nlohmann::json json;
    json["state"] = 0;
    json["result"] = true;
    return ctx->send(json.dump());
}

int SendSuccess(const HttpContextPtr &ctx, nlohmann::json result) {
    nlohmann::json json;
    json["state"] = 0;
    json["result"] = result;
    return ctx->send(json.dump());
}

int SendFail(const HttpContextPtr &ctx, long code, const std::string &msg) {
    nlohmann::json json;
    json["state"] = code;
    json["msg"] = msg;
    return ctx->send(json.dump());
}
};  // namespace handler
