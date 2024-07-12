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
#include "httplib.h"

class AppSrvRest : BOFWEBRPC::BofWebServer
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

    WebServerParam_X.CertificatePath_S = "";
    WebServerParam_X.PrivateKeyPath_S = "";
    WebServerParam_X.WebAppParam_X.AppName_S = "bofwebrpc-tests";
    WebServerParam_X.WebAppParam_X.ConfigThreadPollTimeInMs_U32 = 2000;

    std::shared_ptr<BOF::IBofLoggerFactory> psLoggerFactory = std::make_shared<BOF::BofBasicLoggerFactory>(true, ".");
    mpuAppSrvRest = std::make_unique<AppSrvRest>(psLoggerFactory, WebServerParam_X);
  }

  void TearDown() override
  {
    // Cleanup common resources after each test
  }

private:
  std::unique_ptr<AppSrvRest> mpuAppSrvRest = nullptr;
};

TEST_F(bofwebapp_tests, Test)
{
  EXPECT_TRUE(true);
}
