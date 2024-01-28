#include <dlfcn.h>
#include <unistd.h>
#include <pthread.h>
#include "SteamUtils.h"
#include "SteamEngine.h"
#include "SteamUGC.h"
#include "SteamUser.h"
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include "2fa.h"
#include "steam_header.h"

#if __GNUC__ > 7
    #include <filesystem>
#else
    #include <experimental/filesystem>
    namespace std {
        namespace filesystem = experimental::filesystem;
    }
#endif

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

struct SubmitItemUpdateResult_t
{
	enum { k_iCallback = 3400 + 4 };
	int m_eResult;
	bool m_bUserNeedsToAcceptWorkshopLegalAgreement;
	unsigned long long m_nPublishedFileId;
};


time_t GetTime() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

bool file_exists (const std::string& file) {
    std::ifstream f(file.c_str());
    return f.good();
}

int WaitForLogin() {
    int status = 0;
    while(true) {
        CallbackMsg_t c;
        g_pEngine->RunFrame();
        if(GetCallback(steamPipe, &c)) {
            FreeLastCallback(steamPipe);
            switch(c.m_iCallback) {
                debug << c.m_iCallback << std::endl;
                case 987:
                    status |= 1;
                    break;
                case 101:
                    status |= 2;
                    break;
                case 5801:
                    status |= 4;
                    break;
                case 102:
                    return 0;
                case 1020046:
                    return 1;
                    break;
                default:
                    break;
            }
            if(status == 7)
                return 1;
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
    SteamUser* user = (SteamUser*)g_pEngine->GetIClientUser(steamPipe,userHdl);
    IUGC* ugc = (IUGC*)g_pEngine->GetIClientUGC(steamPipe, userHdl);

    debug << "IUGC: " << ugc << std::endl;
    debug << "Set CCheckCallback" << std::endl;
    g_pEngine->Set_Client_API_CCheckCallbackRegisteredInProcess(0);
    g_pEngine->RunFrame();
    
    if(!utils) {
        std::cout << "Could not acquire Utils interface!" << std::endl;
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
        std::cout << "Cached login credentials not available." << std::endl;

        if(!std::getenv("STEAM_PASSWORD")) {
            std::cout << "No password available. Quitting." << std::endl;
            return 1;
        }
    }
    //int login_result = 0;

    user->RaiseConnectionPriority(2, 13);
    std::cout << "Trying logon state...." << std::endl;

    if(!login_result) {
        user->SetLoginInformation(args[1], std::getenv("STEAM_PASSWORD"), 1);
    }

    {
        const char* secret = std::getenv("STEAM_2FASECRET");
        if (secret) {
            std::cout << "Setting 2FA Code..." << std::endl;
            const char* code = code_from_secret(secret);

            std::cout << "Generated code: " << code << std::endl;
            user->SetTwoFactorCode(code);
        }
    }
    
    CSteamID s = user->GetSteamID();	
    std::cout << "STEAMID: " << s.m_unAll64Bits << std::endl;
    user->LogOn(s);
        
    if(!WaitForLogin()) {
        std::cout << "Login failed. Please check credentials and try again." << std::endl;
        return 1;
    }
    
    std::cout << "Logged in." << std::endl;
    std::cout << "SteamID: " << user->GetSteamID().m_unAll64Bits << std::endl;
    std::string sWorkshopIdentifier = args[2];
    std::cout << "Workshop ID: " << sWorkshopIdentifier << std::endl;
    unsigned long id = 0;
    try {
        id = std::stol(sWorkshopIdentifier);
    } catch(...) {
        std::cout << "Invalid workshop ID!" << std::endl;
        return 1;
    }

    {
        uint64_t* uud = (uint64_t*)ugc;
        char* uu2 = (char*)(*(uint64_t*)(*(uint64_t*)(*uud - 8) + 8));
        debug << "UGC VTABLE: " << uud[0] << std::endl;
        debug << "UGC TYPEINFO: " << uu2 << std::endl;
    }

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
    ugc->SetItemVisibility(update_handle, 0);
    std::string update_note = "";
    if(num_args >= 4)
        update_note = args[4];

    std::cout << "Setting tags..." << std::endl;
    SteamParamStringArray_t *pTags = new SteamParamStringArray_t();
    pTags->m_ppStrings = new const char*[1];
    pTags->m_ppStrings[0] = "ServerContent";
    pTags->m_nNumStrings = 1;
    bool tags_result = ugc->SetItemTags(update_handle, pTags);

    std::cout << "Set Tags result: " << tags_result << std::endl;

    std::cout << "Submitting item update." << std::endl;
    unsigned long long call_result = ugc->SubmitItemUpdate(update_handle, (const char*)update_note.c_str());
    debug << "Call Result: " << call_result << std::endl;

    if(!call_result) {
        std::cout << "SubmitItemUpdate returned invalid handle" << std::endl;
        return 1;
    }

    {
        uint64_t* uud = (uint64_t*)utils;
        char* uu2 = (char*)(*(uint64_t*)(*(uint64_t*)(*uud - 8) + 8));
        debug << "UTILS VTABLE: " << uud[0] << std::endl;
        debug << "UTILS TYPEINFO: " << uu2 << std::endl;
    }

    bool callCompleteFailed = 0;
    unsigned long long lastCurrent = 0;
    while(!utils->IsAPICallCompleted(call_result, &callCompleteFailed) && !callCompleteFailed) {
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
    if(callCompleteFailed) {
        std::cout << "API Call failed" << std::endl;
        return 1;
    }

    SubmitItemUpdateResult_t updateRes;
    bool pbFailed = 0;
    bool callResultStatus = utils->GetAPICallResult(call_result, &updateRes, sizeof(SubmitItemUpdateResult_t), 3404, &pbFailed);
    debug << "pbFailed: " << pbFailed << std::endl;
    debug << "CallResultStatus: " << callResultStatus << std::endl;
    debug << "m_eResult: " << updateRes.m_eResult << std::endl;
    debug << "m_bUserNeedsToAcceptWorkshopLegalAgreement: " << updateRes.m_bUserNeedsToAcceptWorkshopLegalAgreement << std::endl;

    debug << "IsAPICallCompleted Result: " << callCompleteFailed << std::endl;

    if(pbFailed || !callResultStatus) {
        std::cout << "API Call Result failed" << std::endl;
        return 1;
    }

    if(updateRes.m_eResult != 1) {
        std::cout << "Item update failed (Code " << updateRes.m_eResult << ")" << std::endl;
        return 1;
    }

    std::cout << "Item update complete." << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    std::cout << "Loaded." << std::endl;
    void* steam = dlopen("steamclient.so", RTLD_NOW);
    
    if(std::getenv("GMODWS_DEBUG")) {
        std::cout << "Starting debug output." << std::endl;
        debug.rdbuf(std::cout.rdbuf());
    } else {
        freopen("/dev/null", "w", stderr);
    }
    
    if(!steam) {
        std::cout << "steamclient not present!" << std::endl;
        return 1;
    }
    
    GetCallback = (CallbackFunc)dlsym(steam, "Steam_BGetCallback");
    FreeLastCallback = (FreeCallbackFunc)dlsym(steam, "Steam_FreeLastCallback");

    debug << "GetCallback: " << (void*)GetCallback << std::endl;
    debug << "FreeLastCallback: " << (void*)FreeLastCallback << std::endl;

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

    int ret = Workshop_Func(argv, argc-1);

    g_pEngine->ReleaseUser(steamPipe, userHdl);
    g_pEngine->BReleaseSteamPipe(steamPipe);
    g_pEngine->BShutdownIfAllPipesClosed();
    return ret;
}
