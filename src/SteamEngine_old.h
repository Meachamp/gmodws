
class IEngine
{
public:
  virtual int CreateSteamPipe(int*);
  virtual int BReleaseSteamPipe(int);
  virtual int CreateGlobalUser(int *);
  virtual int ConnectToGlobalUser(int);
  virtual int CreateLocalUser(int *,int);
  virtual int CreatePipeToLocalUser(int,int *);
  virtual int ReleaseUser(int,int);
  virtual int IsValidHSteamUserPipe(int,int);
  virtual void* GetIClientUser(int,int);
  virtual int GetIClientGameServer(int,int);
  virtual int SetLocalIPBinding(unsigned int,unsigned short);
  virtual int GetUniverseName(int);
  virtual int GetIClientFriends(int,int);
  virtual void* GetIClientUtils(int);
  virtual int GetIClientBilling(int,int);
  virtual int GetIClientMatchmaking(int,int);
  virtual int GetIClientApps(int,int);
  virtual int GetIClientMatchmakingServers(int,int);
  virtual int GetIClientGameSearch(int,int);
  virtual int RunFrame(void);
  virtual int GetIPCCallCount(void);
  virtual int GetIClientUserStats(int,int);
  virtual int GetIClientGameServerStats(int,int);
  virtual int GetIClientNetworking(int,int);
  virtual void* GetIClientRemoteStorage(int,int);
  virtual void GetIClientScreenshots(int,int);
  virtual void SetWarningMessageHook(void*);
  virtual void GetIClientGameCoordinator(int,int);
  virtual void SetOverlayNotificationPosition(int);
  virtual void SetOverlayNotificationInset(int,int);
  virtual void HookScreenshots(bool);
  virtual void IsScreenshotsHooked(void);
  virtual void IsOverlayEnabled(void);
  virtual void GetAPICallResult(int,unsigned long long,void *,int,int,bool *);
  virtual void GetIClientProductBuilder(int,int);
  virtual void GetIClientDepotBuilder(int,int);
  virtual void GetIClientNetworkDeviceManager(int);
  virtual void ConCommandInit(void*);
  virtual void GetIClientAppManager(int,int);
  virtual void GetIClientConfigStore(int,int);
  virtual void BOverlayNeedsPresent(void);
  virtual void GetIClientGameStats(int,int);
  virtual void GetIClientHTTP(int,int);
  virtual void FlushBeforeValidate(void);
  virtual void BShutdownIfAllPipesClosed(void);
  virtual void GetIClientAudio(int,int);
  virtual void GetIClientMusic(int,int);
  virtual void GetIClientUnifiedMessages(int,int);
  virtual void GetIClientController(int);
  virtual void GetIClientParentalSettings(int,int);
  virtual void GetIClientStreamLauncher(int,int);
  virtual void GetIClientDeviceAuth(int,int);
  virtual void GetIClientRemoteClientManager(int);
  virtual void GetIClientStreamClient(int,int);
  virtual void GetIClientShortcuts(int,int);
  virtual void* GetIClientUGC(int,int);
  virtual void GetIClientInventory(int,int);
  virtual void GetIClientVR(int);
  virtual void GetIClientGameNotifications(int,int);
  virtual void GetIClientHTMLSurface(int,int);
  virtual void GetIClientVideo(int,int);
  virtual void GetIClientControllerSerialized(int);
  virtual void GetIClientAppDisableUpdate(int,int);
  virtual void Set_Client_API_CCheckCallbackRegisteredInProcess(unsigned int (*)(int));
  virtual void GetIClientBluetoothManager(int);
  virtual void GetIClientSharedConnection(int,int);
  virtual void GetIClientShader(int,int);
  virtual void GetIClientNetworkingSocketsSerialized(int,int);
  virtual void GetIClientCompat(int,int);
  virtual void SetClientCommandLine(int,char **);
  virtual void GetIClientParties(int,int);
  virtual void GetIClientNetworkingMessages(int,int);
  virtual void GetIClientNetworkingSockets(int,int);
  virtual void GetIClientNetworkingUtils(int);
  virtual void GetIClientNetworkingUtilsSerialized(int);
  virtual void GetIClientSTARInternal(int,int);
  virtual void GetIClientRemotePlay(int,int);
};

