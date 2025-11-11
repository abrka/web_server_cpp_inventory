#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <dotenv.h>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

#include "http_form.hpp"
#include "server_http.hpp"

#include "json_adl_pqxx_result.hpp"

int main() {
  dotenv::init("../.env");

  pqxx::connection pg_connection{};

  ServerHTTP::Server server{};
  std::string ip = "0.0.0.0";
  std::string port = "3000";
  server.init(ip.c_str(), port.c_str(), 10);

  server.register_http_req_handler(
      std::regex{R"(/items/(\d+))"}, "GET",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};
          std::string id = req.custom_params[1];
          pqxx::result pg_result = transaction.exec(
              "SELECT * FROM items WHERE id=$1;", pqxx::params{id});
          transaction.commit();

          return HTTP::HttpResponse{200,
                                    "OK",
                                    {{"Content-Type", "application/json"}},
                                    nlohmann::json(pg_result).dump()};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id GET handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/items/)"}, "GET",
      [&pg_connection](const HTTP::HttpRequest &) {
        try {
          pqxx::work transaction{pg_connection};
          pqxx::result pg_result = transaction.exec("SELECT * FROM items;");
          transaction.commit();

          return HTTP::HttpResponse{200,
                                    "OK",
                                    {{"Content-Type", "application/json"}},
                                    nlohmann::json(pg_result).dump()};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id GET handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/items)"}, "POST",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};

          nlohmann::json input_json = nlohmann::json::parse(req.body);
          std::string input_name = input_json["name"];
          std::string category_id = input_json["category_id"];
          pqxx::result pg_result =
              transaction.exec("INSERT INTO items(name,category_id) VALUES "
                               "($1, $2) RETURNING *;",
                               pqxx::params{input_name, category_id});
          std::string created_id = pg_result[0]["id"].c_str();
          transaction.commit();

          return HTTP::HttpResponse{
              201, "Created", {{"Location", "items/" + created_id}}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items POST handler in server", {}, ""};
        }
      });

  server.register_http_req_handler(
      std::regex{R"(/items/(\d+))"}, "PUT",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};

          std::string id = req.custom_params[1];
          nlohmann::json input_json = nlohmann::json::parse(req.body);
          std::string input_name = input_json["name"];
          std::string category_id = input_json["category_id"];
          transaction.exec(
              R"(UPDATE items SET name=$2, category_id=$3 WHERE id=$1;)",
              pqxx::params{id, input_name, category_id});
          transaction.commit();

          return HTTP::HttpResponse{204, "No Content", {}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id PUT handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/items/(\d+))"}, "DELETE",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};

          std::string id = req.custom_params[1];
          transaction.exec(R"(DELETE FROM items WHERE id=$1;)",
                           pqxx::params{id});
          transaction.commit();

          return HTTP::HttpResponse{204, "No Content", {}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id DELETE handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/categories/(\d+))"}, "GET",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};
          std::string id = req.custom_params[1];
          pqxx::result pg_result = transaction.exec(
              "SELECT * FROM categories WHERE id=$1;", pqxx::params{id});
          transaction.commit();

          return HTTP::HttpResponse{200,
                                    "OK",
                                    {{"Content-Type", "application/json"}},
                                    nlohmann::json(pg_result).dump()};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id GET handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/categories/)"}, "GET",
      [&pg_connection](const HTTP::HttpRequest &) {
        try {
          pqxx::work transaction{pg_connection};
          pqxx::result pg_result =
              transaction.exec("SELECT * FROM categories;");
          transaction.commit();

          return HTTP::HttpResponse{200,
                                    "OK",
                                    {{"Content-Type", "application/json"}},
                                    nlohmann::json(pg_result).dump()};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404, "Exception in /items/:id GET handler in server", {}, ""};
        }
      });
  server.register_http_req_handler(
      std::regex{R"(/categories)"}, "POST",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};

          nlohmann::json input_json = nlohmann::json::parse(req.body);
          std::string input_name = input_json["name"];
          pqxx::result pg_result = transaction.exec(
              "INSERT INTO categories(name) VALUES($1) RETURNING *;",
              pqxx::params{input_name});
          std::string created_id = pg_result[0]["id"].c_str();
          transaction.commit();

          return HTTP::HttpResponse{
              201, "Created", {{"Location", "categories/" + created_id}}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404,
              "Exception in /categories/:id POST handler in server",
              {},
              ""};
        }
      });

  server.register_http_req_handler(
      std::regex{R"(/categories/(\d+))"}, "PUT",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {
          pqxx::work transaction{pg_connection};

          std::string id = req.custom_params[1];
          nlohmann::json input_json = nlohmann::json::parse(req.body);
          std::string input_name = input_json["name"];
          transaction.exec(R"(UPDATE categories SET name=$2 WHERE id=$1;)",
                           pqxx::params{id, input_name});
          transaction.commit();

          return HTTP::HttpResponse{204, "No Content", {}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404,
              "Exception in /categories/:id PUT handler in server",
              {},
              ""};
        }
      });

  server.register_http_req_handler(
      std::regex{R"(/categories/(\d+))"}, "DELETE",
      [&pg_connection](const HTTP::HttpRequest &req) {
        try {

          std::string id = req.custom_params[1];

          pqxx::work transaction_0{pg_connection};
          transaction_0.exec(R"(DELETE FROM items WHERE category_id=$1;)",
                             pqxx::params{id});
          transaction_0.commit();

          pqxx::work transaction{pg_connection};
          transaction.exec(R"(DELETE FROM categories WHERE id=$1;)",
                           pqxx::params{id});
          transaction.commit();

          return HTTP::HttpResponse{204, "No Content", {}, ""};
        } catch (const std::exception &e) {
          std::cerr << "SERVER APP ERROR: Caught Exception: " << e.what()
                    << '\n';
          return HTTP::HttpResponse{
              404,
              "Exception in /categories/:id DELETE handler in server",
              {},
              ""};
        }
      });

  server.listen();
  return 0;
}