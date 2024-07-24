/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
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
using BOF_WEB_HEADER = httplib::Headers;
using BOF_WEB_REQUEST = httplib::Request;
using BOF_WEB_RESPONSE = httplib::Response;
using BOF_WEB_HANDLER_RESPONSE = httplib::Server::HandlerResponse;
using BOF_WEB_RESULT = httplib::Result;
using BOF_WEB_HANDLER = httplib::Server::Handler;
using BOF_HTTP_SERVER = httplib::Server;
using BOF_HTTPS_SERVER = httplib::SSLServer;
using BOF_HTTP_CLIENT = httplib::Client;
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
  bool Start(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  bool Stop();
  std::string LogRequestAndResponse(const httplib::Request &_rReq, const httplib::Response &_rRes);
  std::string GenerateSessionId(uint32_t _SessionIdLen_U32);

  BOF_WEB_APP_PARAM mWebAppParam_X;
  BOF_WEB_JSON mWebAppConfig;
  static std::array<std::shared_ptr<BOF::IBofLogger>, WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_MAX> S_mpsWebAppLoggerCollection;

private:
  BOF_WEB_JSON ReadConfig();
  void ConfigureLogger(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  std::string DumpHeader(const BOF_WEB_HEADER &_rHeader);

  static uint32_t S_mInstanceId_U32;
  bool mServer_B = false;
};

END_WEBRPC_NAMESPACE()
