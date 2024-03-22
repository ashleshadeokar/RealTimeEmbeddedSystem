#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>

int main() {
    struct utsname sysname;
    uname(&sysname);
    printf("System Name : %s\n", sysname.sysname);
    printf("Node Name : %s\n", sysname.nodename);
    printf("Machine : %s\n", sysname.machine);

    // User input for GPIO numbers
    char gpioRed1[10], gpioYellow1[10], gpioGreen1[10];
    char gpioRed2[10], gpioYellow2[10], gpioGreen2[10];
    printf("Enter GPIO number for LED1 Red: ");
    scanf("%9s", gpioRed1);
    printf("Enter GPIO number for LED1 Yellow: ");
    scanf("%9s", gpioYellow1);
    printf("Enter GPIO number for LED1 Green: ");
    scanf("%9s", gpioGreen1);
    printf("Enter GPIO number for LED2 Red: ");
    scanf("%9s", gpioRed2);
    printf("Enter GPIO number for LED2 Yellow: ");
    scanf("%9s", gpioYellow2);
    printf("Enter GPIO number for LED2 Green: ");
    scanf("%9s", gpioGreen2);

    // User input for green light duration
    float greenLightDuration;
    printf("Enter green light duration (in seconds): ");
    scanf("%f", &greenLightDuration);


    
    printf("\x1B[1m\n\nFlashing Lights to Indicate the Start\x1B[0m");
    
    
    for (int i = 0; i < 1; i++) {
        
        
        //printf("\n\nFlashing all lights ON\n\n");
        printf("\x1B[1m\n\nFlashing all lights ON\x1B\n[0m");
        
        printf("\nRed Light 1 - ON\n");
        printf("\nRed Light 2 - ON\n");
        printf("\nYellow Light 1 - ON\n");
        printf("\nYellow Light 2 - ON\n");
        printf("\nGreen Light 1 - ON\n");
        printf("\nGreen Light 2 - ON\n");
        
        sleep(1); // Sleep for 1 second
        
        
        
        printf("\x1B[1m\n\nFlashing all lights OFF\x1B\n[0m");
        
        printf("\nRed Light 1 - OFF\n");
        printf("\nRed Light 2 - OFF\n");
        printf("\nYellow Light 1 - OFF\n");
        printf("\nYellow Light 2 - OFF\n");
        printf("\nGreen Light 1 - OFF\n");
        printf("\nGreen Light 2 - OFF\n");
        
        sleep(1); 
    }

    
    
    printf("\x1B[1m\n\nEnsuring all lights OFF\x1B\n[0m");
    printf("\nRed Light 1 - OFF\n");
    printf("\nRed Light 2 - OFF\n");
    printf("\nYellow Light 1 - OFF\n");
    printf("\nYellow Light 2 - OFF\n");
    printf("\nGreen Light 1 - OFF\n");
    printf("\nGreen Light 2 - OFF\n");


    
    int yellow_slp = 5; //5sec
    int slp = 2; //2sec
    while (1) {
    
        
        printf("\x1B[1m\n\nSetting Green Signal 1 (GPIO %s) and Red Signal 2 (GPIO %s) ON\x1B[0m", gpioGreen1, gpioRed2);
        printf("\nRed Signal 1 (GPIO %s) - OFF", gpioRed1);
        printf("\nYellow Signal 1 (GPIO %s) - OFF", gpioYellow1);
        printf("\n\x1B[1mGreen Signal 1 (GPIO %s) - ON\x1B[0m", gpioGreen1);
        printf("\n\x1B[1mRed Signal 2 (GPIO %s) - ON\x1B[0m", gpioRed2);
        printf("\nYellow Signal 2 (GPIO %s) - OFF", gpioYellow2);
        printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        printf("\n\nWaiting for %f seconds...\n\n", greenLightDuration);
        sleep(greenLightDuration);


        
        printf("\x1B[1m\n\nSetting Yellow Signal 1 (GPIO %s) ON\x1B[0m", gpioYellow1);
        printf("\nRed Signal 1 (GPIO %s) - OFF", gpioRed1);
        printf("\n\x1B[1mYellow Signal 1 (GPIO %s) - ON\x1B[0m", gpioYellow1);
        printf("\nGreen Signal 1 (GPIO %s) - OFF", gpioGreen1);
        printf("\n\x1B[1mRed Signal 2 (GPIO %s) - ON\x1B[0m", gpioRed2);
        printf("\nYellow Signal 2 (GPIO %s) - OFF", gpioYellow2);
        printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        printf("\n\nWaiting for %i seconds...\n\n", yellow_slp);
        sleep(yellow_slp);


        

        printf("\x1B[1m\n\nSetting Red Signal 2 (GPIO %s), Yellow Signal 2 (GPIO %s) and Red Signal 1 (GPIO %s) ON\x1B[0m", gpioRed2, gpioYellow2, gpioRed1);      
        printf("\n\x1B[1mRed Signal 1 (GPIO %s) - ON\x1B[0m", gpioRed1);
        printf("\nYellow Signal 1 (GPIO %s) - OFF", gpioYellow1);
        printf("\nGreen Signal 1 (GPIO %s) - OFF", gpioGreen1);
        printf("\n\x1B[1mRed Signal 2 (GPIO %s) - ON\x1B[0m", gpioRed2);
        printf("\n\x1B[1mYellow Signal 2 (GPIO %s) - ON\x1B[0m", gpioYellow2);
        printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        printf("\n\nWaiting for %i seconds...\n\n", slp);
        sleep(slp); //2sec




        printf("\x1B[1m\n\nSetting Red Signal 1 (GPIO %s) and Green Signal 2 (GPIO %s) ON\x1B[0m", gpioRed1, gpioGreen2);
        printf("\n\x1B[1mRed Signal 1 (GPIO %s) - ON\x1B[0m", gpioRed1);
        printf("\nYellow Signal 1 (GPIO %s) - OFF", gpioYellow1);
        printf("\nGreen Signal 1 (GPIO %s) - OFF", gpioGreen1);
        printf("\nRed Signal 2 (GPIO %s) - OFF", gpioRed2);
        printf("\nYellow Signal 2 (GPIO %s) - OFF", gpioYellow2);
        printf("\n\x1B[1mGreen Signal 2 (GPIO %s) - ON\x1B[0m", gpioGreen2); 
        printf("\n\nWaiting for %f seconds...\n\n", greenLightDuration);
        sleep(greenLightDuration);

        
        
        printf("\x1B[1m\n\nSetting Yellow Signal 2 (GPIO %s) ON\x1B[0m", gpioYellow2);
        printf("\n\x1B[1mRed Signal 1 (GPIO %s) - ON\x1B[0m", gpioRed1);
        printf("\nYellow Signal 1 (GPIO %s) - OFF", gpioYellow1);
        printf("\nGreen Signal 1 (GPIO %s) - OFF", gpioGreen1);
        printf("\nRed Signal 2 (GPIO %s) - OFF", gpioRed2);
        printf("\n\x1B[1mYellow Signal 2 (GPIO %s) - ON\x1B[0m", gpioYellow2);
        printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        printf("\n\nWaiting for %i seconds...\n\n", yellow_slp);
        sleep(yellow_slp);


        printf("\x1B[1m\n\nSetting Red Signal 1 (GPIO %s), Yellow Signal 1 (GPIO %s) and Red Signal 2 (GPIO %s) ON\x1B[0m", gpioRed1, gpioYellow1, gpioRed2);   
        printf("\n\x1B[1mRed Signal 1 (GPIO %s) - ON\x1B[0m", gpioRed1);
        printf("\n\x1B[1mYellow Signal 1 (GPIO %s) - ON\x1B[0m", gpioYellow1);
        printf("\nGreen Signal 1 (GPIO %s) - OFF", gpioGreen1);
        printf("\n\x1B[1mRed Signal 2 (GPIO %s) - ON\x1B[0m", gpioRed2);
        printf("\nYellow Signal 2 (GPIO %s) - OFF", gpioYellow2);
        printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        printf("\n\nWaiting for %i seconds...\n\n", slp);
        sleep(slp); //2sec

        // printf("\x1B[1m\n\nSetting Red Signal 2 (GPIO %s) and Green Signal 1 (GPIO %s) ON\x1B[0m", gpioRed2, gpioGreen1);
        // printf("\nRed Signal 1 (GPIO %s) - OFF", gpioRed1);
        // printf("\nYellow Signal 1 (GPIO %s) - OFF", gpioYellow1);
        // printf("\n\x1B[1mGreen Signal 1 (GPIO %s) - ON\x1B[0m", gpioGreen1);
        // printf("\n\x1B[1mRed Signal 2 (GPIO %s) - ON\x1B[0m", gpioRed2);
        // printf("\nYellow Signal 2 (GPIO %s) - OFF", gpioYellow2);
        // printf("\nGreen Signal 2 (GPIO %s) - OFF", gpioGreen2);
        // sleep(greenLightDuration);
        
    }

    return 0;
}
