
#include <signal.h>
#include "Neptune.h"
#include "PltService.h"
#include "PltMRDelegate.h"

PLT_MplayDelegate *forsignal;


 /*check playstatus every secend*/
 static void* UpdateTrackTimeThread(void *param)
 {
	 NPT_String track_duration;
	 NPT_String track_pos;
 	 //int kkk = 1000000;
	 PLT_MplayDelegate *mp = (PLT_MplayDelegate*)param;
	 char result[64] = {0};
	 PLT_Service *service;
	 int ret;
	 time_t curtime;
	 time_t reltime;
	 int duration;
	 int pos;
	 float fduration;
	 float fpos;
	 NPT_String nexturl;
	 NPT_String state;
	 NPT_String lastchange;
	 track_duration.SetLength(16);
	 track_pos.SetLength(16);
 	 printf("enter UpdateTrackTimeThread\n");
	 ret = mp->pMediaRender->FindServiceByName("AVTransport",service);
	 if (ret != NPT_SUCCESS)
	 {
	 	printf("get server error ret=%d\n",ret);
		return NULL;
	 }
	
	 //service->SetStateVariableRate("CurrentTrackDuration",NPT_TimeInterval(0.2f) );
	 //service->SetStateVariableRate("RelativeTimePosition",NPT_TimeInterval(0.2f) );
	 while(1)
	 {
		 usleep(500000);  //500ms
		 
		 //service->GetStateVariableValue("LastChange",lastchange);
		 //printf("lastchange:%s\n",(char*)lastchange);
		 
		 service->GetStateVariableValue("TransportState", state);
    	 if(state == "TRANSITIONING")
        	 continue;
		 
		 //pthread_mutex_lock(&mutex);
		 ret = mp->GetPosition(track_duration,track_pos);
		 if (ret == NPT_SUCCESS)
		 {
		 	 ret = service->SetStateVariable("TransportState", "PLAYING");
		 	 ret = service->SetStateVariable("TransportStatus", "OK");
			 ret = service->SetStateVariable("NumberOfTracks", "1");
        	 ret = service->SetStateVariable("CurrentTrack", "1");
			 ret = service->SetStateVariable("TransportPlaySpeed", "1");
			 ret = service->SetStateVariable("AVTransportURI" , mp->Url);
        	 ret = service->SetStateVariable("CurrentTrackURI", mp->Url);
			 //track_duration.ToInteger(duration,true);
			 track_duration.ToFloat(fduration,true);
			 track_pos.ToFloat(fpos,true);
			 fpos = fpos + 2.0;
			 if (fduration - fpos < 1) //»Áπ˚…Ÿ”⁄1s,÷±Ω”»√fpos==furation
			 {
				 fpos = fduration;
			 }
			 duration = (int)fduration;
			 mp->print_upnp_time(result,64,duration);
			 ret = service->SetStateVariable("CurrentTrackDuration",result );
          	 ret = service->SetStateVariable("CurrentMediaDuration",result);
		 
			 //track_pos.ToInteger(pos,true);
			 
			 pos = (int)fpos;
			 mp->print_upnp_time(result,64,pos);
			 ret = service->SetStateVariable("RelativeTimePosition",result );
			 
			 ret = service->SetStateVariable("AbsoluteTimePosition",result );
		     
			 if (ret != NPT_SUCCESS)
			 {
				printf("fuck error\n");
			 }
			 //printf("RelativeTimePosition %s\n",result);
		 }
		 //else
		 //{
		//	 service->GetStateVariableValue("NextURI",nexturl);
			 //mp->SetPlayStatus(MPLAYER_STOP);
		//	 mp->Stop();
		//	 printf("nexturl %s\n",(char*)nexturl);
		 //}
		 /*
		 if (MPLAYER_NEXT == playstatus && play_trans_callback_)
		 {
			 playstatus = MPLAYER_STOP;
			 play_trans_callback_(PLAY_STARTED_NEXT_STREAM);
		 }*/
		 
		 //pthread_mutex_unlock(&mutex);
	 }
	 return NULL;
 }


void signalhandler(int signumber)
{
	//NPT_String value;
	forsignal->SwitchPause();
	return ;
}

