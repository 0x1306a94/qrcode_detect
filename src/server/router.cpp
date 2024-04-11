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

    service.GET("/paths", [&service](HttpRequest *req, HttpResponse *resp) {
        return resp->Json(service.Paths());
    });
};

};  // namespace qrcode
