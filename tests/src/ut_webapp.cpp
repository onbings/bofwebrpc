/*
 * Copyright (c) 2024-2044, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines and implements the BofImgui unit tests
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Feb 19 2024  BHA : Initial release
 */
#include "gtestrunner.h"
#include <algorithm>
#include <bofwebrpc/bofwebserver.h>
#include <bofwebrpc/bofwebclient.h>
#include <bofstd/bofbasicloggerfactory.h>
#include <bofstd/bofsystem.h>
#include <bofstd/boffs.h>

// constexpr const char *WEB_SERVER_IP_ADDRESS = "0.0.0.0";
constexpr const char *WEB_SERVER_IP_ADDRESS = "10.129.170.29";
constexpr uint16_t WEB_SERVER_PORT = 8090;
constexpr uint32_t UPDOWN_CHUNK_SIZE = (64 * 1024);

class UtWebServer : public BOFWEBRPC::BofWebServer
{
public:
  UtWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOFWEBRPC::BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
      : BOFWEBRPC::BofWebServer(_psLoggerFactory, _rWebServerParam_X)
  {
    EXPECT_TRUE(Get("/", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->GetRoot(_rReq, _rRes); }));
    EXPECT_TRUE(Get("/hi", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->GetHi(_rReq, _rRes); }));

    // Match the request path against a regular expression and extract its captures
    EXPECT_TRUE(
        Get(R"(/numbers/(\d+))", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->GetNumber(_rReq, _rRes); }));
    // Capture the second segment of the request path as "id" path param
    EXPECT_TRUE(Get("/users/:id", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->GetUser(_rReq, _rRes); }));
    // Extract values from HTTP headers and URL query params
    EXPECT_TRUE(
        Get("/download/:dir/:file", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->GetDownload(_rReq, _rRes); }));
    EXPECT_TRUE(
        Post("/upload/:dir/:file", [this](const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) { this->PostUpload(_rReq, _rRes); }));

    EXPECT_FALSE(SetMountPoint("/www", "/tmp/dontexist"));
    EXPECT_TRUE(SetMountPoint("/www", mWebServerParam_X.RootDir_S));
    EXPECT_TRUE(SetMountPoint("/abc", mWebServerParam_X.RootDir_S));
    EXPECT_TRUE(RemoveMountPoint("/abc"));
    EXPECT_TRUE(SetFileExtensionAndMimetypeMapping("bha", "mime/bha"));

    //    EXPECT_TRUE(Start(WEB_SERVER_IP_ADDRESS, bofwebapp_tests::WEB_SERVER_PORT));
    //   EXPECT_TRUE(IsRunning());
  }
  ~UtWebServer()
  {
  }
  // Ut
  uint32_t mOnFileRequest_U32 = 0;
  uint32_t mOnError_U32 = 0;
  uint32_t mOnException_U32 = 0;
  uint32_t mOnPreRouting_U32 = 0;
  uint32_t mOnPostRouting_U32 = 0;
  uint32_t mOnExpect100Continue_U32 = 0;
  uint32_t mOnSetSocketOption_U32 = 0;
  uint32_t mGetRoot_U32 = 0;
  uint32_t mGetHi_U32 = 0;
  uint32_t mGetNumber_U32 = 0;
  uint32_t mGetUser_U32 = 0;

  std::string mNumber_S;
  std::string mUserId_S;
  std::string mDirDownload_S;
  std::string mFileDownload_S;
  std::string mDirUpload_S;
  std::string mFileUpload_S;

