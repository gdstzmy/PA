#include <common.h>
void __am_gpu_config(AM_GPU_CONFIG_T *cfg);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl);
void __am_gpu_init();

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif
int screen_w=0,screen_h=0;

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  // Log("print_out");
  char *buf_c = (char*)buf;
  for(int i=0; i<len; i++)
    putch(buf_c[i]);
  return 0;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  char* buf_c = (char*)buf;
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  int siz = sprintf(buf_c,"%s %s", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  return siz;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  char* buf_c = (char*)buf;
  int siz=sprintf(buf_c,"WIDTH:%d\nHEIGHT:%d\n",cfg.width,cfg.height);
  Log("read_gpu_info:%s",buf_c);
  return siz;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  uint32_t* buf_uint = (uint32_t*)buf;
  AM_GPU_FBDRAW_T ctl;
  ctl.x = ((offset >> 32) & 0xFFFF);
  ctl.y = ((offset >> 0) & 0xFFFF);
  ctl.w = ((len >> 32) & 0xFFFF);
  ctl.h = ((len >> 0) & 0xFFFF);
  ctl.pixels = buf_uint;
  ctl.sync = 1;
  // Log("write %d %d %d %d\n",ctl.w,ctl.h,ctl.x,ctl.y);
  __am_gpu_fbdraw(&ctl);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
  __am_gpu_init();
  Log("finish initialize device 1");

  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  screen_w=cfg.width, screen_h=cfg.height;
  Log("finish initialize device 2");
}
