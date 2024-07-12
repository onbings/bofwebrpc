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
#include "bofwebrpc/bofwebapp.h"

namespace httplib
{
class Server;
class SSLServer;
} // namespace httplib

BEGIN_WEBRPC_NAMESPACE()

struct BOF_WEB_SERVER_PARAM
{
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

private:
  bool mUseHttps_B = false;
  std::unique_ptr<httplib::Server> mpuHttpSvr;     // HTTP
  std::unique_ptr<httplib::SSLServer> mpuHttpsSvr; // HTTPS
};
END_WEBRPC_NAMESPACE()
