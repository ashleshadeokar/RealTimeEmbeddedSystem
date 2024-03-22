#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/utsname.h>

volatile bool isApproaching = false;
volatile bool isCollision = false;
volatile bool exitSignal = false;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

void* inputHandler(void* args);
void* ledHandler(void* args);
void* servoAndBuzzerHandler(void* args);

int main() {

    struct utsname sysname;
    uname(&sysname);
    printf("System Name : %s\n", sysname.sysname);
    printf("Node Name : %s\n", sysname.nodename);
    printf("Machine : %s\n", sysname.machine);
    
    
    pthread_t inputThread, ledThread, servoBuzzerThread;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&condition_var, NULL);

    pthread_create(&inputThread, NULL, inputHandler, NULL);
    pthread_create(&ledThread, NULL, ledHandler, NULL);
    pthread_create(&servoBuzzerThread, NULL, servoAndBuzzerHandler, NULL);

    pthread_join(inputThread, NULL);
    pthread_join(ledThread, NULL);
    pthread_join(servoBuzzerThread, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&condition_var);

    return 0;
}

void* inputHandler(void* args) {
    int input;
    while (1) {
        printf("\nEnter '12' for West->East approach, '34' for West->East exit,\n"
               "'43' for East->West approach, '21' for East->West exit,\n"
               "'99' for manual override clearance: ");
        scanf("%d", &input);

        pthread_mutex_lock(&lock);
        switch (input) {
            case 12:
            case 43:
                if (isApproaching) { // Trigger collision if already approaching.
                    isCollision = true;
                    printf("\n******Collision Scenario*******\n");
                } else {
                    isApproaching = true;
                }
                exitSignal = false;
                break;
            case 34:
            case 21:
                isApproaching = false;
                isCollision = false;
                exitSignal = true; // Added to ensure LEDs turn off explicitly.
                //printf("LED1 is OFF, LED2 is OFF\n");
                break;
            case 99:
                isApproaching = false;
                isCollision = false;
                exitSignal = true;
                break;
            default:
                printf("Invalid input.\n");
                break;
        }
        pthread_cond_broadcast(&condition_var);
        pthread_mutex_unlock(&lock);
    }
}

void* ledHandler(void* args) {
    while (true) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&condition_var, &lock);

        if (isCollision) {
            printf("LED1 is ON, LED2 is ON\n");
            pthread_mutex_unlock(&lock);
            sleep(1); // Adjust for visible blinking
            pthread_mutex_lock(&lock);
            printf("LED1 is OFF, LED2 is OFF\n"); // Explicit OFF state for collision blinking.
        } else if (isApproaching) {
            // Alternating blinking for normal approach
            printf("LED1 is ON, LED2 is OFF\n");
            sleep(1);
            printf("LED1 is OFF, LED2 is ON\n");
        }
        if (exitSignal) {
            // Print LEDs OFF state once more upon exit, for clarity.
            printf("LED1 is OFF, LED2 is OFF\n");
            sleep(1);
            printf("LED1 is OFF, LED2 is OFF\n");
        }
        pthread_mutex_unlock(&lock);
        sleep(1); // Maintain consistent update timing.
    }
}

void* servoAndBuzzerHandler(void* args) {
    while (true) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&condition_var, &lock);

        printf("Servo is %s\n", isApproaching ? "DOWN" : "UP");
        printf("Buzzer is %s\n", isCollision ? "ON" : "OFF");

        pthread_mutex_unlock(&lock);
        sleep(1); // Consistent update interval.
    }
}