int PLT_MplayDelegate::SwitchPause()
{
	if (GetPlayStatus() == MPLAYER_PLAYING)
	{
		pthread_mutex_lock(&(forsignal->mutex));
		SetPlayStatus(MPLAYER_PAUSE);
		WriteCMDToMPlayer("pause");
		pthread_mutex_unlock(&(forsignal->mutex));
	}
	else if (GetPlayStatus() == MPLAYER_PAUSE)
	{
		pthread_mutex_lock(&(forsignal->mutex));
		SetPlayStatus(MPLAYER_PLAYING);
		WriteCMDToMPlayer("pause");
		pthread_mutex_unlock(&(forsignal->mutex));
	}
	return 0;
}


 int PLT_MplayDelegate::GetPosition(NPT_String &track_duration,
					 NPT_String &track_pos) {
    char buf[256] = {0};
    int ret = 0;
	float fduration;
	 float fpos;
	track_duration = LastDuration ;
	track_pos = LastPoistion;
	
    pthread_mutex_lock(&mutex);
	
	if (GetPlayStatus() != MPLAYER_PLAYING) {
	    pthread_mutex_unlock(&mutex);
		return -2;  
	}
	
    WriteCMDToMPlayer("get_property time_pos");
    while(ret = GetLine(buf,256,800))
    {
        //printf("output_mplayer_get_position1:%s",buf);
        /* Ù–‘≤ª¥Ê‘⁄£¨‘ÚÀµ√˜≤•∑≈ÕÍ≥…ªÚ’ﬂŒ¥∆Ù∂Ø*/
        if(strncmp(buf,"ANS_ERROR",strlen("ANS_ERROR")) == 0 || 
          strncmp(buf,"Failed to get value of property",strlen("Failed to get value of property")) == 0)
        {
        	//“ÚŒ™OSS«˝∂Øµƒ£¨mplayerªÒ»°µƒŒª÷√”–¥Û∏≈3~5s≤Ó÷µ£ª
        	//±‹√‚Ω¯∂»Ãı‘⁄◊Ó∫Ûº∏√Îø®À¿£
        	LastDuration.ToFloat(fduration,true);
			LastPoistion.ToFloat(fpos,true);
			if (fduration == fpos)
			{
				pthread_mutex_unlock(&mutex);
				return -1;
			}
			if (fduration - fpos < 3)
			{
				track_pos = LastPoistion = LastDuration;
				pthread_mutex_unlock(&mutex);
				return NPT_SUCCESS;
			}
			pthread_mutex_unlock(&mutex);
            return -1;
        }
        if(strncmp(buf,"ANS_time_pos",strlen("ANS_time_pos")) == 0)
            break;
        else
        {
            continue;
            //pthread_mutex_unlock(&mutex);
            //return rc;
        }
        
        bzero(buf,256);
    }
    if (ret == 0)
    {
        printf("output_mplayer_get_position timeout \n");
		pthread_mutex_unlock(&mutex);
        return -1;
    }
    sscanf(buf,"ANS_time_pos=%s",(char*)track_pos);
    LastPoistion = track_pos;
    WriteCMDToMPlayer("get_property length");
    //while (fgets(buf,256,gpipe))
    while (ret = GetLine(buf,256,800))
    {
        //printf("output_mplayer_get_position2:%s",buf);
        /* Ù–‘≤ª¥Ê‘⁄£¨‘ÚÀµ√˜≤•∑≈ÕÍ≥…ªÚ’ﬂŒ¥∆Ù∂Ø*/
        if(strncmp(buf,"ANS_ERROR",strlen("ANS_ERROR")) == 0|| 
          strncmp(buf,"Failed to get value of property",strlen("Failed to get value of property")) == 0)
        {
        	//“ÚŒ™OSS«˝∂Øµƒ£¨mplayerªÒ»°µƒŒª÷√”–¥Û∏≈3~5s≤Ó÷µ£ª
        	//±‹√‚Ω¯∂»Ãı‘⁄◊Ó∫Ûº∏√Îø®À¿£
        	LastDuration.ToFloat(fduration,true);
			LastPoistion.ToFloat(fpos,true);
			if (fduration == fpos)
			{
				pthread_mutex_unlock(&mutex);
				return -1;
			}
			if (fduration - fpos < 3)
			{
				track_pos = LastPoistion = LastDuration;
				pthread_mutex_unlock(&mutex);
				return NPT_SUCCESS;
			}
			pthread_mutex_unlock(&mutex);
            return -1;
        }
        if(strncmp(buf,"ANS_length",strlen("ANS_length")) == 0)
            break;
        else
        {
            continue;
            //pthread_mutex_unlock(&mutex);
            //return rc;
        }
        bzero(buf,256);
    }
    if (ret == 0)
    {
        printf("output_mplayer_get_position timeout \n");
		pthread_mutex_unlock(&mutex);
        return -1;
    }
    sscanf(buf,"ANS_length=%s",(char*)track_duration);
    LastDuration = track_duration;
	pthread_mutex_unlock(&mutex);
	return NPT_SUCCESS;
}


