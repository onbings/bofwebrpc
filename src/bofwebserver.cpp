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
  inline virtual void V_SetErrorHandler(httplib::Server::Handler _Handler) = 0;
  inline virtual void V_SetLogger(httplib::Logger _Logger) = 0;
  inline virtual bool V_Listen(const char *_pHost_c, uint16_t _Port_U16) = 0;
  inline virtual bool V_IsRunning() = 0;
  inline virtual bool V_Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler) = 0;
  inline virtual bool V_Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler) = 0;
  inline virtual bool V_Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler) = 0;

  inline virtual bool V_SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection = Headers());
  inline virtual bool V_RemoveMountPoint(const std::string &_rMountPoint_S);
  Server &set_file_extension_and_mimetype_mapping(const std::string &ext, const std::string &mime);
  Server &set_default_file_mimetype(const std::string &mime);
  Server &set_file_request_handler(Handler handler);

  template <class ErrorHandlerFunc> Server &set_error_handler(ErrorHandlerFunc &&handler)
  {
    return set_error_handler_core(std::forward<ErrorHandlerFunc>(handler), std::is_convertible<ErrorHandlerFunc, HandlerWithResponse>{});
  }

  Server &set_exception_handler(ExceptionHandler handler);
  Server &set_pre_routing_handler(HandlerWithResponse handler);
  Server &set_post_routing_handler(Handler handler);

  Server &set_expect_100_continue_handler(Expect100ContinueHandler handler);
  Server &set_logger(Logger logger);

  Server &set_address_family(int family);
  Server &set_tcp_nodelay(bool on);
  Server &set_socket_options(SocketOptions socket_options);

  Server &set_default_headers(Headers headers);
  Server &set_header_writer(std::function<ssize_t(Stream &, Headers &)> const &writer);

  Server &set_keep_alive_max_count(size_t count);
  Server &set_keep_alive_timeout(time_t sec);

  Server &set_read_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period> Server &set_read_timeout(const std::chrono::duration<Rep, Period> &duration);

  Server &set_write_timeout(time_t sec, time_t usec = 0);
  template <class Rep, class Period> Server &set_write_timeout(const std::chrono::duration<Rep, Period> &duration);

  Server &set_idle_interval(time_t sec, time_t usec = 0);
  template <class Rep, class Period> Server &set_idle_interval(const std::chrono::duration<Rep, Period> &duration);

  Server &set_payload_max_length(size_t length);

  bool bind_to_port(const std::string &host, int port, int socket_flags = 0);
  int bind_to_any_port(const std::string &host, int socket_flags = 0);
  bool listen_after_bind();

  bool listen(const std::string &host, int port, int socket_flags = 0);

  bool is_running() const;
  void wait_until_ready() const;
  void stop();
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

  inline void V_SetErrorHandler(httplib::Server::Handler _Handler) override
  {
    mpuSvr->set_error_handler(_Handler);
  }
  inline void V_SetLogger(httplib::Logger _Logger) override
  {
    mpuSvr->set_logger(_Logger);
  }
  inline bool V_Listen(const char *_pHost_c, uint16_t _Port_U16) override
  {
    return mpuSvr->listen(_pHost_c, _Port_U16);
  }
  inline bool V_IsRunning() override
  {
    return mpuSvr->is_running();
  }
  bool V_Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Get(_rPattern_S, _Handler);
    return Rts_B;
  }
  bool V_Post(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Post(_rPattern_S, _Handler);
    return Rts_B;
  }
  bool V_Put(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Put(_rPattern_S, _Handler);
    return Rts_B;
  }
  bool V_Patch(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Patch(_rPattern_S, _Handler);
    return Rts_B;
  }
  bool V_Delete(const std::string &_rPattern_S, httplib::Server::HandlerWithContentReader _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Delete(_rPattern_S, _Handler);
    return Rts_B;
  }
  bool V_Options(const std::string &_rPattern_S, httplib::Server::Handler _Handler)
  {
    bool Rts_B = true;
    mpuSvr->Options(_rPattern_S, _Handler);
    return Rts_B;
  }

private:
  // bool mUseHttps_B = false;
  std::unique_ptr<T> mpuSvr; // HTTP httplib::Serve or HTTPS httplib::SSLServer
  //  std::unique_ptr<httplib::Server> mpuHttpSvr;     // HTTP
  //  std::unique_ptr<httplib::SSLServer> mpuHttpsSvr; // HTTPS
};

BofWebServer::BofWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
    : BofWebApp(_psLoggerFactory, true, _rWebServerParam_X.WebAppParam_X)
{
  bool CreateHttpsServer_B;

  mWebServerParam_X = _rWebServerParam_X;
  mStopServerThread.store(false);
  CreateHttpsServer_B = _rWebServerParam_X.CertificatePath_S.empty() ? false : true;
  if (CreateHttpsServer_B)
  {
    CreateHttpsServer_B = _rWebServerParam_X.PrivateKeyPath_S.empty() ? true : false;
  }
  if (CreateHttpsServer_B)
  {
    mpuWebServerProxy = std::make_unique<BofWebServerProxy<httplib::SSLServer>>(_rWebServerParam_X.CertificatePath_S, _rWebServerParam_X.PrivateKeyPath_S);
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
bool BofWebServer::Start()
{
  bool Rts_B = false;
  uint32_t Start_U32, Delta_U32;
  WEB_APP_HOST Host_X(mWebAppConfig.at("httpServer"));

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Starting Server on %s:%d\n", Host_X.IpAddress_S.c_str(),
           Host_X.Port_U16);
  for (auto &rBannedIp : Host_X.BannedIpAddressCollection)
  {
    LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  Banned IP: %s\n", rBannedIp.c_str());
  }

  mpuWebServerProxy->V_SetErrorHandler([](const httplib::Request &_rReq, httplib::Response &_rRes) {
    const char *pFmt_c = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char pBuffer_c[0x4000];
    int Len_i = snprintf(pBuffer_c, sizeof(pBuffer_c), pFmt_c, _rRes.status);
    _rRes.set_content(pBuffer_c, Len_i, std::string("text/html"));
  });

  mpuWebServerProxy->V_SetLogger([this](const httplib::Request &_rReq, const httplib::Response &_rRes) {
    LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "%s", LogRequest(_rReq, _rRes).c_str());
  });

  mStopServerThread.store(false);
  if (mServerThread.joinable())
  {
    Stop();
  }
  mServerThread = std::thread([this, Host_X]() { this->mpuWebServerProxy->V_Listen(Host_X.IpAddress_S.c_str(), Host_X.Port_U16); });
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
  // Try to stop with timeout like Start
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
  return Rts_B;
}
bool BofWebServer::Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler)
{
  return mpuWebServerProxy->V_Get(_rPattern_S, _Handler);
}
#if 0
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
bool BofWebServer::Delete(const std::string &_rPattern_S, httplib::Server::HaHandlerWithContentReaderndler _Handler)
{
  return mpuWebServerProxy->V_Delete(_rPattern_S, _Handler);
}
bool BofWebServer::Options(const std::string &_rPattern_S, Handler _Handler)
{
  return mpuWebServerProxy->V_Options(_rPattern_S, _Handler);
}
#endif

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