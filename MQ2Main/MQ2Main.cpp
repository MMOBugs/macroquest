/*****************************************************************************
MQ2Main.dll: MacroQuest's extension DLL for EverQuest
    Copyright (C) 2002-2003 Plazmic, 2003-2004 Lax

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
******************************************************************************/

// Exclude rarely-used stuff from Windows headers
#if !defined(CINTERFACE)
#error /DCINTERFACE
#endif

#define DBG_SPEW
#define DEBUG_TRY

#include "MQ2Main.h"

#ifdef EQLIB_EXPORTS
#pragma message("EQLIB_EXPORTS")
#else
#pragma message("EQLIB_IMPORTS")
#endif

DWORD WINAPI MQ2Start(LPVOID lpParameter);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    CHAR szFilename[MAX_STRING]={0};
    //CHAR ClientName[MAX_STRING]={0};
    PCHAR szProcessName;
    ghModule = (HMODULE)hModule;
    ghInstance = (HINSTANCE)hModule;

    GetModuleFileName(ghModule,szFilename,MAX_STRING);
    szProcessName = strrchr(szFilename,'\\');
    szProcessName[0] = '\0';
    strcat(szFilename,"\\eqgame.ini");
//    GetPrivateProfileString("MacroQuest","ClientName","eqgame",ClientName,MAX_STRING,szFilename);

    GetModuleFileName(NULL,szFilename,MAX_STRING);

    szProcessName = strrchr(szFilename,'.');
    szProcessName[0] = '\0';
    szProcessName = strrchr(szFilename,'\\')+1;
    if (!stricmp(szProcessName,__ClientName)) {
        if (ul_reason_for_call == DLL_PROCESS_ATTACH) {

            GetModuleFileName(ghModule,szFilename,MAX_STRING);
            szProcessName = strrchr(szFilename,'\\');
            szProcessName[0] = '\0';
            g_Loaded = TRUE;
            CreateThread(NULL,0,&MQ2Start,strdup(szFilename),0,&ThreadID);
        } else if (ul_reason_for_call == DLL_PROCESS_DETACH){
            gbUnload=TRUE;
            //Fuck waiting for us to cleanly exit... we're being unloaded!
            //while (g_Loaded) Sleep(20);
            return TRUE;
        }
    }
    return TRUE;
}


