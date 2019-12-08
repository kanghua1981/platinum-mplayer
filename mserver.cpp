/*****************************************************************
|
|   Platinum - Test UPnP A/V MediaServer
|
|
| Copyright (c) 2004-2010, Plutinosoft, LLC.
| All rights reserved.
| http://www.plutinosoft.com
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or (at your option) any later version.
|
| OEMs, ISVs, VARs and other distributors that combine and 
| distribute commercially licensed software with Platinum software
| and do not wish to distribute the source code for the commercially
| licensed software under version 2, or (at your option) any later
| version, of the GNU General Public License (the "GPL") must enter
| into a commercial license agreement with Plutinosoft, LLC.
| licensing@plutinosoft.com
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc., 
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-2.0.html
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "PltUPnP.h"
#include "PltAudioMediaServer.h"
#include "PltMediaRenderer.h"
#include "PltMRDelegate.h"

#include <stdlib.h>
#include <unistd.h>

NPT_SET_LOCAL_LOGGER("platinum.media.server.audio")

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
struct Options {
    const char* path;
    const char* friendly_name;
	
    NPT_UInt32  port;
	NPT_UInt32  enable_render;
	NPT_UInt32  enable_server;
} Options;

/*----------------------------------------------------------------------
|   PrintUsageAndExit
+---------------------------------------------------------------------*/
static void
PrintUsageAndExit()
{
    fprintf(stderr, "usage: AudioMediaServerTest [-f <friendly_name>] [-p <port>] [-r] [-s] [-b] <path>\n");
    fprintf(stderr, "-f : optional upnp device friendly name\n");
    fprintf(stderr, "-p : optional http port\n");
	fprintf(stderr, "-r : optional media render\n");
	fprintf(stderr, "-s : optional media server\n");
    fprintf(stderr, "<path> : local path to serve\n");
    exit(1);
}

/*----------------------------------------------------------------------
|   ParseCommandLine
+---------------------------------------------------------------------*/
static void
ParseCommandLine(char** args)
{
    const char* arg;

    /* default values */
    Options.path     = NULL;
    Options.friendly_name = NULL;
    Options.port = 0;

    while ((arg = *args++)) {
        if (!strcmp(arg, "-f")) {
            Options.friendly_name = *args++;
        } else if (!strcmp(arg, "-p")) {
            if (NPT_FAILED(NPT_ParseInteger32(*args++, Options.port))) {
                fprintf(stderr, "ERROR: invalid argument\n");
                PrintUsageAndExit();
            }
        }else if (!strcmp(arg, "-r")){
        	Options.enable_render = 1;
        }else if (!strcmp(arg, "-s")){
        	Options.enable_server = 1;
        }else if (Options.path == NULL) {
            Options.path = arg;
        } else {
            fprintf(stderr, "ERROR: too many arguments\n");
            PrintUsageAndExit();
        }
    }

    /* check args */
    if (Options.path == NULL) {
        fprintf(stderr, "ERROR: path missing\n");
        PrintUsageAndExit();
    }
}

/*----------------------------------------------------------------------
|   main
+---------------------------------------------------------------------*/
int
main(int /* argc */, char** argv)
{
    // setup Neptune logging
    NPT_LogManager::GetDefault().Configure("plist:.level=INFO;.handlers=ConsoleHandler;.ConsoleHandler.colors=off;.ConsoleHandler.filter=42");

    /* parse command line */
    ParseCommandLine(argv+1);
	PLT_UPnP upnp;
	PLT_UPnP upnp_render;
	/* for DLNA faster testing */
	//PLT_Constants::GetInstance().m_DefaultDeviceLease = 30.;
    if (Options.enable_server)
    {
	    
	    PLT_DeviceHostReference device(
	        new PLT_AudioMediaServer(
	            Options.path, 
	            Options.friendly_name?Options.friendly_name:"Platinum UPnP Media Server",
	            false,
	            "SAMEDEVICEGUID", // NULL for random ID
	            (NPT_UInt16)Options.port)
	            );

	    NPT_List<NPT_IpAddress> list;
	    NPT_CHECK_SEVERE(PLT_UPnPMessageHelper::GetIPAddresses(list));
	    NPT_String ip = list.GetFirstItem()->ToString();

	    device->m_ModelDescription = "Platinum File Media Server";
	    device->m_ModelURL = "http://www.plutinosoft.com/";
	    device->m_ModelNumber = "1.0";
	    device->m_ModelName = "Platinum File Media Server";
	    device->m_Manufacturer = "Plutinosoft";
	    device->m_ManufacturerURL = "http://www.plutinosoft.com/";

	    upnp.AddDevice(device);
	    NPT_String uuid = device->GetUUID();

	    NPT_CHECK_SEVERE(upnp.Start());
	}
	if (Options.enable_render)
	{
		NPT_LOG_INFO("start render.");
		
	    
		
		PLT_MediaRenderer *MRender=new PLT_MediaRenderer(Options.friendly_name?Options.friendly_name:"Platinum Media Renderer",
	                              false,
	                              "e6572b54-f3c7-2d91-2fb5-b757f2537e21");
		
		
		PLT_DeviceHostReference device_render(MRender);
	    upnp_render.AddDevice(device_render);
	    //bool added = true;

	    upnp_render.Start();
		
		PLT_MplayDelegate *MPlayDelegate = new PLT_MplayDelegate(MRender);
		
		if (MPlayDelegate->Init() < 0)
		{
			printf("Start Error\n");
			return -1;
		}
		MRender->SetDelegate(MPlayDelegate);
	}
    //NPT_LOG_INFO("Press 'q' to quit.");

    //char buf[256];
    while (1) {
        sleep(1);
    }

    upnp.Stop();
	upnp_render.Stop();

    return 0;
}
