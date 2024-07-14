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
#include <atomic>
#include "bofwebrpc/bofwebapp.h"

BEGIN_WEBRPC_NAMESPACE()
class IBofWebServerProxy;

struct BOF_WEB_SERVER_PARAM
{
  uint32_t ServerStartStopTimeoutInMs_U32;
  std::string CertificatePath_S; // If empty create an HTTP server instead of an HTTPS
  std::string PrivateKeyPath_S;
  // Also possible, const char *client_ca_cert_file_path = nullptr, const char *client_ca_cert_dir_path = nullptr, const char *private_key_password = nullptr
  BOF_WEB_APP_PARAM WebAppParam_X;

  BOF_WEB_SERVER_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    ServerStartStopTimeoutInMs_U32 = 0;
    CertificatePath_S = "";
    PrivateKeyPath_S = "";
    WebAppParam_X.Reset();
  }
};

class BofWebServer : public BofWebApp
{
public:
  BofWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_SERVER_PARAM &_rWebServerParam_X);
  virtual ~BofWebServer();
  bool Start();
  bool Stop();
  bool Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler);
  bool Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler);

private:
  std::unique_ptr<IBofWebServerProxy> mpuWebServerProxy;
  std::atomic<bool> mStopServerThread;
  std::thread mServerThread;
  BOF_WEB_SERVER_PARAM mWebServerParam_X;
};
END_WEBRPC_NAMESPACE()
