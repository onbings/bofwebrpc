/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
/*
The CRUD model stands for Create, Read, Update, and Delete, which are the four basic operations that can be performed on data in a database or a similar storage
system. Here's how the commonly used HTTP methods (or verbs) relate to the CRUD operations:

POST: Often used for Create in CRUD. It sends data to the server to create a new resource. The data is included in the body of the request. It can also be used
for operations that don't fit into the CRUD model, like triggering actions on the server.
Ex: Used to create a new user. A POST request to /api/users with user data in the request body would add a new user to the database. The server decides the new
user's ID and returns it as part of the response.

GET: Corresponds to Read in CRUD. It is used to retrieve data from a server. A GET request should only fetch data and not change any state on the server.
Ex: Used to retrieve information about a user or a list of users. For example, sending a GET request to /api/users retrieves a list of all users, while
/api/users/1 fetches details of the user with ID 1.

PUT: Maps to Update in CRUD. It replaces all current representations of the target resource with the
uploaded content. PUT requests are idempotent, meaning that multiple identical requests should have the same effect as a single request.
Ex: Updates an existing user's information completely. A PUT request to /api/users/1 with the user's updated data in the request body would replace the existing
user's data with ID 1 in the database. If the user does not exist, the server might create a new user with ID 1, depending on the API's design.

PATCH: Also relates to Update in CRUD but is used for making partial updates to an existing resource, whereas PUT replaces the entire resource.
Ex: Applies partial modifications to a user. Unlike PUT, a PATCH request to /api/users/1 with partial user data would update only the specified fields for the
user with ID 1 without replacing the entire user data.

DELETE: Directly corresponds to Delete in CRUD. It is used to remove resources from the server. Ex:
Removes a user from the database. A DELETE request to /api/users/1 would delete the user with ID 1. If successful, the server might return a success status
code, indicating the user was deleted.
Ex: Removes a user from the database. A DELETE request to /api/users/1 would delete the user with ID 1. If successful, the server might return a success status
code, indicating the user was deleted.

The HEAD and OPTIONS methods do not directly correspond to any CRUD operations. HEAD can be seen as a part of Read since it asks for a response identical to a
GET request but without the response body. OPTIONS is used to describe the communication options for the target resource and doesn't directly map to any CRUD
operation.

- HEAD
Ex: Similar to GET, but it retrieves only the header information and no body. For instance, a HEAD request to /api/users/1 could be used to check if the user
exists or to get the last modified date without downloading the user data.

- OPTIONS Ex: Used to describe the communication options available for the API or a
specific resource. An OPTIONS request to /api/users or /api/users/1 would return the HTTP methods that the server supports for these endpoints. This is useful
for understanding capabilities or for cross-origin resource sharing (CORS) in web applications.

In REST APIs, the Uniform Resource Identifier (URI) is used to identify resources. The format of a URI in REST APIs typically follows a structured approach to
ensure resources are easily identifiable and manipulable. Here's a breakdown of the URI format used in REST APIs, including how query parameters are utilized:

- Base URI
The base URI represents the root address of the API. It includes the protocol (usually HTTP or HTTPS), the domain, and may include a port and a path to the API
root if necessary.
Ex: https://api.example.com/v1

- Resource Path
Following the base URI, the resource path specifies the path to a specific resource or a collection of resources. It's hierarchical, using slashes to denote
levels.
Ex: /users for accessing all users or /users/1 for accessing a user with ID 1.

- Query Parameters
Query parameters are used to further refine the request, often for filtering, searching, sorting, or specifying which parts of the resource to return. They are
appended to the URI after a question mark (?), with key-value pairs separated by an equals sign (=) and multiple parameters separated by an ampersand (&).
Ex: /users?age=25 to filter users by age, or /users?sort=name&limit=10 to sort users by name and limit the result to 10 users.

- Full URI Example
Combining the above elements, a full URI in a REST API might look like this:
https://api.example.com/v1/users?age=25&sort=name&limit=10
This URI would be used to request a list of users from version 1 of the API at api.example.com, filtering the users to those who are 25 years old, sorting the
list by name, and limiting the results to 10 users.

Best Practices for URI Design in REST APIs
- Use nouns for resources: The path should represent the resource (e.g., /users, /orders) rather than the action being performed on it.
- Use hierarchical structures: Represent relationships in the path (e.g., /users/1/orders for orders belonging to user 1).
Keep it lowercase: Paths are generally kept lowercase with hyphens (-) to separate words.
- Use query parameters for filtering: Use query parameters for operations that don't involve directly manipulating resources, like filtering or sorting.
By following these conventions, REST APIs achieve a level of standardization that makes them more intuitive and easier to use for developers.
*/

