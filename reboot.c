#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  if (reboot() >= 0) {
    printf(2, "rebooting...\n");
  } else {
    printf(2, "sorry, I can't reboot.\n");
  }
  exit();
}
