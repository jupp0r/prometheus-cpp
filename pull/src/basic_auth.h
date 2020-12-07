#pragma once

#include <functional>
#include <string>
#include <unordered_map>

#include "civetweb.h"
#include "prometheus/detail/pull_export.h"

namespace prometheus {

/**
 * Handler for HTTP Basic authentication for Endpoints.
 */
class BasicAuthHandler {
 public:
  using AuthFunc = std::function<bool(const std::string&, const std::string&)>;
  BasicAuthHandler(AuthFunc callback, std::string realm);

  /**
   * authHandler(struct mg_connection *, void *cbdata)
   *
   * Handles the authorization requests.
   *
   * @param conn - the connection information
   * @param cbdata - pointer to the CivetAuthHandler instance.
   * @returns 1 if authorized, 0 otherwise
   */
  static int authHandler(struct mg_connection* conn, void* cbdata);

 private:
  /**
   * Implements civetweb authorization interface.
   *
   * Attempts to extract a username and password from the Authorization header
   * to pass to the owning AuthHandler, `this->handler`.
   * If handler returns true, permits the request to proceed.
   * If handler returns false, or the Auth header is absent,
   * rejects the request with 401 Unauthorized.
   */
  bool Authorize(mg_connection* conn);

  bool AuthorizeInner(mg_connection* conn);
  void WriteUnauthorizedResponse(mg_connection* conn);

  AuthFunc callback_;
  std::string realm_;
};

}  // namespace prometheus
