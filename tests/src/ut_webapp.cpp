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

    WebServerParam_X.ServerStartStopTimeoutInMs_U32 = 2000;

    WebServerParam_X.CertificatePath_S = "";
    WebServerParam_X.PrivateKeyPath_S = "";
    WebServerParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
    WebServerParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;

    std::shared_ptr<BOF::IBofLoggerFactory> psLoggerFactory = std::make_shared<BOF::BofBasicLoggerFactory>(true, false, true, ".");
    mpuAppSrvRest = std::make_unique<AppSrvRest>(psLoggerFactory, WebServerParam_X);
  }

  void TearDown() override
  {
    // Cleanup common resources after each test
  }

public:
  std::unique_ptr<AppSrvRest> mpuAppSrvRest = nullptr;
};

TEST_F(bofwebapp_tests, Test)
{
  mpuAppSrvRest->Get("/", [=](const httplib::Request & /*req*/, httplib::Response &_rRes) { _rRes.set_redirect("/hi"); });
  mpuAppSrvRest->Start();
  mpuAppSrvRest->Get("/hi", [](const httplib::Request & /*req*/, httplib::Response &_rRes) { _rRes.set_content("Hello World!\n", "text/plain"); });
#if 0
  mpuAppSrvRest->set_expect_100_continue_handler([](const httplib::Request &req) -> bool {
    // Custom logic to determine if the request should be continued
    // For example, check if the Content-Length is acceptable
    auto it = req.headers.find("Content-Length");
    if (it != req.headers.end())
    {
      size_t content_length = std::stoull(it->second);
      if (content_length > 1024 * 1024)
      {               // Arbitrary limit of 1MB
        return false; // Reject requests larger than 1MB
      }
    }
    return true; // Continue with the request
  });
#endif
  EXPECT_TRUE(true);
  BOF::Bof_MsSleep(99999999);
}
