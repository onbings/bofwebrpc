/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
#include "bofwebrpc/bofwebclient.h"
#include <stdio.h>

BEGIN_WEBRPC_NAMESPACE()
BofWebClient::BofWebClient(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_CLIENT_PARAM &_rWebClientParam_X)
    : BofWebApp(_psLoggerFactory, false, _rWebClientParam_X.WebAppParam_X)
{
  mWebClientParam_X = _rWebClientParam_X;
}
BofWebClient ::~BofWebClient()
{
  Disconnect();
}

bool BofWebClient::Connect(uint32_t _TimeOutInMs_U32, const std::string &_rIpAddress_S, uint16_t _Port_U16)
{
  bool Rts_B = false;
  char pIpAddress_c[0x100];
  BOF_WEB_HEADER HeaderCollection;

  if (mWebClientParam_X.IsHttpsClient_B)
  {
    sprintf(pIpAddress_c, "https://%s:%d", _rIpAddress_S.c_str(), _Port_U16);
    mpuHttpClient = std::make_unique<httplib::Client>(pIpAddress_c);
  }
  else
  {
    mpuHttpClient = std::make_unique<httplib::Client>(_rIpAddress_S, _Port_U16);
  }
  // Rts_B = mpuWebServerProxy->Start(_rIpAddress_S, _Port_U16);
  Rts_B = (mpuHttpClient != nullptr);
  if (Rts_B)
  {
    mpuHttpClient->set_socket_options([this](BOF_WEB_SOCKET _Socket) { this->V_OnSetSocketOption(_Socket); });
    mpuHttpClient->set_connection_timeout(std::chrono::milliseconds(_TimeOutInMs_U32));
    mpuHttpClient->set_read_timeout(std::chrono::milliseconds(mWebClientParam_X.ReadTimeoutInMs_U32));
    mpuHttpClient->set_write_timeout(std::chrono::milliseconds(mWebClientParam_X.WriteTimeoutInMs_U32));
    mpuHttpClient->set_ca_cert_path(mWebClientParam_X.CertificateAuthorityPath_S);
    mpuHttpClient->enable_server_certificate_verification(!mWebClientParam_X.DisableServerCertificateVerification_B);
    HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpcAgent/1.0.0"));
    mpuHttpClient->set_default_headers(HeaderCollection);
  }
  return Rts_B;
}
bool BofWebClient::Disconnect()
{
  bool Rts_B = false;

  if (mpuHttpClient)
  {
    mpuHttpClient->stop();
    mpuHttpClient.reset(nullptr);
    Rts_B = true;
  }
  return Rts_B;
}

BOF_WEB_RESULT BofWebClient::Get(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Get(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Post(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Post(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Put(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Put(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Patch(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Patch(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Delete(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Delete(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Options(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Options(_rUri_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Head(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Head(_rUri_S);
  }
  return Rts;
}
bool BofWebClient ::Upload(const std::string _rFilePathToUpload_S, const std::string _rDestinationUri_S, uint32_t _ChunkSizeInByte_U32)
{
  bool Rts_B = false;
  FILE *pIo_X;
  size_t FileSize, TotalByteSent, BytesRead;
  uint8_t *pChunk_U8;
  std::string SessionId_S;
  httplib::Headers HeaderCollection;
  httplib::Result Res;
  float Progress_f;

  pIo_X = fopen(_rFilePathToUpload_S.c_str(), "rb");
  if (pIo_X)
  {
    fseek(pIo_X, 0, SEEK_END);
    FileSize = ftell(pIo_X);
    fseek(pIo_X, 0, SEEK_SET);
    _ChunkSizeInByte_U32 = (_ChunkSizeInByte_U32 == 0) ? (64 * 1024) : _ChunkSizeInByte_U32;

    pChunk_U8 = new uint8_t[_ChunkSizeInByte_U32];
    if (pChunk_U8)
    {
      TotalByteSent = 0;
      SessionId_S = GenerateSessionId(16);
      HeaderCollection.clear();
      HeaderCollection.emplace("session_id", SessionId_S);
      Rts_B = true;
      while (!feof(pIo_X))
      {
        BytesRead = fread(pChunk_U8, 1, _ChunkSizeInByte_U32, pIo_X);
        if (BytesRead < 0)
        {
          Rts_B = false;
          break;
        }
        else
        {
          if (feof(pIo_X))
          {
            HeaderCollection.emplace("final_chunk", "true");
          }

          Res = mpuHttpClient->Post(_rDestinationUri_S, HeaderCollection, (const char *)pChunk_U8, BytesRead, "application/octet-stream");

          if ((!Res) || (Res->status != httplib::StatusCode::OK_200))
          {
            Rts_B = false;
            break;
          }

          TotalByteSent += BytesRead;
          Progress_f = static_cast<float>(TotalByteSent) / static_cast<float>(FileSize) * 100.0f;
        }
      }
      delete[] pChunk_U8;
    } // if (pChunk_U8)
    fclose(pIo_X);
  } // if (pIo_X)
  return Rts_B;
}

bool Download(const std::string _rSourceUri_S, const std::string _rFilePathWhereToStore_S, uint32_t _ChunkSizeInByte_U32)
{
  return false;
}
END_WEBRPC_NAMESPACE()