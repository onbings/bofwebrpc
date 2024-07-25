/*!
  Copyright (c) 2008, EVS. All rights reserved.

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND,  EITHER EXPRESSED OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  History:
    V 1.00  August 24 2013  BHA : Original version
*/
#include "bofwebrpc/bofwebserver.h"
#include <bofstd/bofsystem.h>
#include <bofstd/bofthread.h>

BEGIN_WEBRPC_NAMESPACE()

struct BOF_THREAD_POOL_PARAM
{
  uint32_t PoolSize_U32;
  uint32_t MaxQueuedRequests_U32;
  std::string BaseName_S;
  bool PriorityInversionAware_B;
  BOF::BOF_THREAD_SCHEDULER_POLICY ThreadSchedulerPolicy_E;
  BOF::BOF_THREAD_PRIORITY ThreadPriority_E;
  uint64_t ThreadCpuCoreAffinityMask_U64;
  uint32_t StackSize_U32;

  BOF_THREAD_POOL_PARAM()
  {
    Reset();
  }

  void Reset()
  {
    PoolSize_U32 = 0;
    MaxQueuedRequests_U32 = 0;
    BaseName_S = "";
    PriorityInversionAware_B = false;
    ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_MAX;
    ThreadPriority_E = BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
    ThreadCpuCoreAffinityMask_U64 = 0;
    StackSize_U32 = 0;
  }
};
class BofThreadPool
{
public:
  BofThreadPool(const BOF_THREAD_POOL_PARAM &_rThreadPoolParam_X)
  {
    uint32_t i_U32;

    mThreadPoolParam_X = _rThreadPoolParam_X;
    for (i_U32 = 0; i_U32 < mThreadPoolParam_X.PoolSize_U32; i_U32++)
    {
      std::unique_ptr<BOF::BofThread> puThread = std::make_unique<BOF::BofThread>();
      puThread->SetThreadCallback(nullptr, BOF_BIND_0_ARG_TO_METHOD(this, BofThreadPool::OnProcessing), nullptr);
      puThread->LaunchBofProcessingThread(mThreadPoolParam_X.BaseName_S + "_" + std::to_string(i_U32), mThreadPoolParam_X.PriorityInversionAware_B, false, 0,
                                          mThreadPoolParam_X.ThreadSchedulerPolicy_E, mThreadPoolParam_X.ThreadPriority_E,
                                          mThreadPoolParam_X.ThreadCpuCoreAffinityMask_U64, 1000, mThreadPoolParam_X.StackSize_U32);
      mThreadCollection.emplace_back(std::move(puThread));
    }
  }

  BofThreadPool(const BofThreadPool &) = delete;
  ~BofThreadPool()
  {
    // Stop all worker threads...
    {
      std::unique_lock<std::mutex> Lock(mMtx);
      mDoShutdown_B = true;
    }
    mDoShedulCv.notify_all();
    // Join...
    for (auto &puThread : mThreadCollection)
    {
      puThread.reset(nullptr);
    }
  }
  bool Enqueue(std::function<void()> _Fn)
  {
    bool Rts_B = false;
    {
      std::unique_lock<std::mutex> Lock(mMtx);
      if ((mThreadPoolParam_X.MaxQueuedRequests_U32 == 0) || (mJobCollection.size() < mThreadPoolParam_X.MaxQueuedRequests_U32))
      {
        mJobCollection.push_back(std::move(_Fn));
        Rts_B = true;
      }
    }
    mDoShedulCv.notify_one();
    return Rts_B;
  }

private:
  BOFERR OnProcessing()
  {
    BOFERR Rts_E = BOF_ERR_NO_ERROR;

    while (1)
    {
      std::function<void()> Fn;
      {
        std::unique_lock<std::mutex> Lock(mMtx);
        mDoShedulCv.wait(Lock, [&] { return ((!mJobCollection.empty()) || (mDoShutdown_B)); });
        if ((mDoShutdown_B) && (mJobCollection.empty()))
        {
          break;
        }
        Fn = mJobCollection.front();
        mJobCollection.pop_front();
      }
      // assert(true == static_cast<bool>(Fn));
      Fn();
    }
    return Rts_E;
  }
  BOF_THREAD_POOL_PARAM mThreadPoolParam_X;
  std::vector<std::unique_ptr<BOF::BofThread>> mThreadCollection;
  std::list<std::function<void()>> mJobCollection;
  bool mDoShutdown_B = false;
  std::condition_variable mDoShedulCv;
  std::mutex mMtx;
};