private:
  struct UPDOWN_TASK
  {
    uint32_t Start_U32;
    size_t RangeMin;
    size_t RangeMax;
    size_t DataSize;
    uint32_t ChunkSize_U32;
    FILE *pIo_X;
    uint8_t *pBuffer_U8;

    UPDOWN_TASK()
    {
      Reset();
    }
    void Reset()
    {
      Start_U32 = 0;
      RangeMin = 0;
      RangeMax = 0;
      DataSize = 0;
      ChunkSize_U32 = 0;
      pIo_X = nullptr;
      pBuffer_U8 = nullptr;
    }
  };
  void V_OnFileRequest(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnFileRequest\n");
    mOnFileRequest_U32++;
  }
  void V_OnError(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnError\n");
    mOnError_U32++;
  }
  void V_OnException(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes, std::exception_ptr ep) override
  {
    char pBuffer_c[0x100];
    printf("V_OnException\n");
    mOnException_U32++;
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
    _rRes.status = BOFWEBRPC::BOF_WEB_STATUS::InternalServerError_500;
  }
  bool V_OnPreRouting(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnPreRouting\n");
    mOnPreRouting_U32++;
    return false;
  }
  void V_OnPostRouting(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnPostRouting\n");
    mOnPostRouting_U32++;
  }
  // By default, the server sends a 100 Continue response for an Expect: 100-continue header.
  BOFWEBRPC::BOF_WEB_STATUS V_OnExpect100Continue(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnExpect100Continue\n");
    mOnExpect100Continue_U32++;
#if 0
    // Send a '417 Expectation Failed' response.
    BOFWEBRPC::BOF_WEB_STATUS Rts = BOFWEBRPC::BOF_WEB_STATUS::ExpectationFailed_417;
    // Send a final status httplib::detail::without reading the message body.
    Rts = BOFWEBRPC::BOF_WEB_STATUS::Unauthorized_401;
    _rRes_X.status = Rts;

    // Custom logic to determine if the request should be continued
    // For example, check if the Content-Length is acceptable
    auto it = _rReq_X.headers.find("Content-Length");
    if (it != _rReq_X.headers.end())
    {
      size_t ContentLength = std::stoull(it->second);
      if (ContentLength > 1024 * 1024)
      {                                                // Arbitrary limit of 1MB
        Rts = BOFWEBRPC::BOF_WEB_STATUS::LengthRequired_411; // Reject requests larger than 1MB
      }
    }
    Rts = BOFWEBRPC::BOF_WEB_STATUS::OK_200; // Continue with the request
#endif
    return BOFWEBRPC::BOF_WEB_STATUS::Continue_100;
  }
  void V_OnSetSocketOption(BOFWEBRPC::BOF_WEB_SOCKET _Socket) override
  {
    printf("V_OnSetSocketOption(%d)\n", _Socket);
    mOnSetSocketOption_U32++;
    int OptVal_i = 1; // Enable the option
    if (setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, &OptVal_i, sizeof(OptVal_i)) == -1)
    {
      //  perror("setsockopt SO_REUSEADDR failed");
    }
  }

  void GetRoot(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    printf("=================GetRoot:redirect to hi================\n");
    mGetRoot_U32++;
    _rRes.set_redirect("/hi");
  }
  void GetHi(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    static int S_Id_i = 0;
    char pRes_c[0x1000];
    int Len_i = sprintf(pRes_c, "Hello %d\n", ++S_Id_i);
    printf("=================GetHi================%d:%s\n", Len_i, pRes_c);
    mGetHi_U32++;
    _rRes.set_content(pRes_c, Len_i, "text/plain");
  }

  // Match the request path against a regular expression and extract its captures
  void GetNumber(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    mNumber_S = _rReq.matches[1];
    printf("=================GetNumber================%s\n", mNumber_S.c_str());
    mGetNumber_U32++;

    _rRes.set_content(mNumber_S, "text/plain");
  }

  // Capture the second segment of the request path as "id" path param
  void GetUser(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    auto pIt = _rReq.path_params.find("id");
    if (pIt != _rReq.path_params.end())
    {
      mUserId_S = pIt->second;
    }
    // mUserId_S = _rReq.path_params.at("id");
    printf("=================GetUser================%s\n", mUserId_S.c_str());
    mGetUser_U32++;
    _rRes.set_content(mUserId_S, "text/plain");
  }

  bool TerminateUpDownTask(UPDOWN_TASK &_rUpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS _Sts_E, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes,
                           std::map<std::string, UPDOWN_TASK>::iterator _It)
  {
    bool Rts_B = false;
    if (_rUpDownTask_X.pIo_X)
    {
      fclose(_rUpDownTask_X.pIo_X);
      _rUpDownTask_X.pIo_X = nullptr;
    }
    if (_rUpDownTask_X.pBuffer_U8)
    {
      delete[] _rUpDownTask_X.pBuffer_U8;
      _rUpDownTask_X.pBuffer_U8 = nullptr;
    }
    _rRes.status = _Sts_E;
    if (_It != mUpDownTaskCollection.end())
    {
      mUpDownTaskCollection.erase(_It);
    }
    Rts_B = true;
    return Rts_B;
  }

  void GetDownload(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    bool Continue_B = false;
    std::string SessionId_S, ContentRange_S, Dir_S, File_S, Path_S;
    size_t RangeMin, RangeMax, DataSize, ReadSize;
    char pRangeRequest_c[0x100];
    UPDOWN_TASK UpDownTask_X;
    std::map<std::string, UPDOWN_TASK>::iterator It;
    try
    {
      SessionId_S = _rReq.get_header_value("Session-Id");
      ContentRange_S = _rReq.get_header_value("Content-Range");
      Dir_S = _rReq.path_params.at("dir");
      File_S = _rReq.path_params.at("file");
      Path_S = mWebServerParam_X.RootDir_S + "/" + Dir_S + "/" + File_S;
      if (Path_S.find("..") == std::string::npos)
      {
        if (BofWebApp::S_ParseContentRangeRequest(ContentRange_S, RangeMin, RangeMax, DataSize))
        {
          Continue_B = true;
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::BadRequest_400, _rRes, It); // Missing or erronous Header item
        }
      }
      else
      {
        TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::Unauthorized_401, _rRes, It); // Missing or erronous Header item
      }
    }
    catch (const std::exception &e)
    {
      printf("Error parsing GetDownload: %s\n", e.what());
    }

    if (Continue_B)
    {
      Continue_B = false;
      It = mUpDownTaskCollection.find(SessionId_S);
      if (It == mUpDownTaskCollection.end())
      {
        if ((!RangeMin) && (RangeMax) && (!DataSize)) // First request
        {
          UpDownTask_X.pIo_X = fopen(Path_S.c_str(), "rb");
          if (UpDownTask_X.pIo_X)
          {
            fseek(UpDownTask_X.pIo_X, 0, SEEK_END);
            DataSize = ftell(UpDownTask_X.pIo_X);
            fseek(UpDownTask_X.pIo_X, 0, SEEK_SET);

            UpDownTask_X.Start_U32 = BOF::Bof_GetMsTickCount();
            UpDownTask_X.RangeMin = RangeMin;
            UpDownTask_X.RangeMax = RangeMax;
            UpDownTask_X.DataSize = DataSize;
            UpDownTask_X.ChunkSize_U32 = RangeMax + 1;
            UpDownTask_X.pBuffer_U8 = new uint8_t[UpDownTask_X.ChunkSize_U32];
            if (UpDownTask_X.pBuffer_U8)
            {
              if (UpDownTask_X.DataSize <= UpDownTask_X.ChunkSize_U32)
              {
                UpDownTask_X.RangeMax = UpDownTask_X.DataSize - 1;
                ReadSize = UpDownTask_X.DataSize;
              }
              else
              {
                ReadSize = UpDownTask_X.ChunkSize_U32;
              }
              Continue_B = true;
            }
            else
            {
              TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::InternalServerError_500, _rRes, It); // Internal Server Error
            }
          }
          else
          {
            TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::Unauthorized_401, _rRes, It); // File not found
          }
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::NotAcceptable_406, _rRes, It); // First range must be formatted as 0-chunksize/0
        }
      }
      else
      {
        UpDownTask_X = It->second;
        assert(UpDownTask_X.pIo_X != nullptr);
        assert(UpDownTask_X.pBuffer_U8 != nullptr);
        ReadSize = std::min(UpDownTask_X.ChunkSize_U32, static_cast<uint32_t>(UpDownTask_X.DataSize - UpDownTask_X.RangeMin));
        //        if ((RangeMin == (UpDownTask_X.RangeMax + 1)) && (RangeMax == (UpDownTask_X.RangeMax + ReadSize)) && (RangeMax <= UpDownTask_X.DataSize) &&
        //            (DataSize == UpDownTask_X.DataSize))
        if ((RangeMin == UpDownTask_X.RangeMin) && (RangeMax == UpDownTask_X.RangeMax) && (RangeMax <= UpDownTask_X.DataSize) &&
            (DataSize == UpDownTask_X.DataSize))
        {
          Continue_B = true;
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::NotAcceptable_406, _rRes, It); // Range mismatch
        }
      }
      if (Continue_B)
      {
        Continue_B = false;
        if (fread(UpDownTask_X.pBuffer_U8, ReadSize, 1, UpDownTask_X.pIo_X) == 1)
        {
          _rRes.set_content(reinterpret_cast<char *>(UpDownTask_X.pBuffer_U8), ReadSize, "application/octet-stream");
          sprintf(pRangeRequest_c, "bytes %zu-%zu/%zu", UpDownTask_X.RangeMin, UpDownTask_X.RangeMax, UpDownTask_X.DataSize);
          _rRes.set_header("Content-Range", std::string(pRangeRequest_c));
          if (RangeMax >= (UpDownTask_X.DataSize - 1))
          {
            TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::OK_200, _rRes, It);
          }
          else
          {
            _rRes.status = BOFWEBRPC::BOF_WEB_STATUS::PartialContent_206;
            UpDownTask_X.RangeMin = UpDownTask_X.RangeMax + 1;
            UpDownTask_X.RangeMax = UpDownTask_X.RangeMax + ReadSize;
            if (UpDownTask_X.RangeMax >= (UpDownTask_X.DataSize - 1))
            {
              UpDownTask_X.RangeMax = UpDownTask_X.DataSize - 1;
            }
            mUpDownTaskCollection[SessionId_S] = UpDownTask_X;
          }
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::InternalServerError_500, _rRes, It); // Range mismatch
        }
      }
    }
  }
  void PostUpload(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    bool Continue_B = false;
    std::string SessionId_S, ContentRange_S, Dir_S, File_S, Path_S;
    size_t RangeMin, RangeMax, DataSize, WriteSize;
    char pRangeRequest_c[0x100];
    UPDOWN_TASK UpDownTask_X;
    std::map<std::string, UPDOWN_TASK>::iterator It;
    try
    {
      SessionId_S = _rReq.get_header_value("Session-Id");
      ContentRange_S = _rReq.get_header_value("Content-Range");
      Dir_S = _rReq.path_params.at("dir");
      File_S = _rReq.path_params.at("file");
      Path_S = mWebServerParam_X.RootDir_S + "/" + Dir_S + "/" + File_S;
      if (Path_S.find("..") == std::string::npos)
      {
        if (BofWebApp::S_ParseContentRangeRequest(ContentRange_S, RangeMin, RangeMax, DataSize))
        {
          Continue_B = true;
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::BadRequest_400, _rRes, It); // Missing or erronous Header item
        }
      }
      else
      {
        TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::Unauthorized_401, _rRes, It); // Missing or erronous Header item
      }
    }
    catch (const std::exception &e)
    {
      printf("Error parsing GetDownload: %s\n", e.what());
    }

    if (Continue_B)
    {
      Continue_B = false;
      It = mUpDownTaskCollection.find(SessionId_S);
      if (It == mUpDownTaskCollection.end())
      {
        if ((!RangeMin) && (RangeMax) && (DataSize) && (RangeMax < DataSize)) // First request
        {
          UpDownTask_X.pIo_X = fopen(Path_S.c_str(), "wb");
          if (UpDownTask_X.pIo_X)
          {
            UpDownTask_X.Start_U32 = BOF::Bof_GetMsTickCount();
            UpDownTask_X.pBuffer_U8 = nullptr;
            UpDownTask_X.RangeMin = RangeMin;
            UpDownTask_X.RangeMax = RangeMax;
            UpDownTask_X.DataSize = DataSize;
            UpDownTask_X.ChunkSize_U32 = UpDownTask_X.RangeMax + 1;
            WriteSize = (UpDownTask_X.DataSize <= UpDownTask_X.ChunkSize_U32) ? UpDownTask_X.DataSize : UpDownTask_X.ChunkSize_U32;
            Continue_B = true;
          }
          else
          {
            TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::Unauthorized_401, _rRes, It); // File not found
          }
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::NotAcceptable_406, _rRes, It); // First range must be formatted as 0-chunksize/0
        }
      }
      else
      {
        UpDownTask_X = It->second;
        assert(UpDownTask_X.pIo_X != nullptr);
        assert(UpDownTask_X.pBuffer_U8 == nullptr);
        WriteSize = std::min(UpDownTask_X.ChunkSize_U32, static_cast<uint32_t>(UpDownTask_X.DataSize - UpDownTask_X.RangeMin));
        if ((RangeMin == UpDownTask_X.RangeMin) && (RangeMax == UpDownTask_X.RangeMax) && (RangeMax <= UpDownTask_X.DataSize) &&
            (DataSize == UpDownTask_X.DataSize))
        {
          Continue_B = true;
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::NotAcceptable_406, _rRes, It); // Range mismatch
        }
      }
      if (Continue_B)
      {
        Continue_B = false;
        if (WriteSize == _rReq.body.size())
        {
          if (fwrite(_rReq.body.c_str(), WriteSize, 1, UpDownTask_X.pIo_X) == 1)
          {
            sprintf(pRangeRequest_c, "bytes %zu-%zu/%zu", UpDownTask_X.RangeMin, UpDownTask_X.RangeMax, UpDownTask_X.DataSize);
            _rRes.set_header("Content-Range", std::string(pRangeRequest_c));
            if (RangeMax >= (UpDownTask_X.DataSize - 1))
            {
              TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::OK_200, _rRes, It);
            }
            else
            {
              _rRes.status = BOFWEBRPC::BOF_WEB_STATUS::PartialContent_206;
              UpDownTask_X.RangeMin = UpDownTask_X.RangeMax + 1;
              UpDownTask_X.RangeMax = UpDownTask_X.RangeMax + WriteSize;
              if (UpDownTask_X.RangeMax >= (UpDownTask_X.DataSize - 1))
              {
                UpDownTask_X.RangeMax = UpDownTask_X.DataSize - 1;
              }
              mUpDownTaskCollection[SessionId_S] = UpDownTask_X;
            }
          }
          else
          {
            TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::InsufficientStorage_507, _rRes, It); // I/O mismatch
          }
        }
        else
        {
          TerminateUpDownTask(UpDownTask_X, BOFWEBRPC::BOF_WEB_STATUS::InternalServerError_500, _rRes, It); // Range mismatch
        }
      }
    }
  }

