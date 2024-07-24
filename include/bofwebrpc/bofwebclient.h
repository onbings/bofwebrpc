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
class Client;
}

BEGIN_WEBRPC_NAMESPACE()
struct BOF_WEB_CLIENT_PARAM
{
  std::string CertificatePath_S; // If empty create an HTTP server instead of an HTTPS
  std::string PrivateKeyPath_S;
  std::string CertificateAuthorityPath_S;
  bool DisableServerCertificateVerification_B;
  BOF_WEB_APP_PARAM WebAppParam_X;

  BOF_WEB_CLIENT_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    CertificatePath_S = "";
    PrivateKeyPath_S = "";
    CertificateAuthorityPath_S = "";
    DisableServerCertificateVerification_B = false;
    WebAppParam_X.Reset();
  }
};
class BofWebClient : public BofWebApp
{
public:
  BofWebClient(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_CLIENT_PARAM &_rWebClientParam_X);
  virtual ~BofWebClient();
  bool Connect(uint32_t _TimeOutInMs_U32, const std::string &_rIpAddress_S, uint16_t _Port_U16);
  bool Disconnect();
  BOF_WEB_RESULT Get(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B);
  bool Upload(const std::string _rFilePathToUpload_S, const std::string _rDestinationUrl_S, uint32_t _ChunkSizeInByte_U32);

private:
  BOF_WEB_CLIENT_PARAM mWebClientParam_X;
  std::unique_ptr<httplib::Client> mpuWebClientProxy;
};

END_WEBRPC_NAMESPACE()