//
//  router.cpp
//  qrcode_detect_server
//
//  Created by king on 2024/3/29.
//

#include "router.hpp"

#include "handler/common_handler.hpp"

#include "handler/detect_handler.hpp"

#include <hv/HttpService.h>

#include <qrcode_detect/core/version.hpp>

namespace qrcode {
void Router::Register(hv::HttpService &service) {
    service.preprocessor = http_handler([](const HttpContextPtr &ctx) -> int {
        ctx->response->content_type = APPLICATION_JSON;
        if (ctx->is(MULTIPART_FORM_DATA)) {
            return HTTP_STATUS_UNFINISHED;
        }
        if (ctx->method() == http_method::HTTP_POST && ctx->request->content_length > 0 && !ctx->is(APPLICATION_JSON)) {
            return handler::SendFail(ctx, HTTP_STATUS_BAD_REQUEST, "request body must be in json format");
        }
        return HTTP_STATUS_UNFINISHED;
    });

    service.POST("/detect", handler::Detect::detect);
    service.POST("/detect_file", handler::Detect::detectFile);

    //    service.GET("/paths", [](const HttpContextPtr &ctx) -> int {
    //        return ctx->sendJson(ctx->service->Paths());
    //    });

    service.GET("/health", [](const HttpContextPtr &ctx) -> int {
        return handler::SendSuccess(ctx);
    });

    service.GET("/version", [](const HttpContextPtr &ctx) -> int {
        nlohmann::json version;
        version["version"] = QRCODE_DETECT_VERSION_FULL;
        version["buildTimestamp"] = QRCODE_DETECT_BUILD_TIMESTAMP;
        version["branch"] = QRCODE_DETECT_GIT_BRANCH;
        version["commit"] = QRCODE_DETECT_GIT_HASH;
        return handler::SendSuccess(ctx, version);
    });
};

};  // namespace qrcode