PLT_MplayDelegate::PLT_MplayDelegate(PLT_MediaRenderer *pltMediaRender)
{
	pMediaRender = pltMediaRender;
	PlayStatus = MPLAYER_STOP;
}

NPT_Result PLT_MplayDelegate::Init()
{
	//pthread_t callback;
    int ret;
    //playstatus = MPLAYER_STOP;
    //mute = 0;
    pid_t pid;
	gpipe = -1;
    gmpipe = -1;
    Volume= "70.0";
	LastDuration="0.00000000";
	LastPoistion="0.00000000";
	Url = "";
	if (pipe2(FdPipe,O_NONBLOCK)<0 ) 
	//if (pipe(fd_pipe)<0 ) 
	{
		perror("pipe error\n");  
		return -1;
	}
	pid=fork();
	if(pid<0)  
	{
		perror("fork"); 
		return -1;
	}
	
	if(pid==0)                              //◊”Ω¯≥Ã≤•∑≈mplayer  
	{
		close(FdPipe[0]);
		dup2(FdPipe[1],1); 
		dup2(FdPipe[1],2); 
		/*
		execlp("mplayer","mplayer","-quiet","-slave",
		            "-cache","4096","-cache-min","20","-cache-seek-min","80","-input","file=/tmp/mfifo","-idle","-ao","alsa",
		            "-codecs-file","/etc/codecs.conf","-format","s16ne","-srate","44100","NULL");
		            */
		execlp("mplayer","mplayer","-quiet","-slave",
		            "-cache","4096","-cache-min","20","-cache-seek-min","80","-input","file=/tmp/mfifo","-idle","-ao","oss",
		            "-codecs-file","/etc/codecs.conf","-format","s16ne","NULL");
		perror("");
	}
	else
	{
		//forsignal = this;
		//signal(SIG_MY_MSG,&signalhandler);
		
		close(FdPipe[1]);
		gpipe = FdPipe[0];
		gmpipe = open("/tmp/mfifo",O_WRONLY);
		if (gmpipe == -1)
		{
			printf("Open mfifo failed");
			//pclose(gpipe);
			gpipe = -1;
			return -1;
		}
		ret = pthread_create(&PosThread,NULL,UpdateTrackTimeThread,this);
		if (0 != ret)	
		{
			printf("Create wait_callback failed");
			return -1;	
		}
		
		pthread_mutex_init(&mutex,NULL);
		//sleep(1);
		WriteCMDToMPlayer("loadfile NULL");
		//SetPlayStatus(MPLAYER_STOP);
	}
    //mplayer quiet -slave  -idle
    return NPT_SUCCESS;
}

#if 0
NPT_Result 
PLT_MplayDelegate::OnGetCurrentConnectionInfo(PLT_ActionReference& action)
{
	
	return NPT_SUCCESS;
}
#endif

