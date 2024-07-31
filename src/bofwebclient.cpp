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
  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Create %s Http client '%s' with:\n",
           mWebClientParam_X.IsHttpsClient_B ? "Secure" : "Non-Secure", mWebClientParam_X.WebAppParam_X.AppName_S.c_str());
  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  CertificateAuthorityPath: '%s'\n",
           mWebClientParam_X.CertificateAuthorityPath_S.c_str());
  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  ReadTimeout:              %d ms\n",
           mWebClientParam_X.ReadTimeoutInMs_U32);
  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  WriteTimeout:             %d ms\n",
           mWebClientParam_X.WriteTimeoutInMs_U32);
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
  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Connect %s Http client '%s' to %s:%d\n",
           mWebClientParam_X.IsHttpsClient_B ? "Secure" : "Non-Secure", mWebClientParam_X.WebAppParam_X.AppName_S.c_str(), _rIpAddress_S.c_str(), _Port_U16);
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
  else
  {
    LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot connect %s Http client '%s' to %s:%d\n",
              mWebClientParam_X.IsHttpsClient_B ? "Secure" : "Non-Secure", mWebClientParam_X.WebAppParam_X.AppName_S.c_str(), _rIpAddress_S.c_str(), _Port_U16);
  }
  return Rts_B;
}
bool BofWebClient::Disconnect()
{
  bool Rts_B = false;

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Disconnect %s Http client '%s'\n",
           mWebClientParam_X.IsHttpsClient_B ? "Secure" : "Non-Secure", mWebClientParam_X.WebAppParam_X.AppName_S.c_str());

  if (mpuHttpClient)
  {
    mpuHttpClient->stop();
    mpuHttpClient.reset(nullptr);
    Rts_B = true;
  }
  if (!Rts_B)
  {
    LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot disconnect %s Http client '%s'\n",
              mWebClientParam_X.IsHttpsClient_B ? "Secure" : "Non-Secure", mWebClientParam_X.WebAppParam_X.AppName_S.c_str());
  }
  return Rts_B;
}

