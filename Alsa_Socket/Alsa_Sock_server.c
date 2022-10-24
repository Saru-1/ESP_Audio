#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>	//For memset()
#include<netdb.h>	//For getaddrinfo()
#include<stdlib.h>	//For malloc()
#include<unistd.h>	//For fork()

#define IP_ADDRESS "192.168.68.200"

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

int main(){

long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints,*res;
	memset(&hints,0,sizeof(hints));
	getaddrinfo(IP_ADDRESS,"5000",&hints,&res);
	int s,b,l,new_s;
	s=socket(AF_INET,SOCK_STREAM,0);
	if(s==-1){
		printf("Error unable to create socket");
	}
	b=bind(s,res->ai_addr,res->ai_addrlen);
	if(b==-1){
		printf("Error unable to bind socket");
	}
	l=listen(s,10);
	if(l==-1){
		printf("Error not listening error");
	}
	
	addr_size=sizeof their_addr;
	new_s=accept(s,(struct sockaddr*)&their_addr,&addr_size);
	if(new_s==-1){
		printf("Error 2nd socket failed!");
	}
	printf("%d",size);
	
	/* 5 seconds in microseconds divided by
   * period time */

	loops = 5000000 / val;
	while (loops > 0) {
    		loops--;
		
		rc = recv(new_s, buffer, frames,0);
		if (rc == 0) {
		    fprintf(stderr, "end of file on input\n");
		    break;
		} else if (rc != size) {
		     fprintf(stderr,
	              "short read: read %d bytes\n", rc);
		}
		rc = snd_pcm_writei(handle, buffer, frames);
		if (rc == -EPIPE) {
	     /* EPIPE means underrun */
		     fprintf(stderr, "underrun occurred\n");
		     snd_pcm_prepare(handle);
		} else if (rc < 0) {
			fprintf(stderr,
		             "error from writei: %s\n",
	              snd_strerror(rc));
		}  else if (rc != (int)frames) {
			fprintf(stderr,
		           "short write, write %d frames\n", rc);
		}

	}
	close(new_s);

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
	return 0;
}
