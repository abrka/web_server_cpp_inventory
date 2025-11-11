#pragma once

#include <iostream>

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

#include "json_adl_pqxx_row.hpp"

namespace nlohmann {
template <> struct adl_serializer<pqxx::result> {
  static void to_json(nlohmann::json &j, const pqxx::result &d) {
    for (const auto &row : d) {
      j.push_back(row);
    }
  }
};
} // namespace nlohmann