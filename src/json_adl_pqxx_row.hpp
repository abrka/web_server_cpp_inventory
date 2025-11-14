#pragma once

#include <cassert>
#include <iostream>

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace nlohmann {
template <> struct adl_serializer<pqxx::row> {
  static void to_json(nlohmann::json &j, const pqxx::row &d) {
    j = nlohmann::json::object();
    for (const auto &field : d) {
      j[field.name()] = field.c_str();
    }
  }
};
} // namespace nlohmann