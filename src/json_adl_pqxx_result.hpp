#pragma once

#include <iostream>

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace nlohmann {
template <> struct adl_serializer<pqxx::result> {
  static void to_json(nlohmann::json &j, const pqxx::result &d) {
    j = nlohmann::json::array();
    for (const auto &row : d) {
      j.push_back(nlohmann::json::parse("{}"));
      for (const auto &field : row) {
        (j.back())[field.name()] = field.c_str();
      }
    }
  }
};
} // namespace nlohmann