#include <stdio.h>
#include <sys/utsname.h>

int main() {
    struct utsname sysInfo;
    uname(&sysInfo);

    printf("Hello World!\n");
    printf("System Name: %s\n", sysInfo.sysname);
    printf("Node Name: %s\n", sysInfo.nodename);
    printf("Machine: %s\n", sysInfo.machine);
    printf("Group Name: Group 20\n");
    printf("Student Name: Ashlesha Deokar\n");
    printf("Student Name: Aniket Raut\n");
    printf("Student Name: Mandar Chaudhari\n");
    printf("Student Name: Ganesh Madarasu\n");

    return 0;
}

