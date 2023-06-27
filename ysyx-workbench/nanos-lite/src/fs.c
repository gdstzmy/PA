#include <fs.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
void __am_gpu_config(AM_GPU_CONFIG_T *cfg);
typedef struct
{
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum
{
  FD_STDIN,
  FD_STDOUT,
  FD_STDERR,
  FD_FB,
  FD_EVENTS,
  PROC_DISPLAYINFO,
};

size_t invalid_read(void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len)
{
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, serial_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
    [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
    [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
    [PROC_DISPLAYINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs()
{
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  file_table[FD_FB].size = cfg.width * cfg.height * 4;
}

int fs_open(const char *pathname, int flags, int mode)
{
  for (int file_pos = 0;; file_pos++)
  {
    assert(file_table[file_pos].name != NULL);
    if (strcmp(pathname, file_table[file_pos].name) == 0)
    {
      file_table[file_pos].open_offset = 0;
      return file_pos;
    }
  }
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
  if(file_table[fd].read==NULL)
  {
    size_t read_len = len<file_table[fd].size-file_table[fd].open_offset? len:file_table[fd].size-file_table[fd].open_offset;
    // Log("fs_read: %d %d %d\n", file_table[fd].disk_offset, file_table[fd].open_offset, len < file_table[fd].size ? len : file_table[fd].size);
    size_t read_len_r = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
    file_table[fd].open_offset += read_len_r;
    return read_len_r;
  }
  else
  {
    int buf_len=file_table[fd].read(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += buf_len;
    // Log("readdevice length : %d\n",buf_len);
    return buf_len;
  }
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  if(file_table[fd].write==NULL)
  {
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset , len);
    file_table[fd].open_offset += len;
  }
  else
  {
    int buf_len=file_table[fd].write(buf, file_table[fd].open_offset, len);
    file_table[fd].open_offset += buf_len;
  }
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
  if (whence == SEEK_SET)
    file_table[fd].open_offset = offset;
  else if (whence == SEEK_CUR)
    file_table[fd].open_offset += offset;
  else if (whence == SEEK_END)
    file_table[fd].open_offset = file_table[fd].size;
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  return 0;
}
