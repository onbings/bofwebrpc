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

class AppSrvRest : public BOFWEBRPC::BofWebServer
{
public:
  AppSrvRest(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOFWEBRPC::BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
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
    EXPECT_TRUE(SetMountPoint("/www", "/tmp/www"));
    EXPECT_TRUE(SetMountPoint("/abc", "/tmp"));
    EXPECT_TRUE(RemoveMountPoint("/abc"));
    EXPECT_TRUE(SetFileExtensionAndMimetypeMapping("bha", "mime/bha"));

    EXPECT_TRUE(Start("10.129.170.29", 8090));
    EXPECT_TRUE(IsRunning());
  }
  ~AppSrvRest()
  {
  }

private:
  void V_OnFileRequest(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnFileRequest\n");
  }
  void V_OnError(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnError\n");
  }
  void V_OnException(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes, std::exception_ptr ep) override
  {
    auto fmt = "<h1>Error 500</h1><p>%s</p>";
    char pBuffer_c[0x100];
    printf("V_OnException\n");
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
    return false;
  }
  void V_OnPostRouting(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnPostRouting\n");
  }
  // By default, the server sends a 100 Continue response for an Expect: 100-continue header.
  BOFWEBRPC::BOF_WEB_STATUS V_OnExpect100Continue(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes) override
  {
    printf("V_OnExpect100Continue\n");
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
    int OptVal_i = 1; // Enable the option
    if (setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, &OptVal_i, sizeof(OptVal_i)) == -1)
    {
      //      perror("setsockopt SO_REUSEADDR failed");
    }
  }

  void GetRoot(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    printf("=================redirect to hi================\n");
    _rRes.set_redirect("/hi");
  }
  void GetHi(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    static int S_Id_i = 0;
    char pRes_c[0x1000];
    int Len_i = sprintf(pRes_c, "Hello %d\n", ++S_Id_i);
    printf("=================gethi================%d:%s\n", Len_i, pRes_c);
    _rRes.set_content(pRes_c, Len_i, "text/plain");
  }

  // Match the request path against a regular expression and extract its captures
  void GetNumber(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    auto Number = _rReq.matches[1];
    _rRes.set_content(Number, "text/plain");
  }

  // Capture the second segment of the request path as "id" path param
  void GetUser(const BOFWEBRPC::BOF_WEB_REQUEST &_rReq, BOFWEBRPC::BOF_WEB_RESPONSE &_rRes)
  {
    auto UserId = _rReq.path_params.at("id");
    _rRes.set_content(UserId, "text/plain");
  }
};

class bofwebapp_tests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    BOFWEBRPC::BOF_WEB_SERVER_PARAM WebServerParam_X;
    BOFWEBRPC::BOF_WEB_CLIENT_PARAM WebClientParam_X;

    WebServerParam_X.ServerStartStopTimeoutInMs_U32 = 5000;

    // WebServerParam_X.CertificatePath_S = "/home/bha/pro/evs-hwfw-sb-xts/cert.pem";
    // WebServerParam_X.PrivateKeyPath_S = "/home/bha/pro/evs-hwfw-sb-xts/key.pem";
    WebServerParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";

    std::shared_ptr<BOF::IBofLoggerFactory> psLoggerFactory = std::make_shared<BOF::BofBasicLoggerFactory>(true, false, true, ".");
    mpuAppSrvRest = std::make_unique<AppSrvRest>(psLoggerFactory, WebServerParam_X);

    // WebClientParam_X.CertificatePath_S = "/home/bha/pro/evs-hwfw-sb-xts/cert.pem";
    // WebClientParam_X.PrivateKeyPath_S = "/home/bha/pro/evs-hwfw-sb-xts/key.pem";
    WebClientParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
    //    mpuWebClient = std::make_unique<BOFWEBRPC::BofWebClient>(psLoggerFactory, WebClientParam_X);
  }

  void TearDown() override
  {
    // Cleanup common resources after each test
  }

public:
  std::unique_ptr<AppSrvRest> mpuAppSrvRest = nullptr;
  //  std::unique_ptr<BOFWEBRPC::BofWebClient> mpuWebClient = nullptr;
};

TEST_F(bofwebapp_tests, Test)
{
  EXPECT_TRUE(mpuAppSrvRest->Start("10.129.170.29", 8090));
  EXPECT_TRUE(mpuAppSrvRest->IsRunning());

  httplib::Result Res;
  // Res = f();

  // BOFWEBRPC::BofWeb b;
  // Res = b.G();

  // std::unique_ptr<BOFWEBRPC::BofWeb> puWeb;
  // puWeb = std::make_unique<BOFWEBRPC::BofWeb>();
  // Res = puWeb->G();

  std::unique_ptr<BOFWEBRPC::BofWebClient> puWebClient;
  BOFWEBRPC::BOF_WEB_CLIENT_PARAM WebClientParam_X;
  WebClientParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
  puWebClient = std::make_unique<BOFWEBRPC::BofWebClient>(nullptr, WebClientParam_X);
  EXPECT_TRUE(puWebClient->Connect(2000, "10.129.170.29", 8090));
  //  Res = puWebClient->G();
  Res = puWebClient->Get("/hi", true, true);
  //    std::unique_ptr<httplib::Client> mpuWebClientProxy;
  //    mpuWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090); //_rIpAddress_S, _Port_U16);
  //    printf("call in client G %p (/hi)\n", mpuWebClientProxy.get());
  //    Res = mpuWebClientProxy->Get("/hi");

  //  httplib::Client cli("10.129.170.29", 8090);
  //  auto Res = cli.Get("/hi");
  // BOF::Bof_MsSleep(99999999);

  // EXPECT_TRUE(mpuWebClient->Connect("10.129.170.29", 8090));

  // HTTPS
  // httplib::Client WebClient("https://cpp-httplib-server.yhirose.repl.co");
  // httplib::Result Res = mpuWebClient->Get("/hi", false, false);
  // BOF::Bof_MsSleep(99999999);

  // auto res = WebClient.Get("/hi");
  if (Res)
  {
    if (Res->status == BOFWEBRPC::BOF_WEB_STATUS::OK_200)
    {
      printf("Status: %d\n", Res->status);
      printf("Header: %s\n", Res->get_header_value("Content-Type").c_str());
      printf("Body: %s\n", Res->body.c_str());
    }
  }
  else
  {
    httplib::Error Err_E = Res.error();
    printf("HTTP error: %s\n", httplib::to_string(Err_E).c_str());
  }

  Res = puWebClient->Get("/hi", false, false);
  if (Res)
  {
    printf("Ok again\n");
  }
  else
  {
    printf("BAD !!!\n");
  }

  printf("Sleep Forever\n");
  BOF::Bof_MsSleep(99999999);

  EXPECT_TRUE(mpuAppSrvRest->Stop());
  EXPECT_FALSE(mpuAppSrvRest->IsRunning());

  BOF::Bof_MsSleep(99999999);
  EXPECT_TRUE(true);
}

// github.com/yhirose/cpp-httplib/blob/master/example/server.cc
