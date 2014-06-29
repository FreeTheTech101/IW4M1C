// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: Notify server owners of an update.
//
// Initial author: redacted
// Started: 2013-12-08
// ==========================================================

#include "StdInc.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

static int lastUpdateCheck;
static int timeTillShutdown;
static bool warningGiven;

size_t UpdateInfoDataRecieved(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t rsize = (size * nmemb);
	char* text = (char*)ptr;
	int version = atoi(text);

	if (version > BUILDNUMBER)
	{
		if ((Com_Milliseconds() - timeTillShutdown) > 21600000)
		{
			// First, kick all of the players.
			for (int i = 0; i < *svs_numclients; i++)
			{
				client_t* client = &svs_clients[i];

				if (client->state < 3)
				{
					continue;
				}

				Cmd_ExecuteSingleCommand(0, 0, va("clientkick %i", client));
			}

			Com_Error(0, "This server version (%d) of IW4M1C has expired.\nPlease run dsut.exe", BUILDNUMBER);
		}


		Com_Printf(1, va("A new server version is available, please run dsut.exe"));

		if (!warningGiven)
			MessageBoxA(NULL, "A new server version is now available. Please shutdown this server and run dsut.exe\nThe server will automatically shutdown in 6 hours should you not update.", "Update available", MB_OK | MB_ICONINFORMATION);
			warningGiven = true;
			timeTillShutdown = Com_Milliseconds();

		SV_GameSendServerCommand(-1, 0, va("%c \"This server version (%d) of ^2IW4M1C^7 has expired.\"", 104, BUILDNUMBER));
		SV_GameSendServerCommand(-1, 0, va("%c \"Please visit http://quadrex.org (or run dsut.exe) to obtain a new version (%d)\"", 104, version));
	}

	return rsize;
}

void PatchMW2_CheckUpdate()
{
	return;

	if (IsDebuggerPresent()) return;

	if ((Com_Milliseconds() - lastUpdateCheck) > 120000)
	{
		curl_global_init(CURL_GLOBAL_ALL);

		CURL* curl = curl_easy_init();

		if (curl)
		{
			char url[255];
			_snprintf(url, sizeof(url), "http://iw4.quadrex.org/version.txt");

			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, UpdateInfoDataRecieved);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, VERSIONSTRING);
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

			CURLcode code = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			curl_global_cleanup();

			if (code == CURLE_OK)
			{
				lastUpdateCheck = Com_Milliseconds();
				return;
			}
			else
			{
				Com_Printf(1, "Could not reach the QuadRex server.\n");
				lastUpdateCheck = Com_Milliseconds();
			}
		}

		curl_global_cleanup();

		lastUpdateCheck = Com_Milliseconds();
	}
}