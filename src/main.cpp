#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "SteamRemote.h"
#include "SteamUtils.h"
#include "SteamEngine.h"
#include "SteamUGC.h"
#include "SteamUser.h"
#include <chrono>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

typedef bool(*CallbackFunc)(int, void*);
typedef bool(*FreeCallbackFunc)(int);

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

int WaitForLogin() {
    while(true) {
        CallbackMsg_t c;
        g_pEngine->RunFrame();
        if(GetCallback(steamPipe, &c)) {
            FreeLastCallback(steamPipe);
            switch(c.m_iCallback) {
                case 101:
                    return 1;
                case 102:
                    return 0;
                default:
                    break;
            }
        }
    }
}

int Workshop_Func(char** args, int num_args) {	
    if(num_args < 3) {
        std::cout << "Insufficient supplied arguments!" << std::endl;
        return 1;
    }
    
    userHdl = g_pEngine->CreateGlobalUser(&steamPipe);
    SteamUtils* utils = (SteamUtils*)g_pEngine->GetIClientUtils(steamPipe);
    SteamRemote* remote = (SteamRemote*)g_pEngine->GetIClientRemoteStorage(steamPipe,userHdl, "");
    SteamUser* user = (SteamUser*)g_pEngine->GetIClientUser(steamPipe,userHdl,"");
    IUGC* ugc = (IUGC*)g_pEngine->GetIClientUGC(steamPipe, userHdl);

    debug << "IUGC: " << ugc << std::endl;
    debug << "Set CCheckCallback" << std::endl;
    g_pEngine->Set_Client_API_CCheckCallbackRegisteredInProcess(0);
    g_pEngine->RunFrame();
    
    if(!utils) {
        std::cout << "Could not acquire Utils interface!" << std::endl;
        return 1;
    }
    
    if(!remote) {
        std::cout << "Could not acquire Remote interface!" << std::endl;
        return 1;
    }
    
    if(!user) {
        std::cout << "Could not acquire User interface!" << std::endl;
        return 1;
    }

    if(!ugc) {
        std::cout << "Could not acquire UGC interface!" << std::endl;
        return 1;
    }
    
    debug << "Setting Creds: " << args[1] << std::endl;
    
    int login_result = user->SetAccountNameForCachedCredentialLogin(args[1], 0);
    if(!login_result) {
        std::cout << "Cached login credentials not available. Please login with steamCMD." << std::endl;
        return 1;
    }

    user->RaiseConnectionPriority(2, 13);
    std::cout << "Trying logon state...." << std::endl;
    
    CSteamID s = user->GetSteamID();	
    std::cout << "STEAMID:" << s.m_unAll64Bits << std::endl;
    
    user->LogOn(s);
    
    if(!WaitForLogin()) {
        std::cout << "Login failed. Please check credentials and try again." << std::endl;
        return 1;
    }
    
    std::cout << "Logged in." << std::endl;

    std::string sWorkshopIdentifier = args[2];
    unsigned long id = 0;
    try {
        id = std::stol(sWorkshopIdentifier);
    } catch(...) {
        std::cout << "Invalid workshop ID!" << std::endl;
        return 1;
    }

    std::cout << "Starting Item Update." << std::endl;
    unsigned long long update_handle = ugc->StartItemUpdate(4000, id);
    debug << "Update handle: " << update_handle << std::endl;
    
    std::filesystem::path p = args[3];
    auto absPath = std::filesystem::absolute(p);
    debug << "Absolute path: " << absPath << std::endl;

    if(!file_exists(absPath)) {
        std::cout << "File doesn't exist." << std::endl;
        return 1;
    }

    std::cout << "Setting Item Content." << std::endl;
    ugc->SetItemContent(update_handle, (const char*)absPath.c_str());

    std::string update_note = "";
    if(num_args >= 4)
        update_note = args[4];

    std::cout << "Submitting item update." << std::endl;
    unsigned long long call_result = ugc->SubmitItemUpdate(update_handle, (const char*)update_note.c_str());

    bool res = 0;
    unsigned long long lastCurrent = 0;
    while(!utils->IsAPICallCompleted(call_result, &res)) {
        g_pEngine->RunFrame();
        unsigned long long current = 0;
        unsigned long long total = 0;

        ugc->GetItemUpdateProgress(update_handle, &current, &total);

        if(current > 0 && total > 0 && current != lastCurrent) {
            std::cout << "Progress: " << std::setprecision(4) 
            << std::left << (double)current/total*100 
            << std::setw(20) << "%" << "\r" << std::flush;
            lastCurrent = current;
        }
    }

    std::cout << std::endl;

    debug << "IsAPICallCompleted Result: " << res << std::endl;
    std::cout << "Item update complete." << std::endl;
    return 0;
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

#ifdef DEBUG
    debug.rdbuf(std::cout.rdbuf());
#endif

    int ret = Workshop_Func(argv, argc-1);

    g_pEngine->ReleaseUser(steamPipe, userHdl);
    g_pEngine->BReleaseSteamPipe(steamPipe);
    g_pEngine->BShutdownIfAllPipesClosed();
    return ret;
}