// ***************************************************************************
// Function:    ParseINIFile
// Description: Parse INI file for memory locations
// ***************************************************************************
BOOL ParseINIFile(PCHAR lpINIPath)
{
    CHAR Filename[MAX_STRING] = {0};
    CHAR MQChatSettings[MAX_STRING] = {0};
    CHAR CustomSettings[MAX_STRING] = {0};
    CHAR ClientINI[MAX_STRING] = {0};
    CHAR szBuffer[MAX_STRING] = {0};
    CHAR ClientName[MAX_STRING] = {0};
    CHAR AliasList[MAX_STRING*10] = {0};
//    CHAR HotkeyList[MAX_STRING*10] = {0};
    CHAR FilterList[MAX_STRING*10] = {0};
//    struct _stat stat;
//    int client;
//    BOOL clientOverride = FALSE;
   GetEQPath(gszEQPath);


    sprintf(Filename,"%s\\MacroQuest.ini",lpINIPath);
    sprintf(ClientINI,"%s\\eqgame.ini",lpINIPath);
    strcpy(gszINIFilename,Filename);

    DebugSpewAlways("Expected Client version: %s %s",__ExpectedVersionDate,__ExpectedVersionTime);
    DebugSpewAlways("    Real Client version: %s %s",__ActualVersionDate,__ActualVersionTime);

	if (strncmp(__ExpectedVersionDate,(const char *)__ActualVersionDate,strlen(__ExpectedVersionDate)) ||
		strncmp(__ExpectedVersionTime,(const char *)__ActualVersionTime,strlen(__ExpectedVersionTime)))
	{
        MessageBox(NULL,"Incorrect client version","MacroQuest",MB_OK);
        return FALSE;
	}
/*
	strcpy(ClientName,__ClientName);
	strcat(ClientName,".exe");
    client = _open(ClientName,_O_RDONLY);
    if (client == -1) {
        sprintf(szBuffer,"Unable to check client version. (%d)",errno);
        MessageBox(NULL,szBuffer,"MacroQuest",MB_OK);
        return FALSE;
    }
    _fstat(client,&stat);
    _close(client);

    DebugSpewAlways("Expected Client version: %s",__ClientVersion);
    DebugSpewAlways("    Real Client version: %s",ctime(&stat.st_mtime));
    if (!__ClientOverride && !CompareTimes(ctime(&stat.st_mtime),__ClientVersion)) {
        sprintf(szBuffer,"Incorrect client version:\n%s",ctime(&stat.st_mtime));
        MessageBox(NULL,__ClientVersion,"MacroQuest",MB_OK);
        return FALSE;
    }
/**/

	gFilterSkillsAll = 0!=GetPrivateProfileInt("MacroQuest","FilterSkills",0,Filename);
    gFilterSkillsIncrease = 2==GetPrivateProfileInt("MacroQuest","FilterSkills",0,Filename);
    gFilterDebug  = 1==GetPrivateProfileInt("MacroQuest","FilterDebug",0,Filename);
    gFilterTarget = 1==GetPrivateProfileInt("MacroQuest","FilterTarget",0,Filename);
    gFilterMoney  = 1==GetPrivateProfileInt("MacroQuest","FilterMoney",0,Filename);
    gFilterFood   = 1==GetPrivateProfileInt("MacroQuest","FilterFood",0,Filename);
    gFilterMacro  = GetPrivateProfileInt("MacroQuest","FilterMacro",0,Filename);
    gFilterEncumber=1==GetPrivateProfileInt("MacroQuest","FilterEncumber",0,Filename);
    gFilterCustom = 1==GetPrivateProfileInt("MacroQuest","FilterCustom",1,Filename);
    gSpewToFile   = 1==GetPrivateProfileInt("MacroQuest","DebugSpewToFile",0,Filename);
    gMQPauseOnChat= 1==GetPrivateProfileInt("MacroQuest","MQPauseOnChat",0,Filename);
    gKeepKeys     = 1==GetPrivateProfileInt("MacroQuest","KeepKeys",0,Filename);

	gFilterSWho.Lastname= GetPrivateProfileInt("SWho Filter","Lastname",1,Filename);
	gFilterSWho.Class	= GetPrivateProfileInt("SWho Filter","Class",1,Filename);
	gFilterSWho.Race	= GetPrivateProfileInt("SWho Filter","Race",1,Filename);
	gFilterSWho.Level	= GetPrivateProfileInt("SWho Filter","Level",1,Filename);
	gFilterSWho.GM		= GetPrivateProfileInt("SWho Filter","GM",1,Filename);
	gFilterSWho.Guild	= GetPrivateProfileInt("SWho Filter","Guild",1,Filename);
    gFilterSWho.Sneak   = GetPrivateProfileInt("SWho Filter","Sneak",1,Filename); 
	gFilterSWho.LD		= GetPrivateProfileInt("SWho Filter","LD",1,Filename);
	gFilterSWho.LFG		= GetPrivateProfileInt("SWho Filter","LFG",1,Filename);
	gFilterSWho.NPCTag	= GetPrivateProfileInt("SWho Filter","NPCTag",1,Filename);
	gFilterSWho.Trader	= GetPrivateProfileInt("SWho Filter","Trader",1,Filename);
	gFilterSWho.AFK		= GetPrivateProfileInt("SWho Filter","AFK",1,Filename);
	gFilterSWho.Anon	= GetPrivateProfileInt("SWho Filter","Anon",1,Filename);
	gFilterSWho.Distance= GetPrivateProfileInt("SWho Filter","Distance",1,Filename);
	gFilterSWho.Light	= GetPrivateProfileInt("SWho Filter","Light",0,Filename);
	gFilterSWho.Body	= GetPrivateProfileInt("SWho Filter","Body",0,Filename);
	gFilterSWho.SpawnID = GetPrivateProfileInt("SWho Filter","SpawnID",0,Filename);
	gFilterSWho.Holding = GetPrivateProfileInt("SWho Filter","Holding",0,Filename);
	gFilterSWho.ConColor= GetPrivateProfileInt("SWho Filter","ConColor",0,Filename);
	gFilterSWho.Invisible= GetPrivateProfileInt("SWho Filter","Invisible",0,Filename);

    GetPrivateProfileString("MacroQuest","MacroPath",".",szBuffer,MAX_STRING,Filename);
    if (szBuffer[0]=='.') {
        sprintf(gszMacroPath,"%s%s",lpINIPath,szBuffer+1);
    } else {
        strcat(gszMacroPath,szBuffer);
    }

    GetPrivateProfileString("MacroQuest","LogPath",".",szBuffer,MAX_STRING,Filename);
    if (szBuffer[0]=='.') {
        sprintf(gszLogPath,"%s%s",lpINIPath,szBuffer+1);
    } else {
        strcat(gszLogPath,szBuffer);
    }

    DefaultFilters();
    GetPrivateProfileString("Filter Names",NULL,"",FilterList,MAX_STRING*10,Filename);
    PCHAR pFilterList = FilterList;
    while (pFilterList[0]!=0) {
        GetPrivateProfileString("Filter Names",pFilterList,"",szBuffer,MAX_STRING,Filename);
        if (szBuffer[0]!=0 && strcmp(szBuffer,"NOBODY")) {
            AddFilter(szBuffer,-1,&gFilterCustom);
        }
        pFilterList+=strlen(pFilterList)+1;
    }
/*
    GetPrivateProfileString("Hotkeys",NULL,"",HotkeyList,MAX_STRING*10,Filename);
    PCHAR pHotkeyList = HotkeyList;
    while (pHotkeyList[0]!=0) {
        GetPrivateProfileString("Hotkeys",pHotkeyList,"",szBuffer,MAX_STRING,Filename);
        if (szBuffer[0]!=0) {
            PHOTKEY pNew = (PHOTKEY)malloc(sizeof(HOTKEY));
            strcpy(pNew->szName,pHotkeyList);
            ConvertHotkeyNameToKeyName(pNew->szName);
            DWORD i;
            for (i=0;gDiKeyID[i].szName[0];i++) {
                if (!stricmp(gDiKeyID[i].szName,pNew->szName)) {
                    pNew->DIKey = gDiKeyID[i].Id;
                }
            }
            strcpy(pNew->szCommand,szBuffer);
            pNew->pNext=pHotkey;
            pHotkey=pNew;
        }
        pHotkeyList+=strlen(pHotkeyList)+1;
    }
/**/

	sprintf(Filename,"%s\\ItemDB.txt",lpINIPath);
    FILE *fDB = fopen(Filename,"rt");
    strcpy(gszItemDB,Filename);
    if (fDB) {
        fgets(szBuffer,MAX_STRING,fDB);
        while ((!feof(fDB)) && (strstr(szBuffer,"\t"))) {
            PITEMDB Item = (PITEMDB)malloc(sizeof(ITEMDB));
            Item->pNext = gItemDB;
            Item->ID = atoi(szBuffer);
            strcpy(Item->szName,strstr(szBuffer,"\t")+1);
            Item->szName[strstr(Item->szName,"\n")-Item->szName]=0;
            gItemDB = Item;
            fgets(szBuffer,MAX_STRING,fDB);
        }
        fclose(fDB);
    }

    return TRUE;
}


