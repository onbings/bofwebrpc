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

BEGIN_WEBRPC_NAMESPACE()
struct BOF_WEB_CLIENT_PARAM
{
  bool IsHttpsClient_B;
  std::string CertificateAuthorityPath_S;
  bool DisableServerCertificateVerification_B;
  uint32_t ReadTimeoutInMs_U32;
  uint32_t WriteTimeoutInMs_U32;
  BOF_WEB_APP_PARAM WebAppParam_X;

  BOF_WEB_CLIENT_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    IsHttpsClient_B = false;
    CertificateAuthorityPath_S = "";
    DisableServerCertificateVerification_B = false;
    ReadTimeoutInMs_U32 = 0;
    WriteTimeoutInMs_U32 = 0;
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

  BOF_WEB_RESULT Get(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X);
  BOF_WEB_RESULT Post(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X, size_t _BodyLength,
                      const char *_pBody_c, const std::string &_rBodyType_S);
  BOF_WEB_RESULT Put(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X, size_t _BodyLength,
                     const char *_pBody_c, const std::string &_rBodyType_S);
  BOF_WEB_RESULT Patch(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X, size_t _BodyLength,
                       const char *_pBody_c, const std::string &_rBodyType_S);
  BOF_WEB_RESULT Delete(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X);
  BOF_WEB_RESULT Options(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X);
  BOF_WEB_RESULT Head(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeader_X);

  bool Upload(const std::string _rFilePathToUpload_S, const std::string _rDestinationUrl_S, bool _Compress_B, bool _KeepAlive_B, uint32_t _ChunkSizeInByte_U32);
  bool Download(const std::string _rSourceUri_S, const std::string _rFilePathWhereToStore_S, bool _Compress_B, bool _KeepAlive_B,
                uint32_t _ChunkSizeInByte_U32);

  virtual void V_OnSetSocketOption(BOF_WEB_SOCKET _Socket)
  {
  }

private:
  BOF_WEB_CLIENT_PARAM mWebClientParam_X;
  std::unique_ptr<BOF_WEB_HTTP_CLIENT> mpuHttpClient;
};

END_WEBRPC_NAMESPACE()