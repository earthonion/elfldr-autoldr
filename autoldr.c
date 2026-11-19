#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ps5/kernel.h>

#include "elfldr.h"
#include "log.h"
#include "notify.h"
#include "pt.h"


static const char *search_paths[] = {
  "/mnt/usb0/", "/mnt/usb1/", "/mnt/usb2/", "/mnt/usb3/",
  "/mnt/usb4/", "/mnt/usb5/", "/mnt/usb6/", "/mnt/usb7/",
  "/data/",
  "/mnt/usb0/ps5_autoloader/", "/mnt/usb1/ps5_autoloader/", "/mnt/usb2/ps5_autoloader/", "/mnt/usb3/ps5_autoloader/",
  "/mnt/usb4/ps5_autoloader/", "/mnt/usb5/ps5_autoloader/", "/mnt/usb6/ps5_autoloader/", "/mnt/usb7/ps5_autoloader/",
  "/data/ps5_autoloader/",
  NULL
};


static int
spawn_elf_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if(!f) {
    LOG_PUTS("Failed to open ELF");
    return -1;
  }

  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  uint8_t *buf = malloc(size);
  if(!buf) {
    fclose(f);
    return -1;
  }

  if(fread(buf, 1, size, f) != size) {
    free(buf);
    fclose(f);
    return -1;
  }
  fclose(f);

  if(elfldr_sanity_check(buf, size)) {
    LOG_PUTS("ELF sanity check failed");
    free(buf);
    return -1;
  }

  char *const argv[] = {(char*)path, 0};
  pid_t pid = elfldr_spawn(-1, argv, buf, size);
  free(buf);

  return pid < 0 ? -1 : 0;
}


static int
process_autoload(const char *basepath) {
  char filepath[256];
  char line[256];

  snprintf(filepath, sizeof(filepath), "%sautoload.txt", basepath);

  FILE *f = fopen(filepath, "r");
  if(!f)
    return -1;

  LOG_PUTS("Found autoload.txt in");
  LOG_PUTS(basepath);
  notify("autoldr: loading from %s", basepath);

  while(fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\r\n")] = 0;

    if(line[0] == '\0')
      continue;

    if(line[0] == '!') {
      int ms = atoi(line + 1);
      if(ms > 0)
        usleep(ms * 1000);
      continue;
    }

    char elfpath[256];
    snprintf(elfpath, sizeof(elfpath), "%s%s", basepath, line);

    LOG_PUTS("Spawning");
    LOG_PUTS(elfpath);
    notify("autoldr: spawning %s", line);

    if(spawn_elf_file(elfpath)) {
      notify("autoldr: failed to spawn %s", line);
    }
  }

  fclose(f);
  return 0;
}


int
main() {
  pid_t mypid = getpid();
  uint8_t caps[16];
  uint64_t authid;
  intptr_t vnode;

  signal(SIGCHLD, SIG_IGN);

  if(!(vnode=kernel_get_proc_rootdir(mypid)))
    return -1;
  if(kernel_get_ucred_caps(mypid, caps))
    return -1;
  if(!(authid=kernel_get_ucred_authid(mypid)))
    return -1;

  if(elfldr_raise_privileges(mypid)) {
    LOG_PUTS("Unable to raise privileges");
    return -1;
  }

  for(int i = 0; search_paths[i]; i++) {
    if(!process_autoload(search_paths[i]))
      break;
  }

  kernel_set_proc_jaildir(mypid, vnode);
  kernel_set_proc_rootdir(mypid, vnode);
  kernel_set_ucred_caps(mypid, caps);
  kernel_set_ucred_authid(mypid, authid);

  return 0;
}
