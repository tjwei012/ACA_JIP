#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <libtecla.h>
#include "jip_cli.h"
#include "sqlite3.h"

#include <JIP.h>

/** JIP context structure */
extern tsJIP_Context sJIP_Context;

/** When non-zero, this global means the user is done using this program. */
extern int iQuit;

/** Port to conenct to */
extern int iPort;

/* Filter variables */
extern char *filter_ipv6;
extern char *filter_device;
extern char *filter_mib;
extern char *filter_var;

/* Connection details */
char *pcBorderRouterIPv6Address = "::0";
char *pcGatewayIPv4Address = "0.0.0.0";
int iUse_TCP = 0; /* UDP by default */

void CbJipNetworkChange(teJIP_NetworkChangeEvent eEvent, struct _tsNode *psNode)
{
	char str[32];
	switch(eEvent)
	{
	case E_JIP_NODE_JOIN:
		sprintf(str,"join ");
		break;
	case E_JIP_NODE_LEAVE:
		sprintf(str,"leave ");
		break;
	case E_JIP_NODE_MOVE:
		sprintf(str,"move ");
		break;
	default:
		break;
	}
	printf("xxxxxxxxxxxxxx node %s :ID %d\n",str,psNode->u32DeviceId);
//	jip_print_node(psNode,0);

}

int JipInit()
{
	uint32_t u32NumAddresses;
	tsJIPAddress *psAddresses;

    if (eJIP_Init(&sJIP_Context, E_JIP_CONTEXT_CLIENT) != E_JIP_OK)
    {
        printf("JIP startup failed\n\r");
        return -1;
    }
    if (eJIP_Connect4(&sJIP_Context, pcGatewayIPv4Address, pcBorderRouterIPv6Address, iPort, iUse_TCP) != E_JIP_OK)
	{
		printf("JIP connect failed\n\r");
		return -1;
	}

    eJIPService_MonitorNetwork(&sJIP_Context,CbJipNetworkChange);
    return 0;
}


int JipSet(char *value)
{
	 jip_ipv6 ("fd04:bd3:80e8:3:215:8d00:4e:2062");
	 jip_device ("0x08040001");
	 jip_mib ("0xfffffe04");
	 jip_var ("0");
	 if(0 == jip_set (value))
		 jip_discover(0);
	 return 0;
}


int JipInitServer()
{
	tsNode *psNode;
	tsMib *psMib;
	tsVar *psVar;

	if (eJIP_Init(&sJIP_Context, E_JIP_CONTEXT_SERVER) != E_JIP_OK)
	{
		printf("Error initialising server\n");
		return -1;
	}
	if (eJIPserver_NodeAdd(&sJIP_Context, "fd04:bd3:80e8:3:215:8d00:36:4d1d", JIP_DEFAULT_PORT, 0x80100001, "Test Coordinator", (char *)Version, &psNode) != E_JIP_OK)
	{
		printf("Error adding node\n");
		return -1;
	}
	/* Set Network table to be empty */
	psMib = psJIP_LookupMibId(psNode, NULL, 0xffffff01);
	if(psMib)
	{
		psVar = psJIP_LookupVarIndex(psMib, 4);

		if (psVar)
		{
			tsTable *psTable = malloc(sizeof(tsTable));
			if (psTable)
			{
				psTable->u32NumRows = 0;
				psTable->psRows     = NULL;

				psVar->pvData = psTable;
				printf("Empty network table initialised\n");
			}
		}
	}


}