#pragma once
#include <thread>
#include <unordered_set>
#include <nlohmann/json.hpp>
using BOF_WEB_JSON = nlohmann::json;

#include <bofstd/bofbasicloggerfactory.h>
#include "bofwebrpc/bofwebrpc.h"

#if !defined(CPPHTTPLIB_OPENSSL_SUPPORT)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#if !defined(CPPHTTPLIB_ZLIB_SUPPORT)
#define CPPHTTPLIB_ZLIB_SUPPORT
#endif
#if !defined(CPPHTTPLIB_BROTLI_SUPPORT)
#define CPPHTTPLIB_BROTLI_SUPPORT
#endif
#include <httplib.h>

BEGIN_WEBRPC_NAMESPACE()
#define BOF_WEB_LIB httplib
using BOF_WEB_HEADER = httplib::Headers;
using BOF_WEB_REQUEST = httplib::Request;
using BOF_WEB_RESPONSE = httplib::Response;
using BOF_WEB_HANDLER_RESPONSE = httplib::Server::HandlerResponse;
using BOF_WEB_RESULT = httplib::Result;
using BOF_WEB_HANDLER = httplib::Server::Handler;
using BOF_WEB_HTTP_SERVER = httplib::Server;
using BOF_WEB_HTTPS_SERVER = httplib::SSLServer;
using BOF_WEB_HTTP_CLIENT = httplib::Client;
using BOF_WEB_STATUS = httplib::StatusCode;
using BOF_WEB_SOCKET = socket_t;
// Logger Channel Definition
enum WEB_APP_LOGGER_CHANNEL : uint32_t
{
  WEB_APP_LOGGER_CHANNEL_APP = 0,
  WEB_APP_LOGGER_CHANNEL_MAX
};
struct BOF_WEB_APP_PARAM
{
  std::string AppName_S;

  BOF_WEB_APP_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    AppName_S = "";
  }
};
struct BOF_WEB_APP_HOST
{
  std::string IpAddress_S{};
  uint16_t Port_U16;
  std::unordered_set<std::string> BannedIpAddressCollection;

  BOF_WEB_APP_HOST()
  {
    Reset();
  }
  BOF_WEB_APP_HOST(const BOF_WEB_JSON &_rConfig)
  {
    IpAddress_S = _rConfig.at("ip");

    Port_U16 = _rConfig.at("port");
    if (_rConfig.contains("bannedIps"))
    {
      BannedIpAddressCollection = _rConfig.value("bannedIps", std::unordered_set<std::string>{});
    }
  }
  void Reset()
  {
    IpAddress_S = "";
    Port_U16 = 0;
    BannedIpAddressCollection.clear();
  }
  bool IsBanned(const std::string &_rIpAddress_S) const
  {
    return BannedIpAddressCollection.find(_rIpAddress_S) != BannedIpAddressCollection.end();
  }
};
class BofWebApp
{
public:
  BofWebApp(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, bool _Server_B, const BOF_WEB_APP_PARAM &_rWebAppParam_X);
  virtual ~BofWebApp();

  BofWebApp(const BofWebApp &) = delete;
  BofWebApp &operator=(const BofWebApp &) = delete;
  BofWebApp(BofWebApp &&) = delete;
  BofWebApp &operator=(BofWebApp &&) = delete;

protected:
  bool Initialize(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  bool Shutdown();
  std::string LogRequestAndResponse(const httplib::Request &_rReq, const httplib::Response &_rRes);
  std::string GenerateSessionId(uint32_t _SessionIdLen_U32);

  BOF_WEB_APP_PARAM mWebAppParam_X;
  BOF_WEB_JSON mWebAppConfig;
  static std::array<std::shared_ptr<BOF::IBofLogger>, WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_MAX> S_mpsWebAppLoggerCollection;
  bool ParseContentRangeRequest(const std::string &_rContentRangeRequest_S, size_t &_rRangeMin, size_t &_rRangeMax, size_t &_rDataSize);

private:
  bool ReadConfig(BOF_WEB_JSON &_rConfig);
  void ConfigureLogger(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  std::string DumpHeader(const BOF_WEB_HEADER &_rHeader);

  static uint32_t S_mInstanceId_U32;
  bool mServer_B = false;
};

END_WEBRPC_NAMESPACE()
