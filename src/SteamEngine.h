
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
  virtual void* GetIClientUser(int,int,char const*);
  virtual int GetIClientGameServer(int,int,char const*);
  virtual int SetLocalIPBinding(unsigned int,unsigned short);
  virtual int GetUniverseName(int);
  virtual int GetIClientFriends(int,int,char const*);
  virtual void* GetIClientUtils(int);
  virtual int GetIClientBilling(int,int,char const*);
  virtual int GetIClientMatchmaking(int,int,char const*);
  virtual int GetIClientApps(int,int,char const*);
  virtual int GetIClientMatchmakingServers(int,int,char const*);
  virtual int GetIClientGameSearch(int,int,char const*);
  virtual int RunFrame(void);
  virtual int GetIPCCallCount(void);
  virtual int GetIClientUserStats(int,int,char const*);
  virtual int GetIClientGameServerStats(int,int,char const*);
  virtual int GetIClientNetworking(int,int,char const*);
  virtual void* GetIClientRemoteStorage(int,int,char const*);
};