// ***************************************************************************
// Function:    MQ2Start
// Description: Where we start execution during the insertion
// ***************************************************************************
DWORD WINAPI MQ2Start(LPVOID lpParameter)
{
    PCHAR lpINIPath = (PCHAR)lpParameter;
    strcpy(gszINIPath, lpINIPath);
    CHAR szBuffer[MAX_STRING] = {0};
    if (!ParseINIFile(lpINIPath)) {
        DebugSpewAlways("ParseINIFile returned false - thread aborted.");
        g_Loaded = FALSE;
        free(lpINIPath);
        return 1;
    }

    srand(time(0));
	ZeroMemory(gDiKeyName,sizeof(gDiKeyName));
	unsigned long i;
	for (i = 0 ; gDiKeyID[i].Id ; i++)
	{
		gDiKeyName[gDiKeyID[i].Id]=gDiKeyID[i].szName;
	}

	ZeroMemory(szEQMappableCommands,sizeof(szEQMappableCommands));
	for (i = 0 ; i < nEQMappableCommands && EQMappableCommandList[i] ; i++)
	{
		szEQMappableCommands[i]=EQMappableCommandList[i];
	}
	gnNormalEQMappableCommands=i;
	szEQMappableCommands[0xA1]="UNKNOWN0xA1";
	szEQMappableCommands[0xA2]="UNKNOWN0xA2";
	szEQMappableCommands[0xA3]="CHAT_SEMICOLON";
	szEQMappableCommands[0xA4]="CHAT_SLASH";
	szEQMappableCommands[0xA5]="UNKNOWN0xA5";
	szEQMappableCommands[0xA6]="UNKNOWN0xA6";
	szEQMappableCommands[0xA7]="INSTANT_CAMP";
	szEQMappableCommands[0xA8]="UNKNOWN0xA8";
	szEQMappableCommands[0xA9]="UNKNOWN0xA9";
	szEQMappableCommands[0xAA]="CHAT_EMPTY";
	szEQMappableCommands[0xAB]="TOGGLE_WINDOWMODE";
	szEQMappableCommands[0xAC]="UNKNOWN0xAC";
	szEQMappableCommands[0xAD]="UNKNOWN0xAD";
	szEQMappableCommands[0xAE]="CHANGEFACE";// maybe? something that requires models.
	szEQMappableCommands[0xAF]="UNKNOWN0xAF";
	szEQMappableCommands[0xB0]="UNKNOWN0xB0";
	szEQMappableCommands[0xB1]="UNKNOWN0xB1";
	szEQMappableCommands[0xB2]="UNKNOWN0xB2";
	szEQMappableCommands[0xB3]="UNKNOWN0xB3";
	szEQMappableCommands[0xB4]="UNKNOWN0xB4";


	InitializeMQ2Benchmarks();
	InitializeParser();
	InitializeMQ2Detours();
	InitializeDisplayHook();
	InitializeChatHook();
	InitializeMQ2Pulse();
	InitializeMQ2Commands();
	InitializeMQ2Windows();
	InitializeMQ2KeyBinds();
	InitializeMQ2Plugins();
	InitializeMQ2Spawns();

	while (gGameState != GAMESTATE_CHARSELECT && gGameState != GAMESTATE_INGAME) Sleep(500);
	InitializeMQ2DInput();


    WriteChatColor(LoadedString,USERCOLOR_DEFAULT);
    DebugSpewAlways(LoadedString);

    while (!gbUnload) {
        Sleep(500);
    }

    WriteChatColor(UnloadedString,USERCOLOR_DEFAULT);
    DebugSpewAlways(UnloadedString);
	UnloadMQ2Plugins();
	DebugTry(ShutdownMQ2KeyBinds());
	DebugTry(ShutdownMQ2Spawns());
	DebugTry(ShutdownDisplayHook());
	DebugTry(ShutdownMQ2DInput());
	DebugTry(ShutdownChatHook());
	DebugTry(ShutdownMQ2Pulse());
	DebugTry(ShutdownMQ2Plugins());
	DebugTry(ShutdownMQ2Windows());
	DebugTry(ShutdownParser());
	DebugTry(ShutdownMQ2Commands());
	DebugTry(ShutdownMQ2Detours());
	DebugTry(ShutdownMQ2Benchmarks());

	DebugSpew("Shutdown completed");
    free(lpINIPath);
    g_Loaded = FALSE;
    return 0;
}


#ifdef DEBUG_ALLOC
DWORD CountMallocs=0;
DWORD CountFrees=0;
#endif

#ifdef malloc
#undef malloc
#endif

void *MQ2Malloc(size_t size)
{
#ifdef DEBUG_ALLOC
	CountMallocs++;
#endif
	return malloc(size);
}

#ifdef free
#undef free
#endif

void MQ2Free(void *memblock)
{
#ifdef DEBUG_ALLOC
	CountFrees++;
#endif
	free(memblock);
}