class BofWebServerTaskQueue : public httplib::TaskQueue
{
public:
  BofWebServerTaskQueue(BofWebServer *_pBofWebServer, size_t _PoolSize)
  {
    BOF_THREAD_POOL_PARAM ThreadPoolParam_X;
    mpBofWebServer = _pBofWebServer;

    ThreadPoolParam_X.PoolSize_U32 = _PoolSize;
    ThreadPoolParam_X.MaxQueuedRequests_U32 = (_PoolSize / 2);
    ThreadPoolParam_X.BaseName_S = "BofWebServer";
    ThreadPoolParam_X.PriorityInversionAware_B = false;
    ThreadPoolParam_X.ThreadSchedulerPolicy_E = BOF::BOF_THREAD_SCHEDULER_POLICY::BOF_THREAD_SCHEDULER_POLICY_OTHER;
    ThreadPoolParam_X.ThreadPriority_E = BOF::BOF_THREAD_PRIORITY::BOF_THREAD_PRIORITY_000;
    ThreadPoolParam_X.ThreadCpuCoreAffinityMask_U64 = 0;
    ThreadPoolParam_X.StackSize_U32 = 0;
    mpuThreadPool = std::make_unique<BofThreadPool>(ThreadPoolParam_X);
  }

  bool enqueue(std::function<void()> _Fn) override
  {
    /* Return true if the task was actually enqueued, or false
     * if the caller must drop the corresponding connection. */
    return mpuThreadPool->Enqueue(_Fn);
  }

  void shutdown() override
  {
    mpuThreadPool.reset(nullptr);
  }

  void on_idle() override
  {
    if (mpBofWebServer)
    {
      mpBofWebServer->V_OnIdle();
    }
  }

private:
  std::unique_ptr<BofThreadPool> mpuThreadPool = nullptr;
  BofWebServer *mpBofWebServer = nullptr;
};
BofWebServer::BofWebServer(std::shared_ptr<BOF::IBofLoggerFactory> _psLoggerFactory, const BOF_WEB_SERVER_PARAM &_rWebServerParam_X)
    : BofWebApp(_psLoggerFactory, true, _rWebServerParam_X.WebAppParam_X)
{
  bool CreateHttpsServer_B;
  BOF_WEB_HEADER HeaderCollection;

  mWebServerParam_X = _rWebServerParam_X;
  mStopServerThread.store(false);
  CreateHttpsServer_B = mWebServerParam_X.CertificatePath_S.empty() ? false : true;
  if (CreateHttpsServer_B)
  {
    CreateHttpsServer_B = mWebServerParam_X.PrivateKeyPath_S.empty() ? false : true;
  }
  if (CreateHttpsServer_B)
  {
    mpHttpsServer = new BOF_HTTPS_SERVER(mWebServerParam_X.CertificatePath_S.c_str(), mWebServerParam_X.PrivateKeyPath_S.c_str());
    if (mpHttpsServer)
    {
      mpHttpServer = reinterpret_cast<BOF_HTTPS_SERVER *>(mpHttpsServer);
    }
  }
  else
  {
    mpHttpServer = new BOF_HTTP_SERVER();
  }
  if (mpHttpServer)
  {
    if (mWebServerParam_X.ThreadPoolSize_U32 == 0)
    {
      mWebServerParam_X.ThreadPoolSize_U32 = 4;
    }
    mpHttpServer->new_task_queue = [this]() { return new BofWebServerTaskQueue(this, this->mWebServerParam_X.ThreadPoolSize_U32); };
    HeaderCollection.insert(std::make_pair("User-Agent", "BofWebRpcAgent/1.0.0"));
    mpHttpServer->set_default_headers(HeaderCollection);

    SetFileExtensionAndMimetypeMapping(".tar.gz", "application/octet-stream");
    SetFileExtensionAndMimetypeMapping(".json", "application/json");

    mpHttpServer->set_file_request_handler([this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes) { this->V_OnFileRequest(_rReq, _rRes); });
    mpHttpServer->set_error_handler([this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes) { this->V_OnError(_rReq, _rRes); });
    mpHttpServer->set_exception_handler(
        [this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes, std::exception_ptr _Ep) { this->V_OnException(_rReq, _rRes, _Ep); });
    mpHttpServer->set_pre_routing_handler(
        [this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes) -> BOF_WEB_HANDLER_RESPONSE { return this->OnPreRouting(_rReq, _rRes); });
    mpHttpServer->set_post_routing_handler([this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes) { this->V_OnPostRouting(_rReq, _rRes); });
    mpHttpServer->set_expect_100_continue_handler(
        [this](const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes) -> BOF_WEB_STATUS { return this->V_OnExpect100Continue(_rReq, _rRes); });
    mpHttpServer->set_socket_options([this](BOF_WEB_SOCKET _Socket) { this->V_OnSetSocketOption(_Socket); });
    mpHttpServer->set_logger([this](const BOF_WEB_REQUEST &_rReq, const BOF_WEB_RESPONSE &_rRes) { this->LogRequestAndResponse(_rReq, _rRes); });

    if (mWebServerParam_X.KeepAliveMaxCount_U32 == 0)
    {
      mWebServerParam_X.KeepAliveMaxCount_U32 = 1;
    }
    mpHttpServer->set_keep_alive_max_count(mWebServerParam_X.KeepAliveMaxCount_U32);
    mpHttpServer->set_keep_alive_timeout(mWebServerParam_X.KeepAliveTimeoutInMs_U32);
    mpHttpServer->set_read_timeout(std::chrono::milliseconds(mWebServerParam_X.ReadTimeoutInMs_U32));
    mpHttpServer->set_write_timeout(std::chrono::milliseconds(mWebServerParam_X.WriteTimeoutInMs_U32));
    mpHttpServer->set_idle_interval(std::chrono::milliseconds(mWebServerParam_X.IdleIntervalInMs_U32));
    if (mWebServerParam_X.PayloadMaxLengthInByte_U32 == 0)
    {
      mWebServerParam_X.KeepAliveMaxCount_U32 = 0x1000;
    }
    mpHttpServer->set_payload_max_length(mWebServerParam_X.PayloadMaxLengthInByte_U32);
    mpHttpServer->set_base_dir("/", mWebServerParam_X.RootDir_S);
  }
}
BofWebServer ::~BofWebServer()
{
  Stop();
  if (mpHttpsServer)
  {
    delete mpHttpsServer;
  }
  else
  {
    delete mpHttpServer;
  }
}
BOF_WEB_HANDLER_RESPONSE BofWebServer::OnPreRouting(const BOF_WEB_REQUEST &_rReq, BOF_WEB_RESPONSE &_rRes)
{
  BOF_WEB_HANDLER_RESPONSE Rts = BOF_WEB_HANDLER_RESPONSE::Unhandled;
  if (V_OnPreRouting(_rReq, _rRes))
  {
    Rts = BOF_WEB_HANDLER_RESPONSE::Handled;
  }
  return Rts;
}
bool BofWebServer::Start(const std::string &_rIpAddress_S, uint16_t _Port_U16)
{
  bool Rts_B = false;
  uint32_t Start_U32, Delta_U32;
  // BOF_WEB_APP_HOST mHost_X;
  if (_rIpAddress_S == "")
  {
    mHost_X = BOF_WEB_APP_HOST(mWebAppConfig.at("httpServer"));
  }
  else
  {
    mHost_X.IpAddress_S = _rIpAddress_S;
    mHost_X.Port_U16 = _Port_U16;
  }

  LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "Starting Server on %s:%d\n", mHost_X.IpAddress_S.c_str(),
           mHost_X.Port_U16);

  for (auto &rBannedIp : mHost_X.BannedIpAddressCollection)
  {
    LOG_INFO(S_mpsWebAppLoggerCollection[WEB_APP_LOGGER_CHANNEL::WEB_APP_LOGGER_CHANNEL_APP], "  Banned IP: %s\n", rBannedIp.c_str());
  }

  mStopServerThread.store(false);
  if (mServerThread.joinable())
  {
    Stop();
  }
  mServerThread = std::thread([this]() {
    mpHttpsServer ? mpHttpsServer->listen(mHost_X.IpAddress_S.c_str(), mHost_X.Port_U16, 0)
                  : mpHttpServer->listen(mHost_X.IpAddress_S.c_str(), mHost_X.Port_U16, 0);
  });
  Start_U32 = BOF::Bof_GetMsTickCount();
  do
  {
    if (IsRunning())
    {
      break;
    }
    else
    {
      BOF::Bof_MsSleep(20);
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
  } while (Delta_U32 < mWebServerParam_X.ServerStartStopTimeoutInMs_U32);
  Rts_B = IsRunning();
  return Rts_B;
}

