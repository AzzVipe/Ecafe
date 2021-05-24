#ifndef __SYSTEM_LINUX_
#define __SYSTEM_LINUX_ 

#include <stdlib.h>

int system_gnome_lock();
int system_gnome_unlock();

int system_linux_lock();
int system_linux_unlock();

int system_linux_poweroff();

int system_linux_notify(const char *title, const char *msg);
int system_linux_screenshot(const char *filename);



#endif