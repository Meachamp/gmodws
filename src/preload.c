
#include <stdio.h>
#include <dlfcn.h>
#include <string>
#include <sstream>
#include <Bootil/Bootil.h>
#include "SteamRemote.h"
#include "SteamUtils.h"

typedef SteamRemote*(*RemoteUtils_func)();
typedef SteamUtils*(*SteamUtils_func)();

RemoteUtils_func RemoteUtils;
SteamUtils_func AppUtils;

struct ArgFormat{
	int unk_1;
	int unk_2;
	char** args;
	int unk_3;
	int unk_4;
	unsigned int num_args;
};

const char* arg_func(ArgFormat* arg_base, int arg_num) {
	const char* none = "";
	const char* res = arg_base->args[arg_num-1];
	if(!res)
		return none;
		
	return res;
}

void Workshop_Func(int unk, ArgFormat* args) {
	printf("Loaded Workshop Command!\n");

	if(args->num_args < 2) {
		printf("Insufficient supplied arguments!\n");
		return;
	}


	std::string sWorkshopIdentifier = arg_func(args, 1);

	unsigned long id = 0;

	try {
		id = std::stol(sWorkshopIdentifier);
	} catch(...) {
		printf("Invalid workshop ID!\n");
		return;
	}

	std::string file = arg_func(args, 2);
	
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

	SteamRemote* remote = RemoteUtils();
	SteamUtils* utils = AppUtils();

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

__attribute__((constructor)) void Init() {
	void* dll = dlopen("steamconsole.so", RTLD_NOW);
	
	if(!dll) {
			printf("steamconsole not present!");
			return;
	}
	
	void* createIface = dlsym(dll, "CreateInterface");
	if(!createIface) {
			printf("CreateInterface function not found!\n");
			return;
	}
	
	RemoteUtils = (RemoteUtils_func)((char*)createIface - 0x23DE0);
	AppUtils = (SteamUtils_func)((char*)createIface - 0x23E20);
	
	typedef void(*Register_func)(void* buf, const char* command, void* func, const char* help, int, int, int, int);
	Register_func Register = (Register_func)((char*)createIface - 0x67A0);
	
	char* buf = new char[64];
	
	Register(buf, "gmod_update_ws", (void*)&Workshop_Func, "", 0, 0, 0, 0);

	return;
}
