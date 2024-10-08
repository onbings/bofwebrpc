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

BEGIN_WEBRPC_NAMESPACE()
std::array<std::shared_ptr<BOF::IBofLogger>, WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_MAX> BofWebApp::S_mpsWebAppLoggerCollection;
uint32_t BofWebApp::S_mInstanceId_U32 = 0;

BofWebApp::BofWebApp(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, bool _Server_B, const BOF_WEB_APP_PARAM &_rWebAppParam_X)
{
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
  Rts_B = ReadConfig(mWebAppConfig);
  return Rts_B;
}

bool BofWebApp::Shutdown()
{
  bool Rts_B = true;
  return Rts_B;
}

bool BofWebApp::ReadConfig(BOF_WEB_JSON &_rConfig)
{
  bool Rts_B = false;
  std::string Cwd_S, CfgPath_S;

  try
  {
    BOF::Bof_GetCurrentDirectory(Cwd_S);
    CfgPath_S = Cwd_S + "/assets/" + mWebAppParam_X.AppName_S + (mServer_B ? "-clt.json" : "-srv.json");
    BOF_LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Reading configuration from %s (Cwd is %s)\n",
                 CfgPath_S.c_str(), Cwd_S.c_str());
    std::ifstream ConfigFile(CfgPath_S);

    ConfigFile >> _rConfig;
    Rts_B = true;
  }
  catch (std::exception &e)
  {
  }
  /*
  catch (...)
  {
    throw "Can't open config";
  }
  */
  if (!Rts_B)
  {
    BOF_LOG_ERROR(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Cannot read configuration from %s (Cwd is %s)\n",
                  CfgPath_S.c_str(), Cwd_S.c_str());
  }
  return Rts_B;
}

void BofWebApp::ConfigureLogger(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory)
{
  char pLibName_c[0x100];

  for (auto &rpsLogger : S_mpsWebAppLoggerCollection)
  {
    rpsLogger = nullptr;
  }
  if (_psLoggerFactory)
  {
    snprintf(pLibName_c, sizeof(pLibName_c), "%s_BofWebApp_%04d_", mServer_B ? "Srv" : "Clt", S_mInstanceId_U32);
    S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP] = _psLoggerFactory->V_Create(pLibName_c, "APP");
  }
}

std::string BofWebApp::DumpHeader(const BOF_WEB_HEADER &_rHeader)
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

std::string BofWebApp::LogRequestAndResponse(const BOF_WEB_REQUEST &_rReq, const BOF_WEB_RESPONSE &_rRes)
{
  std::string Rts_S;
  char pBuffer_c[0x1000];
  std::string Query_S;

  Rts_S += "===Begin=============================\n";

  snprintf(pBuffer_c, sizeof(pBuffer_c), "%s %s %s", _rReq.method.c_str(), _rReq.version.c_str(), _rReq.path.c_str());
  Rts_S += pBuffer_c;

  for (auto It = _rReq.params.begin(); It != _rReq.params.end(); ++It)
  {
    const auto &x = *It;
    snprintf(pBuffer_c, sizeof(pBuffer_c), "%c%s=%s", (It == _rReq.params.begin()) ? '?' : '&', x.first.c_str(), x.second.c_str());
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
  BOF_LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "%s", Rts_S.c_str());
  return Rts_S;
}

bool BofWebApp::S_ParseContentRangeRequest(const std::string &_rContentRangeRequest_S, size_t &_rRangeMin, size_t &_rRangeMax, size_t &_rDataSize)
{
  bool Rts_B = false;
  std::regex RegExContentRange(R"(bytes (\d+)-(\d+)/(\d+))");
  std::smatch Match;

  if (std::regex_match(_rContentRangeRequest_S, Match, RegExContentRange))
  {
    if (Match.size() == 4)
    {
      try
      {
        _rRangeMin = std::stoul(Match[1].str());
        _rRangeMax = std::stoul(Match[2].str());
        _rDataSize = std::stoul(Match[3].str());
        Rts_B = true;
      }
      catch (const std::exception &e)
      {
        printf("Error parsing Content-Range: %s\n", e.what());
      }
    }
  }
  return Rts_B;
}
bool BofWebApp::S_SplitCmdAndArg(const std::string &_rCmdArgRequest_S, std::string &_rCmd_S, std::string &_rArg_S)
{
  bool Rts_B = false;
  // std::regex RegExCmdArg(R"((\w+)(?:\s+(.*))?)");
  std::regex RegExCmdArg(R"((\w+)\((.*?)\))");
  std::smatch Match;

  if (std::regex_match(_rCmdArgRequest_S, Match, RegExCmdArg))
  {
    if (Match.size() >= 2)
    {
      _rCmd_S = Match[1].str();
      if (Match.size() == 3)
      {
        _rArg_S = Match[2].str();
      }
      else
      {
        _rArg_S.clear(); // Clear the argument if it's not present
      }
      Rts_B = true;
    }
  }
  return Rts_B;
}
std::vector<std::string> BofWebApp::S_SplitArg(const std::string &_rArg_S)
{
  std::vector<std::string> Rts;
  std::stringstream Ss(_rArg_S);
  std::string Item_S;

  while (std::getline(Ss, Item_S, ','))
  {
    Rts.push_back(Item_S);
  }

  return Rts;
}
std::string BofWebApp::S_GenerateSessionId(uint32_t _SessionIdLen_U32)
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

bool BofWebApp::S_CreateTestFile(const std::string &_rPath_S, uint32_t _SizeInByte_U32, uint8_t _ValueStart_U32)
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
// https://www.moesif.com/blog/technical/api-design/REST-API-Design-Best-Practices-for-Parameters-and-Query-String-Usage/
#if 0
std::map<std::string, std::string> BofWebApp::S_ParseMapString(const std::string &mapString)
{
  std::map<std::string, std::string> Rts;
  std::istringstream Iss(mapString);
  std::string ParamPair_S, Key_S, Value_S;
  size_t EqualPos;

  while (std::getline(Iss, ParamPair_S, '&'))
  {
    EqualPos = ParamPair_S.find('=');
    if (EqualPos != std::string::npos)
    {
      Key_S = ParamPair_S.substr(0, equalPos);
      Value_S = httplib::detail::decode_url(ParamPair_S.substr(EqualPos + 1));
      Rts[Key_S] = Value_S;
    }
  }
  return Rts;
}
std::string BofWebApp::S_ConstructMapString(const std::map<std::string, std::string> &_rParamCollection)
{
  std::ostringstream Oss;
  bool First_B = true;
  for (const auto &rParam : _rParamCollection)
  {
    if (!First_B)
    {
      Oss << "&";
    }
    Oss << rParam.first << "=" << httplib::detail::encode_url(rParam.second);
    First_B = false;
  }
  return Oss.str();
}
#endif
END_WEBRPC_NAMESPACE()