/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
#include "bofwebrpc/bofwebapp.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <chrono>
#include <bofstd/boffs.h>
/*
/usr/bin/c++  -ggdb -g "CMakeFiles/evs-hwfw-hws-tests.dir/src/main.cpp.o" "CMakeFiles/evs-hwfw-hws-tests.dir/src/ut_oal.cpp.o"
"CMakeFiles/evs-hwfw-hws-tests.dir/src/ut_hws.cpp.o" -o evs-hwfw-hws-tests  ../libevs-hwfw-hws.a ../../evs-hwfw-cms-build/libevs-hwfw-cms.a
../../../vcpkg_installed/x64-linux/debug/lib/libgtest.a ../../../vcpkg_installed/x64-linux/debug/lib/libbofstd.a ../../../output/lib/libevs-hwfw-mailboxapi.a
../../../vcpkg_installed/x64-linux/debug/lib/libdate-tz.a ../../../vcpkg_installed/x64-linux/debug/lib/libjsoncpp.a -lrt
*/
BEGIN_WEBRPC_NAMESPACE()
std::array<std::shared_ptr<BOF::IBofLogger>, WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_MAX> BofWebApp::S_mpsWebAppLoggerCollection;
uint32_t BofWebApp::S_mInstanceId_U32 = 0;

BofWebApp::BofWebApp(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, bool _Server_B, const BOF_WEB_APP_PARAM &_rWebAppParam_X)
{
  /*
    json foo;
    std::string f = "hello";
    char *p = "world";
    foo["flex"] = 0.2;
    foo["awesome_str"] = "bleh";
    foo["nested"] = {{"bar", "barz"}};
  */
  mServer_B = _Server_B;
  mWebAppParam_X = _rWebAppParam_X;
  S_mInstanceId_U32++;

  Initialize(_psLoggerFactory);
}
BofWebApp::~BofWebApp()
{
  Shutdown();
}
bool BofWebApp::Initialize(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory)
{
  bool Rts_B = false;

  srand((unsigned int)time(nullptr));
  ConfigureLogger(_psLoggerFactory);
  mWebAppConfig = ReadConfig(true);

  Rts_B = true;

  if (mWebAppParam_X.ConfigThreadPollTimeInMs_U32)
  {
    mCheckConfigThread = std::thread([&]() {
      while (mWebAppParam_X.ConfigThreadPollTimeInMs_U32)
      {
        try
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(mWebAppParam_X.ConfigThreadPollTimeInMs_U32));
          json Config = ReadConfig(false);
          if (Config != mWebAppConfig)
          {
            V_OnConfigUpdate(Config);
            mWebAppConfig = Config;
          }
        }
        catch (...)
        {
        }
      }
    });
  }
  return Rts_B;
}

bool BofWebApp::Shutdown()
{
  bool Rts_B = false;

  mWebAppParam_X.ConfigThreadPollTimeInMs_U32 = 0;

  mCheckConfigThread.join();

  Rts_B = true;

  return Rts_B;
}

json BofWebApp::ReadConfig(bool _LogIt_B)
{
  json Rts;
  std::string Cwd_S, CfgPath_S;

  try
  {
    BOF::Bof_GetCurrentDirectory(Cwd_S);
    CfgPath_S = Cwd_S + mWebAppParam_X.AppName_S + ".json";
    if (_LogIt_B)
    {
      LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cwd is %s, reading configuration from %s\n", Cwd_S.c_str(),
               CfgPath_S.c_str());
    }
    std::ifstream ConfigFile(CfgPath_S);
    if (ConfigFile.std::ios::eof())
    {
      throw std::runtime_error("Config file is empty");
    }

    // Make sure the config is open
    if (!ConfigFile.is_open())
    {
      throw std::runtime_error("Can't open config");
    }

    ConfigFile >> Rts;
  }
  catch (std::exception &e)
  {
    throw std::string(e.what());
  }
  catch (...)
  {
    throw "Can't open config";
  }
  return Rts;
}

void BofWebApp::ConfigureLogger(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory)
{
  char pLibName_c[128];

  for (auto &rpsLogger : S_mpsWebAppLoggerCollection)
  {
    rpsLogger = nullptr;
  }
  if (_psLoggerFactory)
  {
    sprintf(pLibName_c, "%s_BofWebApp_%04d_", mServer_B ? "Srv" : "Clt", S_mInstanceId_U32);

    S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP] = _psLoggerFactory->V_Create(pLibName_c, "APP");
    S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_REST] = _psLoggerFactory->V_Create(pLibName_c, "REST");
  }
}

void BofWebApp::V_OnConfigUpdate(const json &_rConfig)
{
  //  bool dumpReset = _rConfig["dumps"]["on"].get<bool>();
  //  if (dumpReset != Logger::dump())
  //  {
  //  }
}

std::string BofWebApp::DumpHeader(const httplib::Headers &_rHeader)
{
  std::string Rts_S;
  char pBuffer_c[0x4000];

  for (auto It = _rHeader.begin(); It != _rHeader.end(); ++It)
  {
    const auto &rItem = *It;
    snprintf(pBuffer_c, sizeof(pBuffer_c), "%s: %s\n", rItem.first.c_str(), rItem.second.c_str());
    Rts_S += pBuffer_c;
  }

  return Rts_S;
}

std::string BofWebApp::LogRequest(const httplib::Request &_rReq, const httplib::Response &_rRes)
{
  std::string Rts_S;
  char pBuffer_c[0x4000];
  std::string Query_S;

  Rts_S += "===Begin=============================\n";

  snprintf(pBuffer_c, sizeof(pBuffer_c), "%s %s %s", _rReq.method.c_str(), _rReq.version.c_str(), _rReq.path.c_str());
  Rts_S += pBuffer_c;

  for (auto it = _rReq.params.begin(); it != _rReq.params.end(); ++it)
  {
    const auto &x = *it;
    snprintf(pBuffer_c, sizeof(pBuffer_c), "%c%s=%s", (it == _rReq.params.begin()) ? '?' : '&', x.first.c_str(), x.second.c_str());
    Query_S += pBuffer_c;
  }
  snprintf(pBuffer_c, sizeof(pBuffer_c), "%s\n", Query_S.c_str());
  Rts_S += pBuffer_c;

  Rts_S += DumpHeader(_rReq.headers);

  Rts_S += "--------------------------------\n";

  snprintf(pBuffer_c, sizeof(pBuffer_c), "%d %s\n", _rRes.status, _rRes.version.c_str());
  Rts_S += pBuffer_c;
  Rts_S += DumpHeader(_rRes.headers);
  Rts_S += "\n";

  if (!_rRes.body.empty())
  {
    Rts_S += _rRes.body;
  }

  Rts_S += "\n";
  Rts_S += "===End===============================\n";

  return Rts_S;
}

std::string BofWebApp::GenerateSessionId(uint32_t _SessionIdLen_U32)
{
  std::string Rts_S;
  static std::string S_CharToUse_S = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  uint32_t i_U32;

  for (i_U32 = 0; i_U32 < _SessionIdLen_U32; i_U32++)
  {
    Rts_S += S_CharToUse_S[rand() % S_CharToUse_S.size()];
  }
  return Rts_S;
}
END_WEBRPC_NAMESPACE()