NPT_Result 
PLT_MplayDelegate::OnGetCurrentConnectionInfo(PLT_ActionReference& action)
{
	printf("OnGetCurrentConnectionInfo\n");
	if (NPT_FAILED(action->VerifyArgumentValue("ConnectionID", "0"))) {
        action->SetError(706,"No Such Connection.");
        return NPT_FAILURE;
    }

    if (NPT_FAILED(action->SetArgumentValue("RcsID", "0"))){
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentValue("AVTransportID", "0"))) {
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentOutFromStateVariable("ProtocolInfo"))) {
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentValue("PeerConnectionManager", "/"))) {
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentValue("PeerConnectionID", "-1"))) {
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentValue("Direction", "Input"))) {
        return NPT_FAILURE;
    }
    if (NPT_FAILED(action->SetArgumentValue("Status", "Unknown"))) {
        return NPT_FAILURE;
    }

	return NPT_SUCCESS;
}

NPT_Result 
PLT_MplayDelegate::OnNext(PLT_ActionReference& action)
{
	printf("OnNext\n");
	return NPT_SUCCESS;
}

NPT_Result PLT_MplayDelegate::OnPause(PLT_ActionReference& action)
{
	//NPT_String value;
	if (GetPlayStatus() == MPLAYER_PLAYING)
	{
		pthread_mutex_lock(&mutex);
		//printf("OnPause\n");
		SetPlayStatus(MPLAYER_PAUSE);
		WriteCMDToMPlayer("pause");
		pthread_mutex_unlock(&mutex);
	}
	return NPT_SUCCESS;
}
void PLT_MplayDelegate::CheckAudiolock()
{
    FILE *fstreamer = NULL;
    char buff[16] = {0};
    /*read /tmp/audiolock*/
    if ((fstreamer = fopen("/tmp/audiolock","r+")) == NULL)
        return;
    fgets(buff,16,fstreamer);

    if (strncmp(buff,"shairport",9) == 0)
    {
        system("killall -9 shairport");
        //system("killall -9 audioserver");
        system("echo dlna > /tmp/audiolock");
        sleep(1);
        //system("/sbin/audiorestart.sh gmrender");
		//fprintf(fstreamer,"dlna");
    }
    
    fclose(fstreamer);
}

