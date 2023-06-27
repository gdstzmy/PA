#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  int time = gettimeofday(); //get us pa 3.3 TODO
  return time/1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r");
  assert(fp);
  memset(buf,'\0',len);
  fgets(buf,len,fp);
  int buf_len = strlen(buf);
  fclose(fp);
  return buf_len;
}

void NDL_OpenCanvas(int *w, int *h) {
  if((*w)==0 && (*h)==0)
  {
    FILE *fp = fopen("/proc/dispinfo", "r");
    assert(fp);
    int width=0,height=0;
    fscanf(fp, "WIDTH:%d\nHEIGHT:%d", &width, &height);
    (*w)=width,(*h)=height;
    fclose(fp);
  }
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  FILE *fp = fopen("/dev/fb", "w");
  size_t offset = (((size_t)x<<32) | (size_t)y);
  size_t len =  (((size_t)w<<32) | (size_t)h);
  fseek(fp, offset, SEEK_SET);
  write(3, pixels, len);
  fclose(fp);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
