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
  bool Start(const std::string &_rIpAddress_S, uint16_t _Port_U16);
  bool Stop();

  bool Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler);
  bool Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler);
  bool Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler);
  bool SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection = httplib::Headers());
  bool RemoveMountPoint(const std::string &_rMountPoint_S);
  bool SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S);
  bool SetDefaultFileMimetype(const std::string &_rMime_S);
  bool SetFileRequestHandler(httplib::Server::Handler _Handler);
  bool SetErrorHandler(httplib::Server::Handler _Handler);
  bool SetExceptionHandler(httplib::Server::ExceptionHandler _Handler);
  bool SetPreRoutingHandler(httplib::Server::HandlerWithResponse _Handler);
  bool SetPostRoutingHandler(httplib::Server::Handler _Handler);
  bool SetExpect100ContinueHandler(httplib::Server::Expect100ContinueHandler _Handler);
  bool SetLogger(httplib::Logger _Logger);
  bool SetAddressFamily(int _Family_i);
  bool SetTcpNoDelay(bool _On_B);
  bool SetSocketOptions(httplib::SocketOptions _SocketOptions);
  bool SetDefaultHeaders(httplib::Headers _Headers);
  bool SetHeaderWriter(std::function<ssize_t(httplib::Stream &, httplib::Headers &)> const &_rWriter);
  bool SetKeepAliveMaxCount(size_t _Count);
  bool SetKeepAliveTimeout(uint32_t _TimeInMs_U32);
  bool SetReadTimeout(uint32_t _TimeInMs_U32);
  bool SetWriteTimeout(uint32_t _TimeInMs_U32);
  bool SetIdleInterval(uint32_t _TimeInMs_U32);
  bool SetPayloadMaxLength(size_t _Length);
  bool IsRunning() const;

private:
  std::unique_ptr<IBofWebServerProxy> mpuWebServerProxy;
  std::atomic<bool> mStopServerThread;
  std::thread mServerThread;
  BOF_WEB_SERVER_PARAM mWebServerParam_X;
  BOF_WEB_APP_HOST mHost_X;
};
END_WEBRPC_NAMESPACE()