private:
  std::map<std::string, UPDOWN_TASK> mUpDownTaskCollection;
};

class UtWebClient : public BOFWEBRPC::BofWebClient
{
public:
  UtWebClient(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOFWEBRPC::BOF_WEB_CLIENT_PARAM &_rWebClientParam_X)
      : BOFWEBRPC::BofWebClient(_psLoggerFactory, _rWebClientParam_X)
  {
  }
  ~UtWebClient()
  {
  }
  // Ut
  uint32_t mOnSetSocketOption_U32 = 0;

private:
  void V_OnSetSocketOption(BOFWEBRPC::BOF_WEB_SOCKET _Socket) override
  {
    printf("V_OnSetSocketOption(%d)\n", _Socket);
    mOnSetSocketOption_U32++;
    /*
        int OptVal_i = 1; // Enable the option
        if (setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, &OptVal_i, sizeof(OptVal_i)) == -1)
        {
          //  perror("setsockopt SO_REUSEADDR failed");
        }
    */
  }
};
class bofwebapp_tests : public ::testing::Test
{
protected:
  void SetUp() override
  {
  }

  bool CreateClientAndServer(bool _UseHttps_B)
  {
    bool Rts_B = false;
    std::string Path_S;

    if (_UseHttps_B)
    {
      mWebServerParam_X.CertificatePath_S = "/home/bha/pro/evs-hwfw-sb-xts/cert.pem";
      mWebServerParam_X.PrivateKeyPath_S = "/home/bha/pro/evs-hwfw-sb-xts/key.pem";
    }
    mWebServerParam_X.ServerStartStopTimeoutInMs_U32 = 2000;
    mWebServerParam_X.LogRequestAndResponse_B = true;
    mWebServerParam_X.KeepAliveMaxCount_U32 = 25000;
    mWebServerParam_X.KeepAliveTimeoutInMs_U32 = 1000;
    mWebServerParam_X.ReadTimeoutInMs_U32 = 1000;
    mWebServerParam_X.WriteTimeoutInMs_U32 = 1000;
    mWebServerParam_X.IdleIntervalInMs_U32 = 2000;
    mWebServerParam_X.PayloadMaxLengthInByte_U32 = 0x100000;
    mWebServerParam_X.ThreadPoolSize_U32 = 12;
    EXPECT_EQ(BOF::Bof_GetCurrentDirectory(mWebServerParam_X.RootDir_S), BOF_ERR_NO_ERROR);

    Path_S = mWebServerParam_X.RootDir_S + "/local/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, Path_S), BOF_ERR_NO_ERROR);
    EXPECT_EQ(BOF::Bof_CleanupDirectory(true, Path_S, false), BOF_ERR_NO_ERROR);
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/MulticamDeveloper_21.00.00.83879.tar.gz", UPDOWN_CHUNK_SIZE * 100, 0));
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/less_than_one_chunk_size.bin", UPDOWN_CHUNK_SIZE / 2, 1));
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/one_chunk_size.bin", UPDOWN_CHUNK_SIZE, 2));
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/more_than_one_chunk_size.bin", (UPDOWN_CHUNK_SIZE * 4) + 5678, 4));

    mWebServerParam_X.RootDir_S += "/www/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, mWebServerParam_X.RootDir_S), BOF_ERR_NO_ERROR);
    EXPECT_EQ(BOF::Bof_CleanupDirectory(true, mWebServerParam_X.RootDir_S, false), BOF_ERR_NO_ERROR);

    Path_S = mWebServerParam_X.RootDir_S + "/log/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, Path_S), BOF_ERR_NO_ERROR);
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/less_than_one_chunk_size.log", UPDOWN_CHUNK_SIZE / 3, 10));
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/one_chunk_size.log", UPDOWN_CHUNK_SIZE, 11));
    EXPECT_TRUE(BOFWEBRPC::BofWebApp::S_CreateTestFile(Path_S + "/more_than_one_chunk_size.log", (UPDOWN_CHUNK_SIZE * 3000) + 9876, 12));

    Path_S = mWebServerParam_X.RootDir_S + "/update/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, Path_S), BOF_ERR_NO_ERROR);

    Path_S = mWebServerParam_X.RootDir_S + "/version/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, Path_S), BOF_ERR_NO_ERROR);

    mWebServerParam_X.WebAppParam_X.AppName_S = "web-srv";

    std::shared_ptr<BOF::IBofLoggerFactory> psLoggerFactory = std::make_shared<BOF::BofBasicLoggerFactory>(true, false, true, (8 * 1024 * 1024), ".");
    if (psLoggerFactory)
    {
      mpuWebServer = std::make_unique<UtWebServer>(psLoggerFactory, mWebServerParam_X);
      if (mpuWebServer)
      {
        mWebClientParam_X.IsHttpsClient_B = _UseHttps_B;
        mWebClientParam_X.CertificateAuthorityPath_S = "";
        mWebClientParam_X.DisableServerCertificateVerification_B = true; // In ut env
        mWebClientParam_X.ReadTimeoutInMs_U32 = 1000;
        mWebClientParam_X.WriteTimeoutInMs_U32 = 1000;
        mWebClientParam_X.WebAppParam_X.AppName_S = "web-clt";
        mpuWebClient = std::make_unique<UtWebClient>(psLoggerFactory, mWebClientParam_X);
        if (mpuWebClient)
        {
          Rts_B = true;
        }
      }
    }
    return Rts_B;
  }
  bool DestroyClientAndServer()
  {
    bool Rts_B = true;
    mpuWebClient.reset(nullptr);
    mpuWebServer.reset(nullptr);
    return Rts_B;
  }
  void TearDown() override
  {
    // Cleanup common resources after each test
  }

public:
  BOFWEBRPC::BOF_WEB_SERVER_PARAM mWebServerParam_X;
  BOFWEBRPC::BOF_WEB_CLIENT_PARAM mWebClientParam_X;
  std::unique_ptr<UtWebServer> mpuWebServer = nullptr;
  std::unique_ptr<UtWebClient> mpuWebClient = nullptr;
};

