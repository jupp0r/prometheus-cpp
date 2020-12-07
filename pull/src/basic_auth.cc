#include "basic_auth.h"

#include "civetweb.h"
#include "detail/base64.h"
#include "prometheus/detail/future_std.h"

namespace prometheus {

BasicAuthHandler::BasicAuthHandler(AuthFunc callback, std::string realm)
    : callback_(std::move(callback)), realm_(std::move(realm)) {}

int BasicAuthHandler::authHandler(struct mg_connection* conn, void* cbdata) {
  auto* handler = reinterpret_cast<BasicAuthHandler*>(cbdata);
  if (!handler) {
    return 0;  // No handler found
  }

  return handler->Authorize(conn) ? 1 : 0;
}

bool BasicAuthHandler::Authorize(mg_connection* conn) {
  if (!AuthorizeInner(conn)) {
    WriteUnauthorizedResponse(conn);
    return false;
  }
  return true;
}

bool BasicAuthHandler::AuthorizeInner(mg_connection* conn) {
  const char* authHeader = mg_get_header(conn, "Authorization");

  if (authHeader == nullptr) {
    // No auth header was provided.
    return false;
  }
  std::string authHeaderStr = authHeader;

  // Basic auth header is expected to be of the form:
  // "Basic dXNlcm5hbWU6cGFzc3dvcmQ="

  const std::string prefix = "Basic ";
  if (authHeaderStr.compare(0, prefix.size(), prefix) != 0) {
    return false;
  }

  // Strip the "Basic " prefix leaving the base64 encoded auth string
  auto b64Auth = authHeaderStr.substr(prefix.size());

  std::string decoded;
  try {
    decoded = detail::base64_decode(b64Auth);
  } catch (...) {
    return false;
  }

  // decoded auth string is expected to be of the form:
  // "username:password"
  // colons may not appear in the username.
  auto splitPos = decoded.find(':');
  if (splitPos == std::string::npos) {
    return false;
  }

  auto username = decoded.substr(0, splitPos);
  auto password = decoded.substr(splitPos + 1);

  // TODO: bool does not permit a distinction between 401 Unauthorized
  //  and 403 Forbidden. Authentication may succeed, but the user still
  //  not be authorized to perform the request.
  return callback_(username, password);
}

void BasicAuthHandler::WriteUnauthorizedResponse(mg_connection* conn) {
  mg_printf(conn, "HTTP/1.1 401 Unauthorized\r\n");
  mg_printf(conn, "WWW-Authenticate: Basic realm=\"%s\"\r\n", realm_.c_str());
  mg_printf(conn, "Connection: close\r\n");
  mg_printf(conn, "Content-Length: 0\r\n");
  // end headers
  mg_printf(conn, "\r\n");
}

}  // namespace prometheus
