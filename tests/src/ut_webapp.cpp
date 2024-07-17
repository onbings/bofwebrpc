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
// #include "httplib.h"

class AppSrvRest : public BOFWEBRPC::BofWebServer
{
public:
  AppSrvRest(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOFWEBRPC::BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
      : BOFWEBRPC::BofWebServer(_psLoggerFactory, _rWebServerParam_X)
  {
  }
  ~AppSrvRest()
  {
  }
  void Logger(const httplib::Request &_rReq_X, const httplib::Response &_rRes_X)
  {
    printf("Logger: %s\n", LogRequest(_rReq_X, _rRes_X).c_str());
  }

private:
  void V_OnConfigUpdate(const json &_rConfig) override
  {
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
    WebServerParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;

    std::shared_ptr<BOF::IBofLoggerFactory> psLoggerFactory = std::make_shared<BOF::BofBasicLoggerFactory>(true, false, true, ".");
    mpuAppSrvRest = std::make_unique<AppSrvRest>(psLoggerFactory, WebServerParam_X);

    // WebClientParam_X.CertificatePath_S = "/home/bha/pro/evs-hwfw-sb-xts/cert.pem";
    // WebClientParam_X.PrivateKeyPath_S = "/home/bha/pro/evs-hwfw-sb-xts/key.pem";
    WebClientParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
    WebClientParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;
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

void GetRoot(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  printf("=================redirect to hi================\n");
  _rRes_X.set_redirect("/hi");
}
void GetHi(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  // printf("=================gethi================\n");
  // _rRes_X.set_content("Hello World!\n", "text/plain");

  static int S_Id_i = 0;
  char pRes_c[0x1000];
  int Len_i = sprintf(pRes_c, "Hello %d\n", ++S_Id_i);
  printf("=================gethi================%d:%s\n", Len_i, pRes_c);
  _rRes_X.set_content(pRes_c, Len_i, "text/plain");
}

// Match the request path against a regular expression and extract its captures
void GetNumber(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  auto Number = _rReq_X.matches[1];
  _rRes_X.set_content(Number, "text/plain");
}

// Capture the second segment of the request path as "id" path param
void GetUser(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  auto UserId = _rReq_X.path_params.at("id");
  _rRes_X.set_content(UserId, "text/plain");
}

// Extract values from HTTP headers and URL query params
void GetBodyHeaderParam(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  if (_rReq_X.has_header("Content-Length"))
  {
    auto Val = _rReq_X.get_header_value("Content-Length");
  }
  if (_rReq_X.has_param("key"))
  {
    auto Val = _rReq_X.get_param_value("key");
  }
  _rRes_X.set_content(_rReq_X.body, "text/plain");
}
void FileRequestHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  _rRes_X.set_content("FileRequestHandler\n", "text/plain");
}
void ErrorHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  _rRes_X.set_content("ErrorHandler\n", "text/plain");
  const char *pFmt_c = "<p>ErrorHandler: Error Status: <span style='color:red;'>%d</span></p>";
  char pBuffer_c[0x4000];
  int Len_i = snprintf(pBuffer_c, sizeof(pBuffer_c), pFmt_c, _rRes_X.status);
}
void ExceptionHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X, std::exception_ptr ep)
{
  _rRes_X.set_content("ExceptionHandler\n", "text/plain");
}
httplib::Server::HandlerResponse PreRoutingHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  httplib::Server::HandlerResponse Rts = httplib::Server::HandlerResponse::Unhandled;

  if (_rReq_X.path == "/hello")
  {
    _rRes_X.set_content("world", "text/html");
    Rts = httplib::Server::HandlerResponse::Handled;
  }
  return Rts;
  //  _rRes_X.set_content("PreRoutingHandler\n", "text/plain");
}
void PostRoutingHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  _rRes_X.set_header("ADDITIONAL_HEADER", "value");
  //_rRes_X.set_content("PostRoutingHandler\n", "text/plain");
}
// By default, the server sends a 100 Continue response for an Expect: 100-continue header.
httplib::StatusCode Expect100ContinueHandler(const httplib::Request &_rReq_X, httplib::Response &_rRes_X)
{
  // Send a '417 Expectation Failed' response.
  httplib::StatusCode Rts = httplib::StatusCode::ExpectationFailed_417;
  // Send a final status httplib::detail::without reading the message body.
  Rts = httplib::StatusCode::Unauthorized_401;
  _rRes_X.status = Rts;

  // Custom logic to determine if the request should be continued
  // For example, check if the Content-Length is acceptable
  auto it = _rReq_X.headers.find("Content-Length");
  if (it != _rReq_X.headers.end())
  {
    size_t ContentLength = std::stoull(it->second);
    if (ContentLength > 1024 * 1024)
    {                                                // Arbitrary limit of 1MB
      Rts = httplib::StatusCode::LengthRequired_411; // Reject requests larger than 1MB
    }
  }
  Rts = httplib::StatusCode::OK_200; // Continue with the request
  return Rts;
  //_rRes_X.set_content("Expect100ContinueHandler\n", "text/plain");
}
/*
void Logger(const httplib::Request &_rReq_X, const httplib::Response &_rRes_X)
{
  printf("Logger: %s\n", LogRequest(_rReq_X, _rRes_X).c_str());
}
*/
void SocketOptions(socket_t _Sock)
{
  printf("Set SocketOptions of %d\n", _Sock);
  int Yes_i = 1;
  if (setsockopt(_Sock, SOL_SOCKET, SO_REUSEADDR, &Yes_i, sizeof(Yes_i)) == -1)
  {
    perror("setsockopt");
  }
}
ssize_t HeaderWriter(httplib::Stream &_rStream, httplib::Headers &_rHeaderCollection)
{
  ssize_t Rts = 0;
  int Len_i;

  for (const auto &rHeader : _rHeaderCollection)
  {
    Len_i = _rStream.write_format("%s: %s\r\n", rHeader.first.c_str(), rHeader.second.c_str());
    if (Len_i < 0)
    {
      return Len_i;
    }
    Rts += Len_i;
  }
  Len_i = _rStream.write("\r\n");
  if (Len_i < 0)
  {
    return Len_i;
  }
  Rts += Len_i;
  return Rts;

  // return printf("HeaderWriter\n");
  //  Example: Add a custom header
  Rts += _rStream.write("X-Custom-Header: Value\r\n");

  // Write the original headers
  for (const auto &rHeader : _rHeaderCollection)
  {
    printf("%s: %s\r\n", rHeader.first.c_str(), rHeader.second.c_str());
    Rts += _rStream.write(rHeader.first.c_str());
    Rts += _rStream.write(": ");
    Rts += _rStream.write(rHeader.second.c_str());
    Rts += _rStream.write("\r\n");
  }
  printf("rts is %ld\r\n", Rts);
  return Rts;
}
httplib::Result f()
{
  std::unique_ptr<httplib::Client> mpuWebClientProxy;
  // Connect("10.129.170.29", 8090);
  mpuWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090); //_rIpAddress_S, _Port_U16);
  printf("call in BofWeb f %p (/hi)\n", mpuWebClientProxy.get());
  return mpuWebClientProxy->Get("/hi");
}
TEST_F(bofwebapp_tests, Test)
{
  std::multimap<std::string, std::string, httplib::detail::ci> HeaderCollection;

  EXPECT_TRUE(mpuAppSrvRest->SetSocketOptions(SocketOptions));

  // HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpc/1.0.0"));
  //  cli.set_default_headers({{"Accept-Encoding", "gzip, deflate"}});
  // EXPECT_TRUE(mpuAppSrvRest->SetDefaultHeaders(HeaderCollection));

  EXPECT_TRUE(mpuAppSrvRest->Get("/", GetRoot));
  EXPECT_TRUE(mpuAppSrvRest->Get("/hi", GetHi));

#if 1
  // Match the request path against a regular expression and extract its captures
  EXPECT_TRUE(mpuAppSrvRest->Get(R"(/numbers/(\d+))", GetNumber));
  // Capture the second segment of the request path as "id" path param
  EXPECT_TRUE(mpuAppSrvRest->Get("/users/:id", GetUser));
  // Extract values from HTTP headers and URL query params
  EXPECT_TRUE(mpuAppSrvRest->Get("/body-header-param", GetBodyHeaderParam));
  EXPECT_TRUE(mpuAppSrvRest->SetMountPoint("/www", "/tmp/www"));
  EXPECT_TRUE(mpuAppSrvRest->SetMountPoint("/abc", "/tmp"));
  EXPECT_TRUE(mpuAppSrvRest->RemoveMountPoint("/abc"));
  EXPECT_TRUE(mpuAppSrvRest->SetFileExtensionAndMimetypeMapping("bha", "mime/bha"));
  EXPECT_TRUE(mpuAppSrvRest->SetDefaultFileMimetype("text/plain"));
  EXPECT_TRUE(mpuAppSrvRest->SetFileRequestHandler(FileRequestHandler));
  EXPECT_TRUE(mpuAppSrvRest->SetErrorHandler(ErrorHandler));
  EXPECT_TRUE(mpuAppSrvRest->SetExceptionHandler(ExceptionHandler));
  EXPECT_TRUE(mpuAppSrvRest->SetPreRoutingHandler(PreRoutingHandler));
  EXPECT_TRUE(mpuAppSrvRest->SetPostRoutingHandler(PostRoutingHandler));
  EXPECT_TRUE(mpuAppSrvRest->SetExpect100ContinueHandler(Expect100ContinueHandler));
  mpuAppSrvRest->SetLogger([this](const httplib::Request &_rReq_X, const httplib::Response &_rRes_X) { mpuAppSrvRest->Logger(_rReq_X, _rRes_X); });

  //  EXPECT_TRUE(mpuAppSrvRest->SetLogger(AppSrvRest::Logger));
  EXPECT_TRUE(mpuAppSrvRest->SetAddressFamily(AF_UNSPEC));
  EXPECT_TRUE(mpuAppSrvRest->SetTcpNoDelay(true));

  EXPECT_TRUE(mpuAppSrvRest->SetHeaderWriter(HeaderWriter));
  EXPECT_TRUE(mpuAppSrvRest->SetKeepAliveMaxCount(5));
  EXPECT_TRUE(mpuAppSrvRest->SetKeepAliveTimeout(2500));
  EXPECT_TRUE(mpuAppSrvRest->SetReadTimeout(20000));
  EXPECT_TRUE(mpuAppSrvRest->SetWriteTimeout(20000));
  EXPECT_TRUE(mpuAppSrvRest->SetIdleInterval(50000));
  EXPECT_TRUE(mpuAppSrvRest->SetPayloadMaxLength(0x100000));
#endif
  // EXPECT_TRUE(mpuAppSrvRest->Start("", 0));
  EXPECT_TRUE(mpuAppSrvRest->Start("10.129.170.29", 8090));
  EXPECT_TRUE(mpuAppSrvRest->IsRunning());

  httplib::Result Res;
#if 1
#if 1
  // Res = f();

  // BOFWEBRPC::BofWeb b;
  // Res = b.G();

  // std::unique_ptr<BOFWEBRPC::BofWeb> puWeb;
  // puWeb = std::make_unique<BOFWEBRPC::BofWeb>();
  // Res = puWeb->G();

  std::unique_ptr<BOFWEBRPC::BofWebClient> puWebClient;
  BOFWEBRPC::BOF_WEB_CLIENT_PARAM WebClientParam_X;
  WebClientParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
  WebClientParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;
  puWebClient = std::make_unique<BOFWEBRPC::BofWebClient>(nullptr, WebClientParam_X);
  EXPECT_TRUE(puWebClient->Connect("10.129.170.29", 8090));
  std::string Url_S = "/hi";

  //  Res = puWebClient->G();
  Res = puWebClient->Get("/hi", true, false);
  //    std::unique_ptr<httplib::Client> mpuWebClientProxy;
  //    mpuWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090); //_rIpAddress_S, _Port_U16);
  //    printf("call in client G %p (/hi)\n", mpuWebClientProxy.get());
  //    Res = mpuWebClientProxy->Get("/hi");
#else
  std::unique_ptr<BOFWEBRPC::BofWebClient> puWebClient;
  BOFWEBRPC::BOF_WEB_CLIENT_PARAM WebClientParam_X;
  WebClientParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
  WebClientParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;
  puWebClient = std::make_unique<BOFWEBRPC::BofWebClient>(nullptr, WebClientParam_X);
  // Res = puWebClient->G();
  EXPECT_TRUE(puWebClient->Connect("10.129.170.29", 8090));
  Res = puWebClient->Get("/hi", false, false);

  printf("111111111111\n");

  // Res = puWebClient->Get("/hi", false, false);
  // printf("222222222222\n");
  // Res = puWebClient->Get("/hi", false, false);
  // printf("333333333333\n");

  // printf("call in ut %p (%s)\n", puWebClient->GetPtr(), Url_S.c_str());
  // Res = puWebClient->GetPtr()->Get(Url_S);
  // printf("44444444444\n");
#endif
#else
  std::unique_ptr<httplib::Client> puWebClientProxy;
  puWebClientProxy = std::make_unique<httplib::Client>("10.129.170.29", 8090);
  puWebClientProxy->set_default_headers({{"User-Agent", "BofWebRpcAgent/1.0"}});
  Res = puWebClientProxy->Get("/hi");
#endif
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
    if (Res->status == httplib::StatusCode::OK_200)
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
  BOF::Bof_MsSleep(99999999);

  BOF::Bof_MsSleep(1000);
  EXPECT_TRUE(mpuAppSrvRest->Stop());
  EXPECT_FALSE(mpuAppSrvRest->IsRunning());

  BOF::Bof_MsSleep(99999999);
  EXPECT_TRUE(true);
}

// github.com/yhirose/cpp-httplib/blob/master/example/server.cc
