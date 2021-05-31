#include <ecafe.h>
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

int system_linux_notify(const char *title, const char *msg)
{
	char buf[2048];

	// @Danger : if message contains "" then bash will get confused (Escape the message first)

	sprintf(buf, "notify-send --app-name=%s --expire-time=10000 \"%s\" \"%s\"", ECAFE_APPNAME, title, msg);
	printf("%s\n", buf);

	return system(buf);
}

int system_linux_screenshot(const char *filename)
{
	char buf[1024];

	sprintf(buf, "import -window root -resize 1280x %s", filename);

	return system(buf);
}