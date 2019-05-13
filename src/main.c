#include <stdio.h>
#include <dlfcn.h>
#include <string>
#include <sstream>
#include <Bootil/Bootil.h>
#include "SteamRemote.h"
#include "SteamUtils.h"
#include "SteamEngine.h"
#include "SteamUser.h"
#include <chrono>

IEngine* g_pEngine;

time_t GetTime() {
	return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Workshop_Func(char** args, int num_args) {
	printf("Loaded Workshop Command!\n");
	
	if(num_args < 3) {
		printf("Insufficient supplied arguments!\n");
		return;
	}
	
	int steamPipe = 0;
	int userHdl = g_pEngine->CreateGlobalUser(&steamPipe);
	SteamUtils* utils = (SteamUtils*)g_pEngine->GetIClientUtils(steamPipe);
	SteamRemote* remote = (SteamRemote*)g_pEngine->GetIClientRemoteStorage(steamPipe,userHdl, "");
	SteamUser* user = (SteamUser*)g_pEngine->GetIClientUser(steamPipe,userHdl,"");
	
	g_pEngine->RunFrame();
	
	if(!utils) {
	    printf("Could not acquire Utils interface!");
	    return;
	}
	
	if(!remote) {
	    printf("Could not acquire Remote interface!");
	    return;
	}
	
	if(!user) {
	    printf("Could not acquire User interface!");
	    return;
	}
	
	int login_result = user->SetAccountNameForCachedCredentialLogin(args[1], 0);
	
	if(!login_result) {
		printf("Cached login credentials not available. Please login with steamCMD.\n");
		return;
	}
		
	user->RaiseConnectionPriority(2, 13);

	printf("Trying logon state....\n");
	
	long long test = 0;
	CSteamID s = user->GetSteamID();
	
	printf("STEAMID %llu\n", s.m_unAll64Bits);
	
	user->LogOn(s);
	
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
	
	printf("Logged in.\n");
	
	std::string sWorkshopIdentifier = args[2];

	unsigned long id = 0;

	try {
		id = std::stol(sWorkshopIdentifier);
	} catch(...) {
		printf("Invalid workshop ID!\n");
		return;
	}

	std::string file = args[3];
	
	Bootil::AutoBuffer buf;
	Bootil::AutoBuffer out;
	bool found = Bootil::File::Read(file, buf);

	if(!found) {
		printf("File not found!\n");
		return;
	}

	printf("Compressing file...\n");
	Bootil::Compression::LZMA::Compress(buf.GetBase(), buf.GetWritten(), out, 9, 0x2000000);
	int written = buf.GetWritten();
	out.SetPos(out.GetWritten());
	out.Write(&written, 4);
	out.Write("+-n+", 4);

	unsigned long crc = Bootil::Hasher::CRC32::Easy(out.GetBase(), out.GetWritten());

	file = std::to_string(crc) + std::string("_.gma");

	printf("Successfully compressed file. CRC: %u\n", crc);

	printf("Attempting to send file ....\n");

	remote->FileDelete(4000, 0, file.c_str());

	bool result = remote->FileWrite(4000, 0, file.c_str(), out.GetBase(), out.GetWritten());
	result ? printf("File written to cloud successfully!\n") : printf("File Write Failed!\n");

	printf("File Name: %s\n", file.c_str());
	unsigned long long api = remote->FileShare(4000, 0, file.c_str());

	if (!api) {
		printf("File Share Initialization failed!\n");
	}

	while (1) {
		if (utils->IsAPICallCompleted(api, &result))
			break;
	}

	result ? printf("File Share failed!\n") : printf("File Share completed successfully!\n");

	api = remote->CreatePublishedFileUpdateRequest(4000, id);

	if (!api) {
		printf("PublishFileUpdateRequest failed!");
	}

	result = remote->UpdatePublishedFileFile(api, file.c_str());
	
	if(num_args == 4) {
	    remote->UpdatePublishedFileSetChangeDescription(api, args[4]);
	}

	if (!result) {
		printf("File update failed.\n");
	} else {
		printf("File update succeeded!\n");
	}
		
	api = remote->CommitPublishedFileUpdate(4000, 0, api);

	while (1) {
		if (utils->IsAPICallCompleted(api, &result))
			break;
	}

	result ? printf("Commit failed!\n") : printf("Commit completed successfully!\n");
}

int main(int argc, char** argv) {
    printf("Loaded.\n");
	void* steam = dlopen("steamclient.so", RTLD_NOW);
	
	if(!steam) {
	    printf("steamclient not present!");
	    return 1;
	}
	
	typedef void*(*InterfaceFunc)(const char*, int);
	InterfaceFunc steamIface = (InterfaceFunc)dlsym(steam, "CreateInterface");
	
	if(!steamIface) {
	    printf("CreateInterface function not present!");
	    return 1;
	}
	
	g_pEngine = (IEngine*)steamIface("CLIENTENGINE_INTERFACE_VERSION005", 0);

	Workshop_Func(argv, argc-1);

	return 0;
}