NPT_Result PLT_MplayDelegate::OnPlay(PLT_ActionReference& action)
{
	char cmd[512] = {0};
    char buf[256] = {0};
	int end;
    int ret = 0;
	NPT_String value;
	int len;
	PLT_Service *service;
	NPT_String scpd;
    pMediaRender->FindServiceByName("AVTransport",service);
	//if (GetPlayStatus() == MPLAYER_PLAYING)
	//    return NPT_SUCCESS;
	//NPT_CHECK_SEVERE(pMediaRender->FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", service));
	//NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURI", value));
    //NPT_CHECK_SEVERE(service->GetStateVariableValue("AVTransportURIMetaData", meta));
	
	printf("OnPlay\n");
	CheckAudiolock();
	pthread_mutex_lock(&mutex);
	if (GetPlayStatus() == MPLAYER_PAUSE)
	{
		if (WriteCMDToMPlayer("pause") < 0) /*resend pause to restart mplayer*/
		{
			pthread_mutex_unlock(&mutex);
    	    return -1;
		}
		SetPlayStatus(MPLAYER_PLAYING);
		pthread_mutex_unlock(&mutex);
		return NPT_SUCCESS;
	}

	if (GetPlayStatus() == MPLAYER_PLAYING)
	{
		WriteCMDToMPlayer("mute 1");
	}
	
	action->GetArgumentValue("CurrentURI",Url);
	printf("%s\n",(char*)Url);
	
	//printf("%s\n",(char*)value);
	//return NPT_SUCCESS;
	
    /*here set play status in stop,avaid get position read*/
    //playstatus = MPLAYER_STOP;
    SetPlayStatus(MPLAYER_STOP);
    //Stop();
	//if (write_cmd_to_mplayer("stop") < 0)
	//	return -1;
	LastDuration = "0.000000";
	LastPoistion = "0.000000";
	
	end = Url.Find(".txt");
	if (end == -1)
	{
		sprintf(cmd,"loadfile \"%s\"",(char*)Url);
	}
	else
	{
		sprintf(cmd,"loadlist \"%s\"",(char*)Url);
	}
	//WriteCMDToMPlayer(cmd);
	if (WriteCMDToMPlayer(cmd) < 0)
	{
		pthread_mutex_unlock(&mutex);
		service->SetStateVariable("TransportStatus", "ERROR_OCCURRED");
	    return -1;
	}
	sprintf(cmd,"volume %s.0 1",(char*)Volume);
    WriteCMDToMPlayer(cmd);
	/*wait for start to return*/
	//while (fgets(buf,256,gpipe))
	while (ret = GetLine(buf,256,10000))
    {
		//printf("%s",buf);
        if(strncmp(buf,"Starting playback",strlen("Starting playback")) == 0)
        {
            break;
        }
        bzero(buf,256);
    }
	
    if (ret == 0)
    {
        printf("get buffer timeout\n");
        pthread_mutex_unlock(&mutex);
		service->SetStateVariable("TransportStatus", "ERROR_OCCURRED");
        return -1;
    }
	sprintf(cmd,"volume %s.0 1",(char*)Volume);
    WriteCMDToMPlayer(cmd);
	
	system("echo 2 > /proc/audiofade");
    //playstatus = MPLAYER_PLAYING;
    SetPlayStatus(MPLAYER_PLAYING);
    pthread_mutex_unlock(&mutex);
    return NPT_SUCCESS;

}
NPT_Result PLT_MplayDelegate::OnPrevious(PLT_ActionReference& action)
{
	printf("OnPrevious\n");
	return NPT_SUCCESS;
}
NPT_Result PLT_MplayDelegate::OnSeek(PLT_ActionReference& action)
{
	NPT_String value;
	int seconds;
	char cmd[32] = {0};
	//printf("OnSeek\n");
	//action->GetArgumentValue("Unit",value);
	//printf("%s\n",(char*)value);
	pthread_mutex_lock(&mutex);
	action->GetArgumentValue("Target",value);
	//printf("%s\n",(char*)value);
	
	
	WriteCMDToMPlayer("pause");
	seconds = ParseUpnpTime(value);
	//system("echo 1 > /proc/audiofade");
	usleep(300000); 
	
	sprintf(cmd,"seek %d 2",seconds);
	WriteCMDToMPlayer(cmd);
	usleep(300000);
	WriteCMDToMPlayer("mute 0");
	//usleep(600000);
	//system("echo 2 > /proc/audiofade");
	//WriteCMDToMPlayer("mute 0");
	//usleep(300000);
	//WriteCMDToMPlayer("mute 0");
	//sprintf(cmd,"volume %s.0 1",(char*)Volume);
    //WriteCMDToMPlayer(cmd);
	pthread_mutex_unlock(&mutex);
	return NPT_SUCCESS;
}

NPT_Result PLT_MplayDelegate::OnStop(PLT_ActionReference& action)
{
	//return Stop();
	pthread_mutex_lock(&mutex);
	//printf("OnStop\n");
	
	
	SetPlayStatus(MPLAYER_STOP);
	WriteCMDToMPlayer("stop");
	pthread_mutex_unlock(&mutex);
	return NPT_SUCCESS;
}
/*
http://tsmusic128.tc.qq.com/XlFNM16/tExk9KmJSMt4711wRA2ywrEd86rAd6XyTGX5oygxhQ24cgMEiNJ8J/pG/OMHrph9V1F2mrZovg==/35576214.mp3?uid=313855626&amp;ct=0&amp;chid=0&amp;stream_pos=5
*/
NPT_Result PLT_MplayDelegate::OnSetAVTransportURI(PLT_ActionReference& action)
{
	PLT_Service *service;
    pMediaRender->FindServiceByName("AVTransport",service);
	pthread_mutex_lock(&mutex);
	//NPT_List<NPT_String> list;
	//NPT_Array<PLT_ArgumentDesc*> argumentdes;
	//PLT_ArgumentDesc * pargs;
	//int i;
	//PLT_ActionDesc adesc = action->GetActionDesc();

	//argumentdes = adesc.GetArgumentDescs();
	//printf("count=%d\n",argumentdes.GetItemCount());
	//for (i=0; i <argumentdes.GetItemCount(); i++)
	//{
		//pargs = argumentdes[i];
		//printf("%s\n",(char*)*it->GetName());
	//	printf("args:%s\n",(char*)argumentdes[i]->GetName());
	//}

	action->GetArgumentValue("CurrentURI",Url);
	action->GetArgumentValue("CurrentURIMetaData", Meta);

	//printf("OnSetAVTransportURI:%s\n",(char*)Url);
	//printf("CurrentURIMetaData:%s\n",(char*)Meta);
	//list = Url.Split("/");

	//for (NPT_List<NPT_String>::Iterator it = list.GetFirstItem(); it; ++it)
	//	printf("%s\n",(char*)*it);

	service->SetStateVariable("AVTransportURI", Url);
    service->SetStateVariable("AVTransportURIMetaData", Meta);
	pthread_mutex_unlock(&mutex);
	
	//printf("%s\n",(char*)Url);
	//printf("%s\n",(char*)Meta);
	return NPT_SUCCESS;
}
NPT_Result PLT_MplayDelegate::OnSetPlayMode(PLT_ActionReference& action)
{
	printf("OnSetPlayMode\n");
	return NPT_SUCCESS;
}


