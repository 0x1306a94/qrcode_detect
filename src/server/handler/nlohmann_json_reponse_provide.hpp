//
//  nlohmann_json_reponse_provide.hpp
//  face_id_cli
//
//  Created by king on 2024/2/6.
//

#ifndef _nlohmann_json_reponse_provide_hpp
#define _nlohmann_json_reponse_provide_hpp

#include <hv/json.hpp>

#include <vector>

namespace qrcode {
namespace detect {
struct Rect;
struct Value;
struct Result;
};  // namespace detect
};  // namespace qrcode

namespace reqparmas {
struct DetectRequest;
struct DetectResponse;
};  // namespace reqparmas

namespace ns {
void from_json(const nlohmann::json &j, reqparmas::DetectRequest &request);
void to_json(nlohmann::json &j, const qrcode::detect::Result &result);
void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Result> &results);
void to_json(nlohmann::json &j, const qrcode::detect::Value &value);
void to_json(nlohmann::json &j, const std::vector<qrcode::detect::Value> &values);
void to_json(nlohmann::json &j, const qrcode::detect::Rect &rect);

};  // namespace ns

#endif /* _nlohmann_json_reponse_provide_hpp */
