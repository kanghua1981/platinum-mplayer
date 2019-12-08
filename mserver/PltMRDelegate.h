

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "PltMediaRenderer.h"

#define int64 long long

struct track_time_info {
    int64 duration;
    int64 position;
};

#define MPLAYER_PLAYING    0
#define MPLAYER_PAUSE      1
#define MPLAYER_STOP       2
#define MPLAYER_NEXT       3


class PLT_MplayDelegate : public PLT_MediaRendererDelegate
{
public:
    PLT_MplayDelegate(PLT_MediaRenderer *pltMediaRender);
    ~PLT_MplayDelegate(){};
    NPT_Result Init();
    NPT_Result OnGetCurrentConnectionInfo(PLT_ActionReference& action);
    // AVTransport
     NPT_Result OnNext(PLT_ActionReference& action);
     NPT_Result OnPause(PLT_ActionReference& action);
     NPT_Result OnPlay(PLT_ActionReference& action);
     NPT_Result OnPrevious(PLT_ActionReference& action);
     NPT_Result OnSeek(PLT_ActionReference& action);
     NPT_Result OnStop(PLT_ActionReference& action);
     NPT_Result OnSetAVTransportURI(PLT_ActionReference& action);
     NPT_Result OnSetPlayMode(PLT_ActionReference& action);

    // RenderingControl
     NPT_Result OnSetVolume(PLT_ActionReference& action);
     NPT_Result OnSetVolumeDB(PLT_ActionReference& action);
     NPT_Result OnGetVolumeDBRange(PLT_ActionReference& action);
     NPT_Result OnSetMute(PLT_ActionReference& action);
     
private:
    int GetLine(char *buffer,int size,int timeout)
    {
        char bufchar;
        char *localbuffer = buffer;
        int ret = 0;
        int count = 0;
        int timenum = 0;
        while ((count < size) && (timenum < timeout))
        {
            ret = read(gpipe,&bufchar,1);
            if (ret < 0)
            {
                if (errno == EAGAIN)
                {
                    //perror(""); 
                    usleep(1000);
                    timenum++;
                    continue;
                }
                perror("read failed\n");
            }
            else if (ret == 1)
            {
                if (bufchar == '\0')
                {
                    *localbuffer++ = bufchar;
                    break;
                }
                
                count++;
                *localbuffer++ = bufchar;
                if (bufchar == '\n')
                {
                    //localbuffer++;
                    *localbuffer = '\0';
                    break;
                }
                
            }
            
        }
        if (timenum == timeout)
        {
            /*保持和fgets统一的返回值*/
            printf("read timeout\n");
            return 0;
        }
        //printf("%s",buffer);
        
        return count;
    }
    void CheckAudiolock(void);
    
    int WriteCMDToMPlayer(char *cmd)
    {
        int ret = 0;
        char strcmd[512] = {0};
        sprintf(strcmd,"%s\n",cmd);
		
        //printf(strcmd);
        ret = write(gmpipe,strcmd,strlen(strcmd));
        if (ret < 0)
        {
            printf("write gmpipe failed : %s\n",cmd);
            return -1;
        }
        return NPT_SUCCESS;
    }

    int ParseUpnpTime( NPT_String time_string) 
    {
    	int hour = 0;
    	int minute = 0;
    	int second = 0;
    	sscanf((char*)time_string, "%02d:%02d:%02d", &hour, &minute, &second);
    	const int seconds = (hour * 3600 + minute * 60 + second);
    	return seconds;
    }
    
    // Print UPnP formatted time into given buffer. time given in nanoseconds.
    int divide_leave_remainder(int *val, int divisor) 
    {
    	int result = *val / divisor;
    	*val %= divisor;
    	return result;
    }
    
public:
    int SwitchPause(void);
    
    void print_upnp_time(char *result, size_t size, int t) 
    {
    	const int hour = divide_leave_remainder(&t, 3600);
    	const int minute = divide_leave_remainder(&t, 60);
    	const int second = divide_leave_remainder(&t, 1);
    	snprintf(result, size, "%d:%02d:%02d", hour, minute, second);
    }

    int GetPosition(NPT_String &track_duration,NPT_String &track_pos);

    int GetPlayStatus()
    {
        return PlayStatus;
    }

    int Stop()
    {
        //printf("OnStop\n");
    	SetPlayStatus(MPLAYER_STOP);
    	WriteCMDToMPlayer("stop");
    	return NPT_SUCCESS;
    }

    void SetPlayStatus(int status)
    {
        PLT_Service *service;
        pMediaRender->FindServiceByName("AVTransport",service);
        PlayStatus = status;
        if (status == MPLAYER_PLAYING)
        {
            service->SetStateVariable("TransportState", "PLAYING");
            service->SetStateVariable("TransportStatus", "OK");
            service->SetStateVariable("NumberOfTracks", "1");
        	service->SetStateVariable("CurrentTrack", "1");
            service->SetStateVariable("AVTransportURI", Url);
            service->SetStateVariable("AVTransportURIMetaData", Meta);
            service->SetStateVariable("TransportPlaySpeed", "1");
            //service->SetStateVariable("CurrentTransportActions", "PAUSE,STOP,SEEK");
            //service->SetStateVariable("TransportState", "PLAYING");
        }
        else if (status == MPLAYER_PAUSE)
        {
            service->SetStateVariable("TransportState", "PAUSED_PLAYBACK");
        }
        else
        {
            service->SetStateVariable("TransportState", "STOPPED");
            service->SetStateVariable("TransportPlaySpeed", "1");
            service->SetStateVariable("NumberOfTracks", "0");
            service->SetStateVariable("CurrentTrack", "0");
            service->SetStateVariable("RelativeTimePosition", "00:00:00");
            service->SetStateVariable("AbsoluteTimePosition", "00:00:00");
            service->SetStateVariable("CurrentTrackDuration", "00:00:00");
            service->SetStateVariable("CurrentMediaDuration", "00:00:00");
            //g_local_time_base
        }
    }
    //static void* UpdateTrackTimeThread(void *param);
    //void* WaitCallback(void *param);
    /*MPlayer control*/
    NPT_String Url;
    NPT_String Meta;
private:
    int FdPipe[2]; 
    int gpipe;
    int gmpipe;
    NPT_String Volume;
    pthread_t PosThread;
    pthread_mutex_t mutex;
    //struct track_time_info last_known_time_ = {0, 0};
    NPT_String LastDuration;
    NPT_String LastPoistion;
    
    int PlayStatus;
public:
    PLT_MediaRenderer *pMediaRender;
    
};