TEST_F(bofwebapp_tests, StartConnectGetStopDisconnectHttp)
{
  BOFWEBRPC::BOF_WEB_RESULT Res;
  BOFWEBRPC::BOF_WEB_HEADER HeaderCollection_X;

  EXPECT_TRUE(CreateClientAndServer(false));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);

  EXPECT_TRUE(mpuWebServer->IsRunning());

  Res = mpuWebClient->Get("/", false, false, HeaderCollection_X);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  // To be tested
  //     EXPECT_TRUE(SetMountPoint("/www", mWebServerParam_X.RootDir_S));
  // Res = mpuWebClient->Head("/www/log/more_than_one_chunk_size.log", false, false);
  // EXPECT_FALSE(Res == nullptr);
  // Res = mpuWebClient->Get("/www/log/more_than_one_chunk_size.log", false, false);
  // EXPECT_FALSE(Res == nullptr);
  // auto a = Res->body.size();
  Res = mpuWebClient->Get("/", false, false, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::Found_302); // Redirect

  Res = mpuWebClient->Get("/DontExist", false, false, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::NotFound_404);

  Res = mpuWebClient->Get("/hi", true, true, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);

  Res = mpuWebClient->Get("/numbers/1234", false, false, HeaderCollection_X);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mNumber_S.c_str(), "1234");

  Res = mpuWebClient->Get("/users/bha", false, false, HeaderCollection_X);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mUserId_S.c_str(), "bha");

  EXPECT_TRUE(mpuWebClient->Disconnect());

  Res = mpuWebClient->Get("/hi", true, true, HeaderCollection_X);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebServer->Stop());

  EXPECT_FALSE(mpuWebServer->IsRunning());

  EXPECT_EQ(mpuWebServer->mOnFileRequest_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnException_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 5);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 5);
  EXPECT_EQ(mpuWebServer->mOnExpect100Continue_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetNumber_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetUser_U32, 1);

  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 4);
  EXPECT_TRUE(DestroyClientAndServer());
}
TEST_F(bofwebapp_tests, StartConnectGetStopDisconnectHttps)
{
  BOFWEBRPC::BOF_WEB_RESULT Res;
  BOFWEBRPC::BOF_WEB_HEADER HeaderCollection_X;

  EXPECT_TRUE(CreateClientAndServer(true));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);

  EXPECT_TRUE(mpuWebServer->IsRunning());

  Res = mpuWebClient->Get("/", false, false, HeaderCollection_X);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));

  Res = mpuWebClient->Get("/", false, false, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::Found_302); // Redirect

  Res = mpuWebClient->Get("/DontExist", false, false, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::NotFound_404);

  Res = mpuWebClient->Get("/hi", true, true, HeaderCollection_X);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);

  Res = mpuWebClient->Get("/numbers/1234", false, false, HeaderCollection_X);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mNumber_S.c_str(), "1234");

  Res = mpuWebClient->Get("/users/bha", false, false, HeaderCollection_X);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mUserId_S.c_str(), "bha");

  EXPECT_TRUE(mpuWebClient->Disconnect());

  Res = mpuWebClient->Get("/hi", true, true, HeaderCollection_X);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebServer->Stop());

  EXPECT_FALSE(mpuWebServer->IsRunning());

  EXPECT_EQ(mpuWebServer->mOnFileRequest_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnException_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 7);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 7);
  EXPECT_EQ(mpuWebServer->mOnExpect100Continue_U32, 0);
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetNumber_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetUser_U32, 1);

  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 6);
  EXPECT_TRUE(DestroyClientAndServer());
}

