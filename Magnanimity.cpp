// Magnaminous.cpp : Defines the entry point for the application.
//

#include <Magnanimity.h>
#include <stdio.h>
#include <XVMem.h>

using namespace std;

static XVMem<UINT64> Magnaminous;

//  For every server, client, listener, the fist 8 utf8 bytes MUST BE UNIQUE from one another
//  Note: that name can be of all three modes (uniqueness is within each collection)
static const UINT64* initialize(char* folder, char* memmap, char* servers[], char* listeners[], UINT64 requestCnt)
{
    if (servers == NULL || listeners == NULL || requestCnt == 0 || requestCnt == 0xFFFFFFFFFFFFFFFF)
        return NULL;
    int len = folder != NULL ? strnlen(folder, MAX_PATH) : 1;
    if (len >= MAX_PATH-1 && len > 0)
        return NULL;
    strncpy(g_hSharedHome, folder != NULL ? folder : ".", MAX_PATH);
    if (g_hSharedHome[len-1] != '/') {
        if (len >= MAX_PATH-2)
            return false;
        g_hSharedHome[len++] = '/';
    }
    UINT64 serverCnt = 0;
    UINT64 listenerCnt = 0;

    for (char** s = servers; *s != NULL; s++)
        serverCnt++;
    for (char** l = listeners; *l != NULL; l++)
        listenerCnt++;

    if (serverCnt == 0)
        return NULL;
    if (listenerCnt == 0 || listenerCnt > 64)
        return NULL;

    UINT64 slots = 3
    + 2
    + (serverCnt * 2)
    + (listenerCnt * 2)
    + ((listenerCnt*requestCnt) * 3);

    auto magnaminous = Magnaminous.Acquire(memmap != NULL ? memmap : "magnaminous.data", true, true, slots, &zero, &zero, &zero);

    //  DIMENENSIONS: read-only [all but magnanimity]
    magnaminous[0] = serverCnt;  // Sn
    magnaminous[1] = listenerCnt;// Ln
    magnaminous[2] = requestCnt; // Rn
    magnaminous[3] = 0; // status-zero == initializing
    magnaminous[4] = 0; // don't mark heartbeat until right before setting status to 1

    auto position = 5;

    //  HEARTBEATS: servers and listeners are expected to manage their own heartbeat with GetTickCount() or similar
    for (char** item = servers; *item != NULL; item++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *item;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        if (encoded == 0)
            return NULL;
        magnaminous[position++] = encoded;
        magnaminous[position++] = 0;    // server heartbeat
    }
    for (char** item = listeners; *item != NULL; item++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *item;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        if (encoded == 0)
            return NULL;
        magnaminous[position++] = encoded;
        magnaminous[position++] = 0;    // listener heartbeat
    }
    //  SERVERS with LISTENERS: read-only [all but magnanimity]
    for (char** server = servers; *server != NULL; server++) {
        UINT64 encoded = 0;
        char* e = (char*)&encoded;
        char* incoming = *server;
        for (int i = 0; *incoming && i < 8; i++)
            *e++ = tolower(*incoming++);
        magnaminous[position++] = encoded;  // server [ 1 thru Ns]

        for (char** listener = listeners; *listener != NULL; listener++) {
            encoded = 0;
            e = (char*)&encoded;
            incoming = *listener;
            for (int i = 0; *incoming && i < 8; i++)
                *e++ = tolower(*incoming++);
            magnaminous[position++] = encoded;  // listener [ 1 thru Nl]
        }
    }
    //  REQUESTS:
    //  Ln*Rn // each with Listener gets Rn requests
    for (int c = 0; c < listenerCnt; c++) {
        for (int r = 0; r < requestCnt; r++) {
            magnaminous[position++] = 0;  // md5 of request
            magnaminous[position++] = 0;  // server-hash
            magnaminous[position++] = 0;  // server-response-timestamp
        }
    }
    return magnaminous;
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
    char* listeners[] = { "avx4word", "avtext", NULL };
    UINT64 perListenerRequestCount = 8;

    auto ok = initialize(folder, memmap, servers, listeners, perListenerRequestCount);

    if (ok)
	    cerr << "Magnanimity has been initialized" << endl;
    else
        cerr << "Magnanimity failed initialization" << endl;

    if (ok) {
        cout << "Type any key to quit > ";
        getchar();
    }
	return ok ? 0 : -1;
}
