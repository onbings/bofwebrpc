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
  uint32_t mGetDownload_U32 = 0;
  uint32_t mPutUpload_U32 = 0;

  std::string mNumber_S;
  std::string mUserId_S;
  std::string mDirDownload_S;
  std::string mFileDownload_S;
  std::string mDirUpload_S;
  std::string mFileUpload_S;

private:
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

  void GetDownload(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    auto pIt = _rReq.path_params.find("dir");
    if (pIt != _rReq.path_params.end())
    {
      mDirDownload_S = pIt->second;
    }
    pIt = _rReq.path_params.find("file");
    if (pIt != _rReq.path_params.end())
    {
      mFileDownload_S = pIt->second;
    }
    printf("=================GetDownload================%s/%s\n", mDirDownload_S.c_str(), mFileDownload_S.c_str());
    mGetDownload_U32++;
  }
  void PostUpload(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    auto pIt = _rReq.path_params.find("dir");
    if (pIt != _rReq.path_params.end())
    {
      mDirUpload_S = pIt->second;
    }
    pIt = _rReq.path_params.find("file");
    if (pIt != _rReq.path_params.end())
    {
      mFileUpload_S = pIt->second;
    }
    printf("=================PostUpload================%s/%s\n", mDirUpload_S.c_str(), mFileUpload_S.c_str());
    mPutUpload_U32++;
  }
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
  bool CreateFile(const std::string &_rPath_S, uint32_t _SizeInByte_U32, uint8_t _ValueStart_U32)
  {
    bool Rts_B = false;
    FILE *pIo_X;
    uint8_t *pBuffer_U8;
    uint32_t i_U32;

    pIo_X = fopen(_rPath_S.c_str(), "wb");
    if (pIo_X != nullptr)
    {
      pBuffer_U8 = new uint8_t[_SizeInByte_U32];
      if (pBuffer_U8)
      {
        for (i_U32 = 0; i_U32 < _SizeInByte_U32; i_U32++)
        {
          pBuffer_U8[i_U32] = _ValueStart_U32++;
        }
        if (fwrite(pBuffer_U8, _SizeInByte_U32, 1, pIo_X) == 1)
        {
          Rts_B = true;
        }

        delete[] pBuffer_U8;
      }
      fclose(pIo_X);
    }
    return Rts_B;
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
    EXPECT_TRUE(CreateFile(Path_S + "/MulticamDeveloper_21.00.00.83879.tar.gz", UPDOWN_CHUNK_SIZE * 100, 0));
    EXPECT_TRUE(CreateFile(Path_S + "/less_than_one_chunk_size.bin", UPDOWN_CHUNK_SIZE / 2, 1));
    EXPECT_TRUE(CreateFile(Path_S + "/one_chunk_size.bin", UPDOWN_CHUNK_SIZE, 2));
    EXPECT_TRUE(CreateFile(Path_S + "/more_than_one_chunk_size.bin", (UPDOWN_CHUNK_SIZE * 4) + 5678, 4));

    mWebServerParam_X.RootDir_S += "/www/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, mWebServerParam_X.RootDir_S), BOF_ERR_NO_ERROR);
    EXPECT_EQ(BOF::Bof_CleanupDirectory(true, mWebServerParam_X.RootDir_S, false), BOF_ERR_NO_ERROR);

    Path_S = mWebServerParam_X.RootDir_S + "/log/";
    EXPECT_EQ(BOF::Bof_CreateDirectory(BOF::BOF_FILE_PERMISSION_ALL_FOR_OWNER, Path_S), BOF_ERR_NO_ERROR);
    EXPECT_TRUE(CreateFile(Path_S + "/less_than_one_chunk_size.log", UPDOWN_CHUNK_SIZE / 3, 10));
    EXPECT_TRUE(CreateFile(Path_S + "/one_chunk_size.log", UPDOWN_CHUNK_SIZE, 11));
    EXPECT_TRUE(CreateFile(Path_S + "/more_than_one_chunk_size.log", (UPDOWN_CHUNK_SIZE * 3000) + 9876, 12));

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

  EXPECT_TRUE(CreateClientAndServer(false));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);

  EXPECT_TRUE(mpuWebServer->IsRunning());

  Res = mpuWebClient->Get("/", false, false);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  // To be tested
  //     EXPECT_TRUE(SetMountPoint("/www", mWebServerParam_X.RootDir_S));
  // Res = mpuWebClient->Head("/www/log/more_than_one_chunk_size.log", false, false);
  // EXPECT_FALSE(Res == nullptr);
  // Res = mpuWebClient->Get("/www/log/more_than_one_chunk_size.log", false, false);
  // EXPECT_FALSE(Res == nullptr);
  // auto a = Res->body.size();
  Res = mpuWebClient->Get("/", false, false);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::Found_302); // Redirect

  Res = mpuWebClient->Get("/DontExist", false, false);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::NotFound_404);

  Res = mpuWebClient->Get("/hi", true, true);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);

  Res = mpuWebClient->Get("/numbers/1234", false, false);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mNumber_S.c_str(), "1234");

  Res = mpuWebClient->Get("/users/bha", false, false);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mUserId_S.c_str(), "bha");
  /*
    Res = mpuWebClient->Get("/download/log/small_log.log", false, false);
    EXPECT_FALSE(Res == nullptr);
    EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
    EXPECT_STREQ(mpuWebServer->mDirDownload_S.c_str(), "log");
    EXPECT_STREQ(mpuWebServer->mFileDownload_S.c_str(), "small_log.log");

    Res = mpuWebClient->Post("/upload/upload/small_file.bin", false, false);
    EXPECT_FALSE(Res == nullptr);
    EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
    EXPECT_STREQ(mpuWebServer->mDirUpload_S.c_str(), "upload");
    EXPECT_STREQ(mpuWebServer->mFileUpload_S.c_str(), "small_file.bin");
  */
  EXPECT_TRUE(mpuWebClient->Disconnect());

  Res = mpuWebClient->Get("/hi", true, true);
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
  EXPECT_EQ(mpuWebServer->mGetDownload_U32, 1);
  EXPECT_EQ(mpuWebServer->mPutUpload_U32, 1);

  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 6);
  EXPECT_TRUE(DestroyClientAndServer());
}
TEST_F(bofwebapp_tests, StartConnectGetStopDisconnectHttps)
{
  BOFWEBRPC::BOF_WEB_RESULT Res;

  EXPECT_TRUE(CreateClientAndServer(true));
  EXPECT_TRUE(mpuWebServer->Start(WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));
  EXPECT_EQ(mpuWebServer->mOnSetSocketOption_U32, 1);

  EXPECT_TRUE(mpuWebServer->IsRunning());

  Res = mpuWebClient->Get("/", false, false);
  EXPECT_TRUE(Res == nullptr);

  EXPECT_TRUE(mpuWebClient->Connect(mWebClientParam_X.ReadTimeoutInMs_U32, WEB_SERVER_IP_ADDRESS, WEB_SERVER_PORT));

  Res = mpuWebClient->Get("/", false, false);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 1);
  EXPECT_EQ(mpuWebServer->mGetRoot_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::Found_302); // Redirect

  Res = mpuWebClient->Get("/DontExist", false, false);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 2);
  EXPECT_EQ(mpuWebServer->mOnError_U32, 1);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::NotFound_404);

  Res = mpuWebClient->Get("/hi", true, true);
  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPreRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mOnPostRouting_U32, 3);
  EXPECT_EQ(mpuWebServer->mGetHi_U32, 1);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);

  Res = mpuWebClient->Get("/numbers/1234", false, false);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mNumber_S.c_str(), "1234");

  Res = mpuWebClient->Get("/users/bha", false, false);
  EXPECT_FALSE(Res == nullptr);
  EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
  EXPECT_STREQ(mpuWebServer->mUserId_S.c_str(), "bha");
  /*
    Res = mpuWebClient->Get("/download/log/small_log.log", false, false);
    EXPECT_FALSE(Res == nullptr);
    EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
    EXPECT_STREQ(mpuWebServer->mDirDownload_S.c_str(), "log");
    EXPECT_STREQ(mpuWebServer->mFileDownload_S.c_str(), "small_log.log");

    Res = mpuWebClient->Post("/upload/upload/small_file.bin", false, false);
    EXPECT_FALSE(Res == nullptr);
    EXPECT_EQ(Res->status, BOFWEBRPC::BOF_WEB_STATUS::OK_200);
    EXPECT_STREQ(mpuWebServer->mDirUpload_S.c_str(), "upload");
    EXPECT_STREQ(mpuWebServer->mFileUpload_S.c_str(), "small_file.bin");
  */
  EXPECT_TRUE(mpuWebClient->Disconnect());

  Res = mpuWebClient->Get("/hi", true, true);
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
  EXPECT_EQ(mpuWebServer->mGetDownload_U32, 1);
  EXPECT_EQ(mpuWebServer->mPutUpload_U32, 1);

  EXPECT_EQ(mpuWebClient->mOnSetSocketOption_U32, 6);
  EXPECT_TRUE(DestroyClientAndServer());
}

TEST_F(bofwebapp_tests, Download)
{
  EXPECT_TRUE(CreateClientAndServer(false));
  EXPECT_TRUE(mpuWebClient->Download("/download/log/less_than_one_chunk_size.log", mWebServerParam_X.RootDir_S + "/../local/down_less_than_one_chunk_size.log",
                                     false, true, UPDOWN_CHUNK_SIZE));
  //  EXPECT_TRUE(mpuWebClient->Download("/download/log/one_chunk_size.log", false, true, UPDOWN_CHUNK_SIZE));
  //  EXPECT_TRUE(mpuWebClient->Download("/download/log/more_than_one_chunk_size.log", false, true, UPDOWN_CHUNK_SIZE));
  EXPECT_TRUE(DestroyClientAndServer());
}
// github.com/yhirose/cpp-httplib/blob/master/example/server.cc