TEST_F(bofwebapp_tests, Download)
{
  uint64_t FileSize1_U64, FileSize2_U64;

  EXPECT_TRUE(CreateClientAndServer(false));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));

  EXPECT_TRUE(mpuWebClient->Download("/download/log/less_than_one_chunk_size.log", mWebServerParam_X.RootDir_S + "/../local/down_less_than_one_chunk_size.log",
                                     false, true, UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/log/less_than_one_chunk_size.log");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/down_less_than_one_chunk_size.log");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Download("/download/log/one_chunk_size.log", mWebServerParam_X.RootDir_S + "/../local/down_one_chunk_size.log", false, true,
                                     UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/log/one_chunk_size.log");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/down_one_chunk_size.log");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Download("/download/log/more_than_one_chunk_size.log", mWebServerParam_X.RootDir_S + "/../local/down_more_than_one_chunk_size.log",
                                     false, true, UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/log/more_than_one_chunk_size.log");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/down_more_than_one_chunk_size.log");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Disconnect());
  EXPECT_TRUE(mpuWebServer->Stop());
  EXPECT_TRUE(DestroyClientAndServer());
}

TEST_F(bofwebapp_tests, Upload)
{
  uint64_t FileSize1_U64, FileSize2_U64;

  EXPECT_TRUE(CreateClientAndServer(false));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_TRUE(mpuWebClient->Upload(mWebServerParam_X.RootDir_S + "/../local/less_than_one_chunk_size.bin", "/upload/update/up_less_than_one_chunk_size.bin",
                                   false, true, UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/less_than_one_chunk_size.bin");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/update/up_less_than_one_chunk_size.bin");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Upload(mWebServerParam_X.RootDir_S + "/../local/one_chunk_size.bin", "/upload/update/up_one_chunk_size.bin", false, true,
                                   UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/one_chunk_size.bin");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/update/up_one_chunk_size.bin");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Upload(mWebServerParam_X.RootDir_S + "/../local/more_than_one_chunk_size.bin", "/upload/update/up_more_than_one_chunk_size.bin",
                                   false, true, UPDOWN_CHUNK_SIZE));
  FileSize1_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/../local/more_than_one_chunk_size.bin");
  FileSize2_U64 = BOF::Bof_GetFileSize(mWebServerParam_X.RootDir_S + "/update/up_more_than_one_chunk_size.bin");
  EXPECT_EQ(FileSize1_U64, FileSize2_U64);

  EXPECT_TRUE(mpuWebClient->Disconnect());
  EXPECT_TRUE(mpuWebServer->Stop());
  EXPECT_TRUE(DestroyClientAndServer());
}

// github.com/yhirose/cpp-httplib/blob/master/example/server.cc