NPT_Result PLT_MplayDelegate::OnSetVolume(PLT_ActionReference& action)
{
	char cmd[32] = {0};
	PLT_Service *service;
	int ivol = 0;
	int old_ivol = 0;
	//printf("OnSetVolume\n");

	if (GetPlayStatus() != MPLAYER_PLAYING)
	    return NPT_SUCCESS;
	pthread_mutex_lock(&mutex);
	Volume.ToInteger(old_ivol,true);
	pMediaRender->FindServiceByName("RenderingControl",service);

	/*ªÒ»°“Ù¡ø≤¢≈‰÷√µΩmplayer*/
	action->GetArgumentValue("DesiredVolume",Volume);
	Volume.ToInteger(ivol,true);
	
	if (ivol <= old_ivol)
		ivol = ivol - (ivol % 3);
	else
		ivol = (ivol + 3) - (ivol % 3);
	
	Volume = Volume.Format("%d",ivol);
	
	//WriteCMDToMPlayer("mute 1");
	sprintf(cmd,"volume %s.0 1",(char*)Volume);
    WriteCMDToMPlayer(cmd);
	//usleep(500000);
    //WriteCMDToMPlayer("mute 0");
	/*–ﬁ∏ƒ∑˛ŒÒ“Ù¡ø*/
	service->SetStateVariable("Volume", Volume);
	pthread_mutex_unlock(&mutex);
	return NPT_SUCCESS;
}

NPT_Result PLT_MplayDelegate::OnSetVolumeDB(PLT_ActionReference& action)
{
	return NPT_ERROR_NOT_IMPLEMENTED;
	/*
	NPT_String value;
	printf("OnSetVolumeDB\n");
	if (GetPlayStatus() != MPLAYER_PLAYING)
	    return NPT_SUCCESS;
	action->GetArgumentValue("DesiredVolume",value);
	printf("%s\n",(char*)value);
	return NPT_SUCCESS;
	*/
}

NPT_Result PLT_MplayDelegate::OnGetVolumeDBRange(PLT_ActionReference& action)
{
		
	return NPT_ERROR_NOT_IMPLEMENTED;
}
NPT_Result PLT_MplayDelegate::OnSetMute(PLT_ActionReference& action)
{
	NPT_String value;
	NPT_String mvalue;
	PLT_Service *service;
	
	if (GetPlayStatus() != MPLAYER_PLAYING)
	    return NPT_SUCCESS;
	pthread_mutex_lock(&mutex);
	pMediaRender->FindServiceByName("RenderingControl",service);
	//service->GetStateVariableValue("Mute",mvalue);
	//printf("AVTransport mute %s\n",(char*)mvalue);
	
	action->GetArgumentValue("DesiredMute",value);
	printf("DesiredMute %s\n",(char*)value);
	WriteCMDToMPlayer("mute 1");
	service->SetStateVariable("Mute",value);
	pthread_mutex_unlock(&mutex);
	return NPT_SUCCESS;
}