bool BofWebServer::Stop()
{
  bool Rts_B = false;
  uint32_t Start_U32, Delta_U32;

  mpHttpServer->stop();
  Start_U32 = BOF::Bof_GetMsTickCount();
  do
  {
    if (!IsRunning())
    {
      break;
    }
    else
    {
      BOF::Bof_MsSleep(20);
    }
    Delta_U32 = BOF::Bof_ElapsedMsTime(Start_U32);
  } while (Delta_U32 < mWebServerParam_X.ServerStartStopTimeoutInMs_U32);
  Rts_B = !IsRunning();
  mStopServerThread.store(true);
  if (mServerThread.joinable())
  {
    mServerThread.join();
  }
  return Rts_B;
}
bool BofWebServer::Get(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Get(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::Post(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Post(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::Put(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Put(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::Patch(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Patch(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::Delete(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Delete(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::Options(const std::string &_rUri_S, BOF_WEB_HANDLER _Handler)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->Options(_rUri_S, _Handler);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::SetMountPoint(const std::string &_rMountPoint_S, const std::string &_rDir_S, httplib::Headers _rHeaderCollection)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    Rts_B = mpHttpServer->set_mount_point(_rMountPoint_S, _rDir_S);
  }
  return Rts_B;
}
bool BofWebServer::RemoveMountPoint(const std::string &_rMountPoint_S)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    Rts_B = mpHttpServer->remove_mount_point(_rMountPoint_S);
  }
  return Rts_B;
}
bool BofWebServer::SetFileExtensionAndMimetypeMapping(const std::string &_rExt_S, const std::string &_rMime_S)
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    mpHttpServer->set_file_extension_and_mimetype_mapping(_rExt_S, _rMime_S);
    Rts_B = true;
  }
  return Rts_B;
}
bool BofWebServer::IsRunning() const
{
  bool Rts_B = false;
  if (mpHttpServer)
  {
    Rts_B = mpHttpServer->is_running();
  }
  return Rts_B;
}
END_WEBRPC_NAMESPACE()