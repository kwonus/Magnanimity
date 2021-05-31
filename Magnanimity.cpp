// Magnaminous.cpp : Defines the entry point for the application.
//

#include <Magnaminity.h>
#include <stdio.h>
#include <XVMem.h>

extern char  g_hSharedHome[MAX_PATH];

using namespace std;

static XVMem<UINT64> Magnaminous;

//  For every server, client, listener, the fist 8 utf8 bytes MUST BE UNIQUE from one another
//  Note: that name can be of all three modes (uniqueness is within each collection)
static const UINT64* initialize(char* folder, char* memmap, char* servers[], char* clients[], char* listeners[], UINT64 perRequestCnt)
{
    if (servers == NULL || clients == NULL || listeners == NULL || perRequestCnt == 0 || perRequestCnt == 0xFFFFFFFFFFFFFFFF)
        return false;
    int len = folder != NULL ? strnlen(folder, MAX_PATH) : 1;
    if (len >= MAX_PATH-1 && len > 0)
        return false;
    strncpy(g_hSharedHome, folder != NULL ? folder : ".", MAX_PATH);
    if (g_hSharedHome[len-1] != '/') {
        if (len >= MAX_PATH-2)
            return false;
        g_hSharedHome[len++] = '/';
    }
    UINT64 serverCnt = 0;
    UINT64 clientCnt = 0;
    UINT64 listenerCnt = 0;

    for (char** s = servers; *s != NULL; s++)
        serverCnt++;
    for (char** c = clients; *c != NULL; c++)
        clientCnt++;
    for (char** l = listeners; *l != NULL; l++)
        listenerCnt++;

    if (serverCnt == 0)
        return false;
    if (clientCnt == 0)
        return false;
    if (listenerCnt == 0 || listenerCnt > 64)
        return false;

    UINT64 slots = 4
    + (serverCnt * 2)
    + (listenerCnt * 2)
    + (serverCnt * (1 + listenerCnt))
    + (serverCnt * clientCnt * (2 + perRequestCnt));

    auto magnaminous = Magnaminous.Acquire(memmap != NULL ? memmap : "magnaminous.data", true, true, slots);

    //  DIMENENSIONS: read-only [all but magnanimity]
    magnaminous[0] = serverCnt;     // Ns
    magnaminous[1] = clientCnt;     // Nc
    magnaminous[2] = listenerCnt;   // Nl
    magnaminous[3] = perRequestCnt; // Nr
    
    auto position = 4;

    //  HEARTBEATS: servers and listeners are expected to manage their own heartbeat with GetTickCount() or similar
    for (char** item = servers; item != NULL; item++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *item;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        if (encoded == 0)
            return false;
        magnaminous[position++] = encoded;
        magnaminous[position++] = 0;    // server heartbeat
    }
    for (char** item = listeners; item != NULL; item++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *item;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        if (encoded == 0)
            return false;
        magnaminous[position++] = encoded;
        magnaminous[position++] = 0;    // listener heartbeat
    }
    //  SERVERS with LISTENERS: read-only [all but magnanimity]
    for (char** server = servers; server != NULL; server++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *server;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        magnaminous[position++] = encoded;  // server [ 1 thru Ns]

        for (char** listener = listeners; listener != NULL; listener++) {
            encoded = 0;
            e = (char*)&encoded;
            incoming = *listener;
            for (int i = 0; *incoming && i < 8; i++)
                *e++ = tolower(*incoming++);
            magnaminous[position++] = encoded;  // listener [ 1 thru Nl]
        }
    }
    //  REQUESTS:
    //  Ns-SERVERS // each with Nc-CLIENTS // each with Nr-Requests
    for (char** server = servers; server != NULL; server++) {
        UINT64 eserver = 0;
        char* e = (char*)&eserver;
        char* incoming = *server;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
 
        for (char** client = clients; client != NULL; client++) {
            UINT64 eclient = 0;
            e = (char*)&eclient;
            incoming = *client;
            for (int i = 0; *incoming && i < 8; i++)
                *e++ = tolower(*incoming++);
            magnaminous[position++] = eserver;  // server [ 1 thru Ns] (read-only)
            magnaminous[position++] = eclient;  // client [ 1 thru Nc] (read-only)
            for (int r = 0; r < perRequestCnt; r++) {
                magnaminous[position++] = 0;    // requestId (client-writable) [round-robin sequential / never zero (unused request) / never all-F (error condition)
                magnaminous[position++] = 0;    // serverCompletion heartbeat (server-writable)
                magnaminous[position++] = 0;    // Released (magnanimity-writable)
            }
        }
    }
}
void release()
{
    Magnaminous.Release();
}

int main(int argc, char* argv[])
{
    //  We should eventually read these from a yaml file ... but this will do for now v0.1
    char* folder = argc >= 2 ? argv[1] : NULL;
    char* memmap = argc >= 3 ? argv[2] : NULL;

    char* servers[] = { "avxlib.net", NULL };
    char* clients[] = { "avx4word", "avtext", NULL };
    char* listeners[] = { "avx4word", "avtext", NULL };
    UINT64 perRequestCount = 8;

    auto ok = initialize(folder, memmap, servers, clients, listeners, perRequestCount);

    if (ok)
	    cerr << "Magnanimity has been initialized" << endl;
    else
        cerr << "Magnanimity failed initialization" << endl;

	return ok ? 0 : -1;
}
