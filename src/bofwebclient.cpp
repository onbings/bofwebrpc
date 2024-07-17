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

bool BofWebClient::Connect(const std::string &_rIpAddress_S, uint16_t _Port_U16)
{
  bool Rts_B = false;
  bool CreateHttpsClient_B;
  //
  CreateHttpsClient_B = mWebClientParam_X.CertificatePath_S.empty() ? false : true;
  if (CreateHttpsClient_B)
  {
    CreateHttpsClient_B = mWebClientParam_X.PrivateKeyPath_S.empty() ? false : true;
  }
  if (CreateHttpsClient_B)
  {
    mpuWebClientProxy = std::make_unique<httplib::Client>(_rIpAddress_S, _Port_U16, mWebClientParam_X.CertificatePath_S, mWebClientParam_X.PrivateKeyPath_S);
  }
  else
  {
    mpuWebClientProxy = std::make_unique<httplib::Client>(_rIpAddress_S, _Port_U16);
  }
  // Rts_B = mpuWebServerProxy->Start(_rIpAddress_S, _Port_U16);
  Rts_B = (mpuWebClientProxy != nullptr);
  if (Rts_B)
  {
    // Hand the app under linux/gcc13 if used here, do it in get, pots,...
    //  HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpcAgent/1.0.0"));
    //   mpuWebClientProxy->set_default_headers(HeaderCollection);
    //     mpuWebClientProxy->set_default_headers({{"User-Agent", "BofWebRpcAgent/1.0.0"}});
  }
  return Rts_B;
}
bool BofWebClient::Disconnect()
{
  bool Rts_B = false;

  // Rts_B = mpuWebServerProxy->Stop();

  return Rts_B;
}

httplib::Result BofWebClient::Get(const std::string &_rUrl_S, bool _Compress_B, bool _KeepAlive_B)
{
  httplib::Result Rts;
  httplib::Headers HeaderCollection;

  if (mpuWebClientProxy)
  {
    mpuWebClientProxy->set_keep_alive(_KeepAlive_B);
    HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpcAgent/1.0.0"));
    if (_Compress_B)
    {
      HeaderCollection.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
    }
    Rts = mpuWebClientProxy->Get(_rUrl_S, HeaderCollection);
  }
  return Rts;
}

httplib::Result BofWebClient::Post(const std::string &_rUrl_S, bool _Compress_B)
{
  httplib::Result Rts;

  // Rts_B = mpuWebServerProxy->Stop();
  // cli.set_keep_alive(true);
  // cli.Get("/world");
  // httplib::Params params{{"name", "john"}, {"note", "coder"}};

  // auto res = cli.Post("/post", params);
  return Rts;
}

bool BofWebClient ::Upload(const std::string _rFilePathToUpload_S, const std::string _rDestinationUrl_S, uint32_t _ChunkSizeInByte_U32)
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

          Res = mpuWebClientProxy->Post(_rDestinationUrl_S, HeaderCollection, (const char *)pChunk_U8, BytesRead, "application/octet-stream");

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
END_WEBRPC_NAMESPACE()