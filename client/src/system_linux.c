#include <system_linux.h>

int system_gnome_lock()
{
	return system("gnome-screensaver-command -l");
}

int system_gnome_unlock()
{
	return system("gnome-screensaver-command -u");
}

int system_kde_lock()
{
	return system("loginctl lock-session");
}

int system_kde_unlock()
{
	return system("loginctl unlock-session");
}