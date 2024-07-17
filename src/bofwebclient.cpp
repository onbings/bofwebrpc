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

httplib::Result BofWeb::G()
{
  //  std::unique_ptr<httplib::Client> mpuWebClientProxy;
  mpuWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090); //_rIpAddress_S, _Port_U16);
  printf("call in BofWeb G %p (/hi)\n", mpuWebClientProxy.get());
  return mpuWebClientProxy->Get("/hi");
}

#if 0
class IBofWebClientProxy
{
public:
  virtual ~IBofWebClientProxy() = default;
  // inline virtual bool V_Get(const std::string &_rPattern_S, httplib::Server::Handler _Handler) = 0;
};
// HTTP httplib::Serve or HTTPS httplib::SSLServer
template <typename T> class BofWebClientProxy : public IBofWebClientProxy
{
public:
  // Constructor for HTTP server
  BofWebClientProxy()
  {
    if constexpr (std::is_same<T, httplib::Server>::value)
    {
      mpuSvr = std::make_unique<T>();
    }
  }

  // Constructor for HTTPS server
  BofWebClientProxy(const std::string &_rCertificatePath_S, const std::string &_rPrivateKeyPath_S)
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

private:
  // bool mUseHttps_B = false;
  std::unique_ptr<T> mpuCli; // HTTP httplib::Server or HTTPS httplib::SSLServer
}
#endif

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
  std::multimap<std::string, std::string, httplib::detail::ci> HeaderCollection;

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
    // HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpcAgent/1.0.0"));
    // mpuWebClientProxy->set_default_headers(HeaderCollection);
    //  mpuWebClientProxy->set_default_headers({{"User-Agent", "BofWebRpcAgent/1.0"}});
  }
  return Rts_B;
}
bool BofWebClient::Disconnect()
{
  bool Rts_B = false;

  // Rts_B = mpuWebServerProxy->Stop();

  return Rts_B;
}
httplib::Result BofWebClient::G()
{
  // Connect("10.129.170.29", 8090);
  mpuWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090); //_rIpAddress_S, _Port_U16);
  printf("call in client G %p (/hi)\n", mpuWebClientProxy.get());
  return mpuWebClientProxy->Get("/hi");
}
httplib::Result BofWebClient::Get(const std::string &_rUrl_S, bool _Compress_B, bool _KeepAlive_B)
{
//  printf("call in client %p (%s)\n", mpuWebClientProxy.get(), _rUrl_S.c_str());
//  return mpuWebClientProxy.get()->Get(_rUrl_S);
#if 1
  httplib::Result Rts;
  httplib::Headers HeaderCollection;

  if (mpuWebClientProxy)
  {
    mpuWebClientProxy->set_keep_alive(_KeepAlive_B);
    if (_Compress_B)
    {
      HeaderCollection = {{"Accept-Encoding", "gzip, deflate"}};
    }
    //    Rts = mpuWebClientProxy->Get(_rUrl_S, HeaderCollection);
    Rts = mpuWebClientProxy->Get(_rUrl_S);
  }
  // Rts_B = mpuWebServerProxy->Stop();
  // cli.set_keep_alive(true);
  // cli.Get("/world");
  // httplib::Headers headers = {{"Accept-Encoding", "gzip, deflate"}};
  // auto res = cli.Get("/hi", headers);
  return Rts;
#endif
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