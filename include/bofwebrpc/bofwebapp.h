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
using json = nlohmann::json;

#include <bofstd/bofbasicloggerfactory.h>
#include "bofwebrpc/bofwebrpc.h"
// #define CPPHTTPLIB_OPENSSL_SUPPORT: done by vcpkg
#include "httplib.h"

#if 0
// https://stackoverflow.com/questions/63584828/making-https-request-using-cpp-httplib
#include "stdafx.h"
>>> #define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <Windows.h>
#include <iostream>

#define CA_CERT_FILE "./ca-bundle.crt"
using namespace std;


int main()
{

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        httplib::SSLClient cli("localhost", 8000);
        //httplib::SSLClient cli("google.com");
        // httplib::SSLClient cli("www.youtube.com");
        cli.set_ca_cert_path(CA_CERT_FILE);
        cli.enable_server_certificate_verification(true);
#else
        httplib::Client cli("localhost", 8000);
#endif
        char* x = { "hello world" };
        httplib::Params params{
            { "key", x }
        };

        auto res = cli.Post("/postReq/", params);
        if (res) {
            cout << res->status << endl;
            cout << res->get_header_value("Content-Type") << endl;
            cout << res->body << endl;
        }
        else {
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            auto result = cli.get_openssl_verify_result();
            if (result) {
                cout << "error";
                cout << "verify error: " << X509_verify_cert_error_string(result)    << endl;
            }
#endif
        }
    system("pause");
    return 0;
}
#endif

BEGIN_WEBRPC_NAMESPACE()
// Logger Channel Definition
enum WEB_APP_LOGGER_CHANNEL : uint32_t
{
  WEB_APP_LOGGER_CHANNEL_APP = 0,
  WEB_APP_LOGGER_CHANNEL_REST,
  WEB_APP_LOGGER_CHANNEL_MAX
};
struct BOF_WEB_APP_PARAM
{
  std::string AppName_S;
  uint32_t ConfigThreadPollTimeInMs_U32;

  BOF_WEB_APP_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    AppName_S = "";
    ConfigThreadPollTimeInMs_U32 = 0; // 0 to disable config thread
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
  BOF_WEB_APP_HOST(const json &_rConfig)
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
  virtual void V_OnConfigUpdate(const json &_rConfig);
  bool Initialize(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  bool Shutdown();
  std::string LogRequest(const httplib::Request &_rReq, const httplib::Response &_rRes);
  std::string GenerateSessionId(uint32_t _SessionIdLen_U32);

  BOF_WEB_APP_PARAM mWebAppParam_X;
  json mWebAppConfig;
  static std::array<std::shared_ptr<BOF::IBofLogger>, WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_MAX> S_mpsWebAppLoggerCollection;

private:
  json ReadConfig(bool _LogIt_B);
  void ConfigureLogger(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory);
  std::string DumpHeader(const httplib::Headers &_rHeader);

  static uint32_t S_mInstanceId_U32;
  std::thread mCheckConfigThread;
  bool mServer_B = false;
};

END_WEBRPC_NAMESPACE()