BOF_WEB_RESULT BofWebClient::Get(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Get(_rUri_S, _rHeaderCollection_X);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Post(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X,
                                  size_t _BodyLength, const char *_pBody_c, const std::string &_rBodyType_S)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Post(_rUri_S, _rHeaderCollection_X, _pBody_c, _BodyLength, _rBodyType_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Put(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X,
                                 size_t _BodyLength, const char *_pBody_c, const std::string &_rBodyType_S)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Put(_rUri_S, _rHeaderCollection_X, _pBody_c, _BodyLength, _rBodyType_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Patch(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X,
                                   size_t _BodyLength, const char *_pBody_c, const std::string &_rBodyType_S)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Patch(_rUri_S, _rHeaderCollection_X, _pBody_c, _BodyLength, _rBodyType_S);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Delete(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Delete(_rUri_S, _rHeaderCollection_X);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Options(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Options(_rUri_S, _rHeaderCollection_X);
  }
  return Rts;
}
BOF_WEB_RESULT BofWebClient::Head(const std::string &_rUri_S, bool _Compress_B, bool _KeepAlive_B, const BOF_WEB_HEADER &_rHeaderCollection_X)
{
  BOF_WEB_RESULT Rts;

  if (mpuHttpClient)
  {
    mpuHttpClient->set_keep_alive(_KeepAlive_B);
    mpuHttpClient->set_compress(_Compress_B);
    mpuHttpClient->set_decompress(_Compress_B);
    Rts = mpuHttpClient->Head(_rUri_S, _rHeaderCollection_X);
  }
  return Rts;
}
bool BofWebClient::Upload(const std::string _rFilePathToUpload_S, const std::string _rDestinationUri_S, bool _Compress_B, bool _KeepAlive_B,
                          uint32_t _ChunkSizeInByte_U32)
{
  bool Rts_B = false;
  FILE *pIo_X;
  std::string SessionId_S, ContentRange_S;
  BOF_WEB_HEADER HeaderCollection_X;
  size_t RangeMin, RangeMax, DataSize, ReadSize, NewRangeMin, NewRangeMax, NewDataSize;
  char pRangeRequest_c[0x100];
  uint32_t ChunkSize_U32;
  uint8_t *pChunk_U8;
  BOF_WEB_RESULT Res;

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Upload '%s' in '%s': Compress %s KeepAlive %s ChunkSize %d B\n",
           _rFilePathToUpload_S.c_str(), _rDestinationUri_S.c_str(), _Compress_B ? "True" : "False", _KeepAlive_B ? "True" : "False", _ChunkSizeInByte_U32);
  if (mpuHttpClient)
  {
    pIo_X = fopen(_rFilePathToUpload_S.c_str(), "rb");
    if (pIo_X != nullptr)
    {
      fseek(pIo_X, 0, SEEK_END);
      DataSize = ftell(pIo_X);
      fseek(pIo_X, 0, SEEK_SET);
      _ChunkSizeInByte_U32 = (_ChunkSizeInByte_U32 == 0) ? (64 * 1024) : _ChunkSizeInByte_U32;
      pChunk_U8 = new uint8_t[_ChunkSizeInByte_U32];
      if (pChunk_U8)
      {
        SessionId_S = BofWebApp::S_GenerateSessionId(36); // Like UUID without start and stop delimiter {}
        RangeMin = 0;
        if (DataSize <= _ChunkSizeInByte_U32)
        {
          RangeMax = DataSize - 1;
        }
        else
        {
          RangeMax = _ChunkSizeInByte_U32 - 1;
        }
        while (1)
        {
          ReadSize = RangeMax - RangeMin + 1;
          if (fread(pChunk_U8, ReadSize, 1, pIo_X) == 1)
          {
            HeaderCollection_X.clear();
            HeaderCollection_X.insert(std::make_pair("Session-Id", SessionId_S));
            sprintf(pRangeRequest_c, "bytes %zu-%zu/%zu", RangeMin, RangeMax, DataSize);
            HeaderCollection_X.insert(std::make_pair("Content-Range", std::string(pRangeRequest_c)));
            Res = Post(_rDestinationUri_S, _Compress_B, _KeepAlive_B, HeaderCollection_X, ReadSize, (const char *)pChunk_U8, "application/octet-stream");
            if ((!Res) || ((Res->status != BOF_WEB_STATUS::OK_200) && (Res->status != BOF_WEB_STATUS::PartialContent_206)))
            {
              if (Res)
              {
                LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Upload error %d: %s\n", Res->status,
                          BOF_WEB_LIB::status_message(Res->status));
              }
              break;
            }
            ContentRange_S = Res->get_header_value("Content-Range");
            if (BofWebApp::S_ParseContentRangeRequest(ContentRange_S, NewRangeMin, NewRangeMax, NewDataSize) && (RangeMin == NewRangeMin) &&
                (RangeMax == NewRangeMax) && (DataSize == NewDataSize))
            {
              if (Res->status == BOF_WEB_STATUS::OK_200)
              {
                Rts_B = (RangeMax >= (DataSize - 1));
                break;
              }
              if (Res->status == BOF_WEB_STATUS::PartialContent_206)
              {
                RangeMin = RangeMax + 1;
                RangeMax = RangeMin + ReadSize - 1;
                if (RangeMax >= (DataSize - 1))
                {
                  RangeMax = DataSize - 1;
                }
              }
            }
            else
            {
              LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Bad control data %zu/%zu,%zu/%zu,%zu/%zu\n", RangeMin,
                        NewRangeMin, RangeMax, NewRangeMax, DataSize, NewDataSize);
              break;
            }
          }
          else
          {
            LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot read buffer %d:%p\n", Res->body.size(),
                      Res->body.c_str());
            break;
          }
        } // while(1)
        delete[] pChunk_U8;
      }
      else
      {
        LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot allocate %d byte of memory\n", _ChunkSizeInByte_U32);
      }
      fclose(pIo_X);
    }
    else
    {
      LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot open file '%s'\n", _rFilePathToUpload_S.c_str());
    }
  }
  if (!Rts_B)
  {
    LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP],
              "Cannot upload '%s' in '%s': Compress %s KeepAlive %s ChunkSize %d B\n", _rFilePathToUpload_S.c_str(), _rDestinationUri_S.c_str(),
              _Compress_B ? "True" : "False", _KeepAlive_B ? "True" : "False", _ChunkSizeInByte_U32);
  }
  return Rts_B;
}

