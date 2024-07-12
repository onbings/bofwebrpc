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
#include "httplib.h"

BEGIN_WEBRPC_NAMESPACE()
BofWebServer::BofWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
    : BofWebApp(_psLoggerFactory, true, _rWebServerParam_X.WebAppParam_X)
{
  mUseHttps_B = _rWebServerParam_X.CertificatePath_S.empty() ? false : true;
  if (mUseHttps_B)
  {
    mUseHttps_B = _rWebServerParam_X.PrivateKeyPath_S.empty() ? true : false;
  }
  if (mUseHttps_B)
  {
    mpuHttpsSvr = std::make_unique<httplib::SSLServer>(_rWebServerParam_X.CertificatePath_S.c_str(), _rWebServerParam_X.PrivateKeyPath_S.c_str());
  }
  else
  {
    mpuHttpSvr = std::make_unique<httplib::Server>();
  }
}
BofWebServer ::~BofWebServer()
{
}
bool BofWebServer::Start()
{
  bool Rts_B = false;
  WEB_APP_HOST Host_X(BofWebApp::GetConfig().at("httpServer"));

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Starting Server on %s:%d\n", Host_X.IpAddress_S.c_str(),
           Host_X.Port_U16);
  for (auto &rBannedIp : Host_X.BannedIpAddressCollection)
  {
    LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  Banned IP: %s\n", rBannedIp.c_str());
  }
  if (mUseHttps_B)
  {
    mpuHttpsSvr->listen(Host_X.IpAddress_S.c_str(), Host_X.Port_U16);
  }
  else
  {

    mpuHttpsSvr->set_error_handler([](const httplib::Request &_rReq, httplib::Response &_rRes) {
      const char *pFmt_c = "<p>Error Status: <span style='color:red;'>%d</span></p>";
      char pBuffer_c[0x4000];
      int Len_i = snprintf(pBuffer_c, sizeof(pBuffer_c), pFmt_c, _rRes.status);
      _rRes.set_content(pBuffer_c, Len_i, std::string("text/html"));
    });

    mpuHttpsSvr->set_logger([this](const httplib::Request &_rReq, const httplib::Response &_rRes) {
      LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "%s", LogRequest(_rReq, _rRes).c_str());
    });

    mpuHttpsSvr->listen(Host_X.IpAddress_S.c_str(), Host_X.Port_U16);
  }

  Rts_B = true;
  return Rts_B;
}

bool BofWebServer::Stop()
{
  bool Rts_B = false;
  if (mUseHttps_B)
  {
    mpuHttpsSvr->stop();
  }
  else
  {
    mpuHttpsSvr->stop();
  }
  Rts_B = true;
  return Rts_B;
}
END_WEBRPC_NAMESPACE()