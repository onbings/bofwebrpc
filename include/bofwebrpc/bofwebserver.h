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
struct BOF_WEB_SERVER_PARAM
{
  uint32_t ServerStartStopTimeoutInMs_U32;
  std::string CertificatePath_S; // If empty create an HTTP server instead of an HTTPS
  std::string PrivateKeyPath_S;

  bool LogRequestAndResponse_B;
  uint32_t KeepAliveMaxCount_U32;
  uint32_t KeepAliveTimeoutInMs_U32;
  uint32_t ReadTimeoutInMs_U32;
  uint32_t WriteTimeoutInMs_U32;
  uint32_t IdleIntervalInMs_U32;
  uint32_t PayloadMaxLengthInByte_U32;
  uint32_t ThreadPoolSize_U32;
  std::string RootDir_S;
  bool DisableCors_B;
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

    LogRequestAndResponse_B = false;
    KeepAliveMaxCount_U32 = 0;
    KeepAliveTimeoutInMs_U32 = 0;
    ReadTimeoutInMs_U32 = 0;
    WriteTimeoutInMs_U32 = 0;
    IdleIntervalInMs_U32 = 0;
    PayloadMaxLengthInByte_U32 = 0;
    ThreadPoolSize_U32 = 0;
    RootDir_S = "";
    DisableCors_B = false;
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

  bool Get(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool Post(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool Put(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool Patch(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool Delete(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool Options(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler);
  bool SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection = httplib::Headers());
  bool RemoveMountPoint(const std::string &_rMountPoint_S);
  bool SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S);
  bool IsRunning() const;
  /*
  GET on file which are on mounted directory:
  if (!head && file_request_handler_) {
    file_request_handler_(req, res);
  }
  */
  virtual void V_OnFileRequest(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
  {
  }
  /*
  For status code above and equal to 400
    if (400 <= res.status && error_handler_ &&
        error_handler_(req, res) == HandlerResponse::Handled) {
      need_apply_ranges = true;
    }
  */
  virtual void V_OnError(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
  {
  }
  /*
    try {
    routed = routing(req, res, strm);
  } catch (std::exception &e) {
    if (exception_handler_) {
  */
  virtual void V_OnException(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes, std::exception_ptr ep)
  {
    char pBuffer_c[0x100];
    try
    {
      std::rethrow_exception(ep);
    }
    catch (std::exception &rE)
    {
      snprintf(pBuffer_c, sizeof(pBuffer_c), "<h1>Error 500</h1><p>%s</p>", rE.what());
    }
    catch (...)
    { // See the following NOTE
      snprintf(pBuffer_c, sizeof(pBuffer_c), "<h1>Error 500</h1><p>%s</p>", "Unknown Exception");
    }
    _rRes.set_content(pBuffer_c, "text/html");
    _rRes.status = BOF_WEB_STATUS::InternalServerError_500;
  }
  virtual bool V_OnPreRouting(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
  {
    return false;
  }
  virtual void V_OnPostRouting(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
  {
  }
  // By default, the server sends a 100 Continue response for an Expect: 100-continue header.
  virtual BOF_WEB_STATUS V_OnExpect100Continue(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
  {
    return BOF_WEB_STATUS::Continue_100;
  }
  virtual void V_OnSetSocketOption(BOF_WEB_SOCKET _Socket)
  {
  }
  virtual void V_OnIdle()
  {
  }

protected:
  BOF_WEB_SERVER_PARAM mWebServerParam_X;
  BOF_WEB_APP_HOST mHost_X;

private:
  BOF_WEB_HANDLER_RESPONSE OnPreRouting(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes);

  BOF_WEB_HTTPS_SERVER *mpHttpsServer = nullptr; // No unique_ptr as we need both pointer for HTTPS
  BOF_WEB_HTTP_SERVER *mpHttpServer = nullptr;   // No unique_ptr as we need both pointer for HTTPS
  std::atomic<bool> mStopServerThread;
  std::thread mServerThread;
};
END_WEBRPC_NAMESPACE()
