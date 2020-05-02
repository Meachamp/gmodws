#include <stdio.h>
#include <dlfcn.h>
#include <string>
#include <sstream>
#include "SteamRemote.h"
#include "SteamUtils.h"
#include "SteamEngine.h"
#include "SteamUGC.h"
#include "SteamUser.h"
#include <chrono>
#include <unistd.h>
#include <pthread.h>
#include <filesystem>
#include <fstream>
#include <iostream>

typedef bool(*CallbackFunc)(int, void*);
typedef bool(*FreeCallbackFunc)();

IEngine* g_pEngine;
CallbackFunc GetCallback;
FreeCallbackFunc FreeLastCallback;
int steamPipe = 0;
int userHdl = 0;

std::ostream debug(0);

struct CallbackMsg_t
{
	int m_hSteamUser;
	int m_iCallback;
	char *m_pubParam; 
	int m_cubParam;
};

time_t GetTime() {
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

bool file_exists (const std::string& file) {
    std::ifstream f(file.c_str());
    return f.good();
}

void Workshop_Func(char** args, int num_args) {	
	if(num_args < 3) {
		std::cout << "Insufficient supplied arguments!" << std::endl;
		return;
	}
	
	userHdl = g_pEngine->CreateGlobalUser(&steamPipe);
	SteamUtils* utils = (SteamUtils*)g_pEngine->GetIClientUtils(steamPipe);
	SteamRemote* remote = (SteamRemote*)g_pEngine->GetIClientRemoteStorage(steamPipe,userHdl, "");
	SteamUser* user = (SteamUser*)g_pEngine->GetIClientUser(steamPipe,userHdl,"");
	IUGC* ugc = (IUGC*)g_pEngine->GetIClientUGC(steamPipe, userHdl);

	debug << "IUGC: " << ugc << std::endl;
	g_pEngine->RunFrame();
	
	if(!utils) {
	    std::cout << "Could not acquire Utils interface!" << std::endl;
	    return;
	}
	
	if(!remote) {
	    std::cout << "Could not acquire Remote interface!" << std::endl;
	    return;
	}
	
	if(!user) {
	    std::cout << "Could not acquire User interface!" << std::endl;
	    return;
	}

	if(!ugc) {
		std::cout << "Could not acquire UGC interface!" << std::endl;
	}
	
	debug << "Setting Creds: " << args[1] << std::endl;
	int login_result = user->SetAccountNameForCachedCredentialLogin(args[1], 0);
	
	if(!login_result) {
		std::cout << "Cached login credentials not available. Please login with steamCMD." << std::endl;
		return;
	}

	user->RaiseConnectionPriority(2, 13);
	std::cout << "Trying logon state...." << std::endl;
	
	CSteamID s = user->GetSteamID();	
	std::cout << "STEAMID:" << s.m_unAll64Bits << std::endl;
	
	user->LogOn(s);

	int i = 0;
	while(i < 5) {
		CallbackMsg_t c;
		g_pEngine->RunFrame();
		if(GetCallback(steamPipe, &c)) {
			std::cout << c.m_iCallback << std::endl;
			if(c.m_iCallback == 101)
				break;
		}
		FreeLastCallback();
		break;
	}
	
	//I really need to reverse callbacks, but this will do for now. 
	auto startTime = GetTime();
	while(!(user->BLoggedOn())) {
	    g_pEngine->RunFrame();
	    auto curTime = GetTime();
	    if(curTime > startTime + 15) {
			printf("Login timed out!\n");
			return;
		}
	}
	
	std::cout << "Logged in." << std::endl;

	std::string sWorkshopIdentifier = args[2];
	unsigned long id = 0;
	try {
		id = std::stol(sWorkshopIdentifier);
	} catch(...) {
		std::cout << "Invalid workshop ID!" << std::endl;
		return;
	}

	std::cout << "Starting Item Update." << std::endl;
	unsigned long long update_handle = ugc->StartItemUpdate(4000, id);
	debug << "Update handle: " << update_handle << std::endl;
	
	std::filesystem::path p = args[3];
	auto absPath = std::filesystem::absolute(p);
	debug << "Absolute path: " << absPath << std::endl;

	if(!file_exists(absPath)) {
		std::cout << "File doesn't exist." << std::endl;
		return;
	}

	std::cout << "Setting Item Content." << std::endl;
	ugc->SetItemContent(update_handle, (const char*)absPath.c_str());

	std::string update_note = "";
	if(num_args >= 4)
		update_note = args[4];

	std::cout << "Submitting item update." << std::endl;
	unsigned long long call_result = ugc->SubmitItemUpdate(update_handle, (const char*)update_note.c_str());


}

int main(int argc, char** argv) {
    std::cout << "Loaded." << std::endl;
	void* steam = dlopen("steamclient.so", RTLD_NOW);
	
	if(!steam) {
	    std::cout << "steamclient not present!" << std::endl;
	    return 1;
	}
	
	GetCallback = (CallbackFunc)dlsym(steam, "Steam_BGetCallback");
	FreeLastCallback = (FreeCallbackFunc)dlsym(steam, "Steam_FreeLastCallback");
	//GetAPICallResult = dlsym(steam, "Steam_GetAPICallResult");

	debug << "GetCallback: " << GetCallback << std::endl;
	debug << "FreeLastCallback: " << FreeLastCallback << std::endl;

	typedef void*(*InterfaceFunc)(const char*, int);
	InterfaceFunc steamIface = (InterfaceFunc)dlsym(steam, "CreateInterface");
	
	if(!steamIface) {
	    std::cout << "CreateInterface function not present!" << std::endl;
	    return 1;
	}
	
	g_pEngine = (IEngine*)steamIface("CLIENTENGINE_INTERFACE_VERSION005", 0);

	if(!g_pEngine) {
		std::cout << "Engine interface not present!" << std::endl;
	    return 1;
	}
	debug.rdbuf(std::cout.rdbuf());
	Workshop_Func(argv, argc-1);

	g_pEngine->ReleaseUser(steamPipe, userHdl);
	g_pEngine->BReleaseSteamPipe(steamPipe);
	g_pEngine->BShutdownIfAllPipesClosed();
	pthread_exit(0);
	return 0;
}