bool BofWebClient::Download(const std::string _rSourceUri_S, const std::string _rFilePathWhereToStore_S, bool _Compress_B, bool _KeepAlive_B,
                            uint32_t _ChunkSizeInByte_U32)
{
  bool Rts_B = false;
  FILE *pIo_X;
  std::string SessionId_S, ContentRange_S;
  BOF_WEB_HEADER HeaderCollection_X;
  size_t RangeMin, RangeMax, DataSize, NewRangeMin, NewRangeMax, NewDataSize;
  char pRangeRequest_c[0x100];
  BOF_WEB_RESULT Res;
  uint32_t ChunkSize_U32;

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Download '%s' in '%s': Compress %s KeepAlive %s ChunkSize %d B\n",
           _rSourceUri_S.c_str(), _rFilePathWhereToStore_S.c_str(), _Compress_B ? "True" : "False", _KeepAlive_B ? "True" : "False", _ChunkSizeInByte_U32);
  if (mpuHttpClient)
  {
    pIo_X = fopen(_rFilePathWhereToStore_S.c_str(), "wb");
    if (pIo_X != nullptr)
    {
      SessionId_S = BofWebApp::S_GenerateSessionId(36); // Like UUID without start and stop delimiter {}
      RangeMin = 0;
      RangeMax = _ChunkSizeInByte_U32 - 1;
      DataSize = 0;

      while (1)
      {
        HeaderCollection_X.clear();
        HeaderCollection_X.insert(std::make_pair("Session-Id", SessionId_S));
        sprintf(pRangeRequest_c, "bytes %zu-%zu/%zu", RangeMin, RangeMax, DataSize);
        HeaderCollection_X.insert(std::make_pair("Content-Range", std::string(pRangeRequest_c)));
        Res = Get(_rSourceUri_S, _Compress_B, _KeepAlive_B, HeaderCollection_X);
        if ((!Res) || ((Res->status != BOF_WEB_STATUS::OK_200) && (Res->status != BOF_WEB_STATUS::PartialContent_206)))
        {
          if (Res)
          {
            LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Download error %d: %s\n", Res->status,
                      BOF_WEB_LIB::status_message(Res->status));
          }
          break;
        }
        if (Res->body.size())
        {
          ContentRange_S = Res->get_header_value("Content-Range");
          if (BofWebApp::S_ParseContentRangeRequest(ContentRange_S, NewRangeMin, NewRangeMax, NewDataSize) && (RangeMin == NewRangeMin) &&
              (RangeMax == NewRangeMax) && (DataSize == NewDataSize))
          {
            ChunkSize_U32 = (RangeMax - RangeMin + 1);
            if (ChunkSize_U32 != Res->body.size())
            {
              LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Data size mismatch %d != %d\n", RangeMax - RangeMin,
                        Res->body.size());
              break;
            }
            else
            {
              if (fwrite(Res->body.c_str(), Res->body.size(), 1, pIo_X) == 1)
              {
                RangeMin = RangeMax + 1;
                RangeMax = RangeMin + ChunkSize_U32 - 1;
                if (RangeMax >= (DataSize - 1))
                {
                  RangeMax = DataSize - 1;
                }
              }
              else
              {
                LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot write buffer %d:%p\n", Res->body.size(),
                          Res->body.c_str());
                break;
              }
            }
          }
          else
          {
            LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Bad control data %zu/%zu,%zu/%zu,%zu/%zu\n", RangeMin,
                      NewRangeMin, RangeMax, NewRangeMax, DataSize, NewDataSize);
            break;
          }
        }
        else
        {
          LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "No data received\n");
          break;
        }
        if (Res->status == BOF_WEB_STATUS::OK_200)
        {
          Rts_B = (RangeMax >= (DataSize - 1));
          break;
        }
        if (Res->status == BOF_WEB_STATUS::PartialContent_206)
        {
        }
      }
      fclose(pIo_X);
    }
    else
    {
      LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot create file '%s'\n", _rFilePathWhereToStore_S.c_str());
    }
  }
  if (!Rts_B)
  {
    LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP],
              "Cannot download '%s' in '%s': Compress %s KeepAlive %s ChunkSize %d B\n", _rSourceUri_S.c_str(), _rFilePathWhereToStore_S.c_str(),
              _Compress_B ? "True" : "False", _KeepAlive_B ? "True" : "False", _ChunkSizeInByte_U32);
  }
  return Rts_B;
}
END_WEBRPC_NAMESPACE()