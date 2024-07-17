/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
#include "bofwebrpc/bofwebserver.h"
#include <bofstd/bofsystem.h>

BEGIN_WEBRPC_NAMESPACE()
class IBofWebServerProxy
{
public:
  virtual ~IBofWebServerProxy() = default;
  inline virtual bool V_Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler) = 0;
  inline virtual bool V_Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler) = 0;
  inline virtual bool V_SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S,
                                      httplib::Headers _rHeaderCollection = httplib::Headers()) = 0;
  inline virtual bool V_RemoveMountPoint(const std::string &_rMountPoint_S) = 0;
  inline virtual bool V_SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S) = 0;
  inline virtual bool V_SetDefaultFileMimetype(const std::string &_rMime_S) = 0;
  inline virtual bool V_SetFileRequestHandler(httplib::Server::Handler _Handler) = 0;
  inline virtual bool V_SetErrorHandler(httplib::Server::Handler _Handler) = 0;
  inline virtual bool V_SetExceptionHandler(httplib::Server::ExceptionHandler _Handler) = 0;
  inline virtual bool V_SetPreRoutingHandler(httplib::Server::HandlerWithResponse _Handler) = 0;
  inline virtual bool V_SetPostRoutingHandler(httplib::Server::Handler _Handler) = 0;

  inline virtual bool V_SetExpect100ContinueHandler(httplib::Server::Expect100ContinueHandler _Handler) = 0;
  inline virtual bool V_SetLogger(httplib::Logger _Logger) = 0;

  inline virtual bool V_SetAddressFamily(int _Family_i) = 0;
  inline virtual bool V_SetTcpNoDelay(bool _On_B) = 0;
  inline virtual bool V_SetSocketOptions(httplib::SocketOptions _SocketOptions) = 0;

  inline virtual bool V_SetDefaultHeaders(httplib::Headers _Headers) = 0;
  inline virtual bool V_SetHeaderWriter(std::function<ssize_t(httplib::Stream &, httplib::Headers &)> const &_rWriter) = 0;

  inline virtual bool V_SetKeepAliveMaxCount(size_t _Count) = 0;
  inline virtual bool V_SetKeepAliveTimeout(uint32_t _TimeInMs_U32) = 0;
  inline virtual bool V_SetReadTimeout(uint32_t _TimeInMs_U32) = 0;
  inline virtual bool V_SetWriteTimeout(uint32_t _TimeInMs_U32) = 0;
  inline virtual bool V_SetIdleInterval(uint32_t _TimeInMs_U32) = 0;
  inline virtual bool V_SetPayloadMaxLength(size_t _Length) = 0;

  inline virtual bool V_Start(const std::string &_rHost, uint16_t _Port_U16) = 0;
  inline virtual bool V_IsRunning() const = 0;
  inline virtual bool V_Stop() = 0;
};
// HTTP httplib::Serve or HTTPS httplib::SSLServer
template <typename T> class BofWebServerProxy : public IBofWebServerProxy
{
public:
  // Constructor for HTTP server
  BofWebServerProxy()
  {
    if constexpr (std::is_same<T, httplib::Server>::value)
    {
      mpuSvr = std::make_unique<T>();
    }
  }

  // Constructor for HTTPS server
  BofWebServerProxy(const std::string &_rCertificatePath_S, const std::string &_rPrivateKeyPath_S)
  {
    if constexpr (std::is_same<T, httplib::SSLServer>::value)
    {
      mpuSvr = std::make_unique<T>(_rCertificatePath_S.c_str(), _rPrivateKeyPath_S.c_str());
    }
  }

  inline bool V_Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Get(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Post(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Put(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Patch(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Delete(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->Options(_rPattern_S, _Handler);
    return Rts_B;
  }
  inline bool V_SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection) override
  {
    return mpuSvr->set_mount_point(_rMountPoint_S, _rDir_S, _rHeaderCollection);
  }
  inline bool V_RemoveMountPoint(const std::string &_rMountPoint_S) override
  {
    return mpuSvr->remove_mount_point(_rMountPoint_S);
  }
  inline bool V_SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S) override
  {
    bool Rts_B = true;
    mpuSvr->set_file_extension_and_mimetype_mapping(_rExt_S, _rMime_S);
    return Rts_B;
  }
  inline bool V_SetDefaultFileMimetype(const std::string &_rMime_S) override
  {
    bool Rts_B = true;
    mpuSvr->set_default_file_mimetype(_rMime_S);
    return Rts_B;
  }
  inline bool V_SetFileRequestHandler(httplib::Server::Handler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_file_request_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetErrorHandler(httplib::Server::Handler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_error_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetExceptionHandler(httplib::Server::ExceptionHandler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_exception_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetPreRoutingHandler(httplib::Server::HandlerWithResponse _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_pre_routing_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetPostRoutingHandler(httplib::Server::Handler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_post_routing_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetExpect100ContinueHandler(httplib::Server::Expect100ContinueHandler _Handler) override
  {
    bool Rts_B = true;
    mpuSvr->set_expect_100_continue_handler(_Handler);
    return Rts_B;
  }
  inline bool V_SetLogger(httplib::Logger _Logger) override
  {
    bool Rts_B = true;
    mpuSvr->set_logger(_Logger);
    return Rts_B;
  }
  inline bool V_SetAddressFamily(int _Family_i) override
  {
    bool Rts_B = true;
    mpuSvr->set_address_family(_Family_i);
    return Rts_B;
  }
  inline bool V_SetTcpNoDelay(bool _On_B) override
  {
    bool Rts_B = true;
    mpuSvr->set_tcp_nodelay(_On_B);
    return Rts_B;
  }
  inline bool V_SetSocketOptions(httplib::SocketOptions _SocketOptions)
  {
    bool Rts_B = true;
    mpuSvr->set_socket_options(_SocketOptions);
    return Rts_B;
  }
  inline bool V_SetDefaultHeaders(httplib::Headers _Headers) override
  {
    bool Rts_B = true;
    mpuSvr->set_default_headers(_Headers);
    return Rts_B;
  }
  inline bool V_SetHeaderWriter(std::function<ssize_t(httplib::Stream &, httplib::Headers &)> const &_rWriter) override
  {
    bool Rts_B = true;
    mpuSvr->set_header_writer(_rWriter);
    return Rts_B;
  }
  inline bool V_SetKeepAliveMaxCount(size_t _Count) override
  {
    bool Rts_B = true;
    mpuSvr->set_keep_alive_max_count(_Count);
    return Rts_B;
  }
  inline bool V_SetKeepAliveTimeout(uint32_t _TimeInMs_U32) override
  {
    bool Rts_B = true;
    mpuSvr->set_keep_alive_timeout(_TimeInMs_U32 / 1000);
    return Rts_B;
  }
  inline bool V_SetReadTimeout(uint32_t _TimeInMs_U32) override
  {
    bool Rts_B = true;
    mpuSvr->set_read_timeout(_TimeInMs_U32 / 1000, (_TimeInMs_U32 % 1000) * 1000);
    return Rts_B;
  }
  inline bool V_SetWriteTimeout(uint32_t _TimeInMs_U32) override
  {
    bool Rts_B = true;
    mpuSvr->set_write_timeout(_TimeInMs_U32 / 1000, (_TimeInMs_U32 % 1000) * 1000);
    return Rts_B;
  }
  inline bool V_SetIdleInterval(uint32_t _TimeInMs_U32) override
  {
    bool Rts_B = true;
    mpuSvr->set_idle_interval(_TimeInMs_U32 / 1000, (_TimeInMs_U32 % 1000) * 1000);
    return Rts_B;
  }
  inline bool V_SetPayloadMaxLength(size_t _Length) override
  {
    bool Rts_B = true;
    mpuSvr->set_payload_max_length(_Length);
    return Rts_B;
  }
  inline bool V_Start(const std::string &_rIpAddress_S, uint16_t _Port_U16)
  {
    bool Rts_B = true;
    mpuSvr->listen(_rIpAddress_S, _Port_U16, 0);
    return Rts_B;
  }
  inline bool V_IsRunning() const
  {
    return mpuSvr->is_running();
  }
  inline bool V_WaitUntilReady() const
  {
    bool Rts_B = true;
    mpuSvr->wait_until_ready();
    return Rts_B;
  }
  inline bool V_Stop()
  {
    bool Rts_B = true;
    mpuSvr->stop();
    return Rts_B;
  }

private:
  // bool mUseHttps_B = false;
  std::unique_ptr<T> mpuSvr; // HTTP httplib::Server or HTTPS httplib::SSLServer
  //  std::unique_ptr<httplib::Server> mpuHttpSvr;     // HTTP
  //  std::unique_ptr<httplib::SSLServer> mpuHttpsSvr; // HTTPS
};

BofWebServer::BofWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
    : BofWebApp(_psLoggerFactory, true, _rWebServerParam_X.WebAppParam_X)
{
  bool CreateHttpsServer_B;

  mWebServerParam_X = _rWebServerParam_X;
  mStopServerThread.store(false);
  CreateHttpsServer_B = mWebServerParam_X.CertificatePath_S.empty() ? false : true;
  if (CreateHttpsServer_B)
  {
    CreateHttpsServer_B = mWebServerParam_X.PrivateKeyPath_S.empty() ? false : true;
  }
  if (CreateHttpsServer_B)
  {
    mpuWebServerProxy = std::make_unique<BofWebServerProxy<httplib::SSLServer>>(mWebServerParam_X.CertificatePath_S, mWebServerParam_X.PrivateKeyPath_S);
  }
  else
  {
    mpuWebServerProxy = std::make_unique<BofWebServerProxy<httplib::Server>>();
  }
}
BofWebServer ::~BofWebServer()
{
  Stop();
}
bool BofWebServer::Start(const std::string &_rIpAddress_S, uint16_t _Port_U16)
{
  bool Rts_B = false;
  uint32_t Start_U32, Delta_U32;
  // BOF_WEB_APP_HOST mHost_X;
  if (_rIpAddress_S == "")
  {
    mHost_X = BOF_WEB_APP_HOST(mWebAppConfig.at("httpServer"));
  }
  else
  {
    mHost_X.IpAddress_S = _rIpAddress_S;
    mHost_X.Port_U16 = _Port_U16;
  }

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Starting Server on %s:%d\n", mHost_X.IpAddress_S.c_str(),
           mHost_X.Port_U16);

  for (auto &rBannedIp : mHost_X.BannedIpAddressCollection)
  {
    LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  Banned IP: %s\n", rBannedIp.c_str());
  }

  mStopServerThread.store(false);
  if (mServerThread.joinable())
  {
    Stop();
  }
  mServerThread = std::thread([this]() { this->mpuWebServerProxy->V_Start(mHost_X.IpAddress_S.c_str(), mHost_X.Port_U16); });
  Start_U32 = BOF::Bof_GetMsTickCount();
  do
  {
    if (mpuWebServerProxy->V_IsRunning())
    {
      break;
    }
    else
    {
      BOF::Bof_MsSleep(20);
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
  } while (Delta_U32 < mWebServerParam_X.ServerStartStopTimeoutInMs_U32);
  Rts_B = mpuWebServerProxy->V_IsRunning();
  return Rts_B;
}

bool BofWebServer::Stop()
{
  bool Rts_B = false;
  uint32_t Start_U32, Delta_U32;
  if (mpuWebServerProxy->V_Stop())
  {
    Start_U32 = BOF::Bof_GetMsTickCount();
    do
    {
      if (!mpuWebServerProxy->V_IsRunning())
      {
        break;
      }
      else
      {
        BOF::Bof_MsSleep(20);
      }
      Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
    } while (Delta_U32 < mWebServerParam_X.ServerStartStopTimeoutInMs_U32);
    Rts_B = !mpuWebServerProxy->V_IsRunning();
    mStopServerThread.store(true);
    if (mServerThread.joinable())
    {
      mServerThread.join();
    }
  }
  return Rts_B;
}
bool BofWebServer::Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_Get(_rPattern_S, _Handler);
}
bool BofWebServer::Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
{
  return mpuWebServerProxy->V_Post(_rPattern_S, _Handler);
}
bool BofWebServer::Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
{
  return mpuWebServerProxy->V_Put(_rPattern_S, _Handler);
}
bool BofWebServer::Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
{
  return mpuWebServerProxy->V_Patch(_rPattern_S, _Handler);
}
bool BofWebServer::Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
{
  return mpuWebServerProxy->V_Delete(_rPattern_S, _Handler);
}
bool BofWebServer::Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_Options(_rPattern_S, _Handler);
}
bool BofWebServer::SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection)
{
  return mpuWebServerProxy->V_SetMountPoint(_rMountPoint_S, _rDir_S, _rHeaderCollection);
}
bool BofWebServer::RemoveMountPoint(const std::string &_rMountPoint_S)
{
  return mpuWebServerProxy->V_RemoveMountPoint(_rMountPoint_S);
}
bool BofWebServer::SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S)
{
  return mpuWebServerProxy->V_SetFileExtensionAndMimetypeMapping(_rExt_S, _rMime_S);
}

bool BofWebServer::SetDefaultFileMimetype(const std::string &_rMime_S)
{
  return mpuWebServerProxy->V_SetDefaultFileMimetype(_rMime_S);
}
bool BofWebServer::SetFileRequestHandler(httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_SetFileRequestHandler(_Handler);
}
bool BofWebServer::SetErrorHandler(httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_SetErrorHandler(_Handler);
}
bool BofWebServer::SetExceptionHandler(httplib::Server::ExceptionHandler _Handler)
{
  return mpuWebServerProxy->V_SetExceptionHandler(_Handler);
}
bool BofWebServer::SetPreRoutingHandler(httplib::Server::HandlerWithResponse _Handler)
{
  return mpuWebServerProxy->V_SetPreRoutingHandler(_Handler);
}
bool BofWebServer::SetPostRoutingHandler(httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_SetPostRoutingHandler(_Handler);
}
bool BofWebServer::SetExpect100ContinueHandler(httplib::Server::Expect100ContinueHandler _Handler)
{
  return mpuWebServerProxy->V_SetExpect100ContinueHandler(_Handler);
}
bool BofWebServer::SetLogger(httplib::Logger _Logger)
{
  return mpuWebServerProxy->V_SetLogger(_Logger);
}
bool BofWebServer::SetAddressFamily(int _Family_i)
{
  return mpuWebServerProxy->V_SetAddressFamily(_Family_i);
}
bool BofWebServer::SetTcpNoDelay(bool _On_B)
{
  return mpuWebServerProxy->V_SetTcpNoDelay(_On_B);
}
bool BofWebServer::SetSocketOptions(httplib::SocketOptions _SocketOptions)
{
  return mpuWebServerProxy->V_SetSocketOptions(_SocketOptions);
}
bool BofWebServer::SetDefaultHeaders(httplib::Headers _Headers)
{
  return mpuWebServerProxy->V_SetDefaultHeaders(_Headers);
}

bool BofWebServer::SetHeaderWriter(std::function<ssize_t(httplib::Stream &, httplib::Headers &)> const &_rWriter)
{
  return mpuWebServerProxy->V_SetHeaderWriter(_rWriter);
}
bool BofWebServer::SetKeepAliveMaxCount(size_t _Count)
{
  return mpuWebServerProxy->V_SetKeepAliveMaxCount(_Count);
}
bool BofWebServer::SetKeepAliveTimeout(uint32_t _TimeInMs_U32)
{
  return mpuWebServerProxy->V_SetKeepAliveTimeout(_TimeInMs_U32);
}
bool BofWebServer::SetReadTimeout(uint32_t _TimeInMs_U32)
{
  return mpuWebServerProxy->V_SetReadTimeout(_TimeInMs_U32);
}

bool BofWebServer::SetWriteTimeout(uint32_t _TimeInMs_U32)
{
  return mpuWebServerProxy->V_SetWriteTimeout(_TimeInMs_U32);
}
bool BofWebServer::SetIdleInterval(uint32_t _TimeInMs_U32)
{
  return mpuWebServerProxy->V_SetIdleInterval(_TimeInMs_U32);
}

bool BofWebServer::SetPayloadMaxLength(size_t _Length)
{
  return mpuWebServerProxy->V_SetPayloadMaxLength(_Length);
}
bool BofWebServer::IsRunning() const
{
  return mpuWebServerProxy->V_IsRunning();
}

// github.com/yhirose/cpp-httplib/blob/master/example/server.cc
// svr.Get("/", [=](const Request & /*req*/, Response &res) { res.set_redirect("/hi"); });
// f (!svr.is_valid())
//
// printf("server has an error...\n");
// return -1;
//
//
/// svr.Get("/hi", [](const Request & /*req*/, Response &res) { res.set_content("Hello World!\n", "text/plain"); });
END_WEBRPC_NAMESPACE()