#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include "http_form.hpp"
#include "server_http.hpp"

namespace nlohmann {
template <> struct adl_serializer<std::tm> {
  static void to_json(json &j, const std::tm &tp) {
    std::time_t time_since_epoch = std::mktime(const_cast<std::tm *>(&tp));
    long int time_since_epoch_int = static_cast<long int>(time_since_epoch);
    j = time_since_epoch_int;
  }
  static void from_json(const json &j, std::tm &tp) {
    long int time_since_epoch_int = j.get<long int>();
    std::time_t time_since_epoch =
        static_cast<std::time_t>(time_since_epoch_int);
    tp = *localtime(&time_since_epoch);
  }
};
} // namespace nlohmann

struct Message {
  std::string user{};
  std::string text{};
  std::tm time_added{};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Message, text, user, time_added)

static time_t time_when_program_started = std::time(NULL);

static std::vector<Message> messages{
    Message{"user 1", "message 1", *std::localtime(&time_when_program_started)},
    Message{"user 2", "message 2",
            *std::localtime(&time_when_program_started)}};

int main() {

  std::cout << "init\n";

  ServerHTTP::Server server{};
  server.mount_point = "../assets";
  server.index_file = "articles.html";

  inja::Environment inja_env{"../assets/"};

  server.register_http_req_handler(
      std::regex{"/"}, "GET", [&inja_env](const HTTP::HttpRequest &) {
        nlohmann::json json_data{{"messages", messages}};
        std::string rendered_output =
            inja_env.render_file("index.j2.html", json_data);
        return HTTP::HttpResponse{200, "OK", {}, rendered_output};
      });

  server.register_http_req_handler(
      std::regex{"/new"}, "GET", [&inja_env](const HTTP::HttpRequest &) {
        std::string rendered_output = inja_env.render_file("form.j2.html", {});
        return HTTP::HttpResponse{200, "OK", {}, rendered_output};
      });

  server.register_http_req_handler(
      std::regex{R"(/messages/(\d+))"}, "GET",
      [&inja_env](const HTTP::HttpRequest &req) {
        int msg_idx = std::stoi(req.custom_params[1]);
        if (msg_idx < 0 || msg_idx >= messages.size()) {
          return HTTP::HttpResponse{
              200, "OK", {}, "No such message with index exists"};
        }
        nlohmann::json json_data{{"msg", messages[msg_idx]}};
        std::string rendered_output =
            inja_env.render_file("message.j2.html", json_data);
        return HTTP::HttpResponse{200, "OK", {}, rendered_output};
      });

  server.register_http_req_handler(
      std::regex{"/new"}, "POST", [](const HTTP::HttpRequest &req) {
        auto form_data = HTTP::parse_http_urlencoded_form_body(req.body);
        std::string new_post_username = form_data["username"];
        std::string new_post_message_body = form_data["message_body"];
        time_t now = std::time(NULL);
        Message new_msg{new_post_username, new_post_message_body,
                        *std::localtime(&now)};
        messages.push_back(new_msg);
        HTTP::HttpResponse res{301, "OK", {{"Location", "/"}}, ""};
        return res;
      });

  std::string ip = "0.0.0.0";
  std::string port = "3000";
  server.init(ip.c_str(), port.c_str(), 10);
  server.listen();
  return 0;
}