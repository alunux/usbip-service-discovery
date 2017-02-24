#include "usbip.h"
#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    int ret = usbip_list();
    if (ret < 0) {
        printf("exit status %d\n", ret);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
