#include <system_linux.h>

int system_gnome_lock()
{
	return system("gnome-screensaver-command -l");
}

int system_gnome_unlock()
{
	return system("gnome-screensaver-command -u");
}

int system_linux_lock()
{
	return system("loginctl lock-session");
}

int system_linux_unlock()
{
	return system("loginctl unlock-session");
}

int system_linux_poweroff()
{
	return system("shutdown -f");
}