#include <glib.h>

int
main(void)
{
    char* arguments[] = {"pkexec", "nekofi-client"};
    g_spawn_sync("/", arguments, g_get_environ(), G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, NULL, NULL, NULL);
    return 0;
}