#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// Global variables for GPIO pin numbers
char LED1_PIN[5];
char LED2_PIN[5];
char BUTTON1_PIN[5];
char BUTTON2_PIN[5];
char BUTTON3_PIN[5];
char BUTTON4_PIN[5];
char BUZZER_PIN[5];

// Global variable to keep track of the last button press time
time_t last_button_press_time;

// Function prototypes
void *buttonListener(void *arg);
void *crossingControl(void *arg);
void initializeGPIO();
void cleanupGPIO();
void blinkLEDs(bool alternating, int duration, bool stopAfterDuration);
void soundBuzzer(int duration);
void setLEDs(bool led1, bool led2);
void exportGPIO(const char *pin);
void export_pwm();
void set_pwm_period(const char *period);
void set_pwm_duty_cycle(const char *duty_cycle);
void set_pwm_enable(const char *enable);
void configure_pin();
void getUserInput();
void soundBuzzerAndBlinkLEDs(int duration);

// Global variables
volatile int sequence = 0;
pthread_mutex_t lock;

// Global variables for GPIO file descriptors
int fd_led1, fd_led2, fd_button1, fd_button2, fd_button3, fd_button4, fd_buzzer;

int main() {
    
    getUserInput();
    
    // Initialize GPIO
    initializeGPIO();
    
    time(&last_button_press_time);

    // Initialize mutex
    pthread_mutex_init(&lock, NULL);

    // Create threads for button listeners and crossing control
    pthread_t buttonThread1, buttonThread2, buttonThread3, buttonThread4, crossingThread;
    pthread_create(&buttonThread1, NULL, buttonListener, (void *)1);
    pthread_create(&buttonThread2, NULL, buttonListener, (void *)2);
    pthread_create(&buttonThread3, NULL, buttonListener, (void *)3);
    pthread_create(&buttonThread4, NULL, buttonListener, (void *)4);
    pthread_create(&crossingThread, NULL, crossingControl, NULL);

    // Wait for threads to finish
    pthread_join(buttonThread1, NULL);
    pthread_join(buttonThread2, NULL);
    pthread_join(buttonThread3, NULL);
    pthread_join(buttonThread4, NULL);
    pthread_join(crossingThread, NULL);

    // Cleanup
    pthread_mutex_destroy(&lock);
    cleanupGPIO();

    return 0;
}

void getUserInput() {
    printf("Enter GPIO pin number for LED1: ");
    scanf("%s", LED1_PIN);
    printf("Enter GPIO pin number for LED2: ");
    scanf("%s", LED2_PIN);
    printf("Enter GPIO pin number for BUZZER: ");
    scanf("%s", BUZZER_PIN);
    printf("Enter GPIO pin number for BUTTON1: ");
    scanf("%s", BUTTON1_PIN);
    printf("Enter GPIO pin number for BUTTON2: ");
    scanf("%s", BUTTON2_PIN);
    printf("Enter GPIO pin number for BUTTON3: ");
    scanf("%s", BUTTON3_PIN);
    printf("Enter GPIO pin number for BUTTON4: ");
    scanf("%s", BUTTON4_PIN);
}

// Function to export the PWM
void export_pwm() {
    FILE *fp = fopen("/sys/class/pwm/pwmchip4/export", "w");
    if (fp == NULL) {
        perror("Error exporting PWM");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "0");
    fclose(fp);
}

// Function to set the PWM period
void set_pwm_period(const char *period) {
    FILE *fp = fopen("/sys/class/pwm/pwmchip4/pwm-4:0/period", "w");
    if (fp == NULL) {
        perror("Error setting PWM period");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", period);
    fclose(fp);
}

// Function to set the PWM duty cycle
void set_pwm_duty_cycle(const char *duty_cycle) {
    FILE *fp = fopen("/sys/class/pwm/pwmchip4/pwm-4:0/duty_cycle", "w");
    if (fp == NULL) {
        perror("Error setting PWM duty cycle");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", duty_cycle);
    fclose(fp);
}

// Function to enable or disable the PWM
void set_pwm_enable(const char *enable) {
    FILE *fp = fopen("/sys/class/pwm/pwmchip4/pwm-4:0/enable", "w");
    if (fp == NULL) {
        perror("Error enabling/disabling PWM");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "%s", enable);
    fclose(fp);
}

// Function to configure the pin for PWM
void configure_pin() {
    system("config-pin P9.14 pwm");
}

void initializeGPIO() {
    printf("Initializing GPIO pins\n");
    
    // Export GPIO pins
    exportGPIO(LED1_PIN);
    exportGPIO(LED2_PIN);
    exportGPIO(BUZZER_PIN);
    exportGPIO(BUTTON1_PIN);
    exportGPIO(BUTTON2_PIN);
    exportGPIO(BUTTON3_PIN);
    exportGPIO(BUTTON4_PIN);
    
    // Set direction out for LEDs and buzzer
    int f;
    char path[50];

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", LED1_PIN);
    f = open(path, O_RDWR);
    write(f, "out", 3);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", LED1_PIN);
    fd_led1 = open(path, O_WRONLY);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", LED2_PIN);
    f = open(path, O_RDWR);
    write(f, "out", 3);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", LED2_PIN);
    fd_led2 = open(path, O_WRONLY);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", BUZZER_PIN);
    f = open(path, O_RDWR);
    write(f, "out", 3);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", BUZZER_PIN);
    fd_buzzer = open(path, O_WRONLY);

    // Set direction in for buttons
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", BUTTON1_PIN);
    f = open(path, O_RDWR);
    write(f, "in", 2);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", BUTTON1_PIN);
        fd_button1 = open(path, O_RDONLY);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", BUTTON2_PIN);
    f = open(path, O_RDWR);
    write(f, "in", 2);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", BUTTON2_PIN);
    fd_button2 = open(path, O_RDONLY);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", BUTTON3_PIN);
    f = open(path, O_RDWR);
    write(f, "in", 2);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", BUTTON3_PIN);
    fd_button3 = open(path, O_RDONLY);

    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/direction", BUTTON4_PIN);
    f = open(path, O_RDWR);
    write(f, "in", 2);
    close(f);
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s/value", BUTTON4_PIN);
    fd_button4 = open(path, O_RDONLY);
}

void *buttonListener(void *arg) {
    int button = (int)arg;
    int fd_button = (button == 1) ? fd_button1 :
                    (button == 2) ? fd_button2 :
                    (button == 3) ? fd_button3 :
                                     fd_button4;
    char value;

    printf("Button listener started for button %d\n", button);

    while (1) {
        // Read the state of the button
        lseek(fd_button, 0, SEEK_SET);
        read(fd_button, &value, 1);

        pthread_mutex_lock(&lock);
        if (value == '0') { // Button pressed
            printf("Button %d pressed\n", button);
            time(&last_button_press_time);
            
            // LEFT SIDE
            if (button == 1 && sequence == 0) {
                sequence = 1; // Start Button 1-2 sequence
            } else if (button == 2 && sequence == 1) {
                sequence = 2; // Button 1-2 sequence completed, start glowing
                
                // Configure the pin for PWM
                configure_pin();
            
                // Export the PWM
                export_pwm();
            
                // Set the PWM period to 20ms (50Hz)
                set_pwm_period("20000000");
            
                // Enable the PWM
                set_pwm_enable("1");
            
                // Sweep the servo from 0 to 180 degrees
                // for (int i = 0; i < 10; i++) {
                    // Set the duty cycle to 1ms (0 degrees)
                    set_pwm_duty_cycle("1000000");
                    sleep(1);
            
                    // Set the duty cycle to 2ms (90 degrees)
                    set_pwm_duty_cycle("2000000");
                    sleep(1);
                // }
            
                // Disable the PWM
                set_pwm_enable("0");
                
                
            }
            else if (button == 3 && sequence == 2) {
                sequence = 3; 
            }
            else if (button == 4 && sequence == 3) {
                sequence = 0; // Button 4 pressed after Button 1-2 sequence
                // Configure the pin for PWM
                configure_pin();
                
                // Export the PWM
                export_pwm();
                
                // Set the PWM period to 20ms (50Hz)
                set_pwm_period("20000000");
                
                // Enable the PWM
                set_pwm_enable("1");
                
                // Set the duty cycle to 1.5ms (90 degrees)
                set_pwm_duty_cycle("2000000");
                sleep(1);
                
                // Set the duty cycle to 1ms (0 degrees)
                set_pwm_duty_cycle("1000000");
                sleep(1);
                
                // Disable the PWM
                set_pwm_enable("0");

            }
            
            
            // RIGHT SIDE
             else if (button == 4 && sequence == 0) {
                sequence = 4; // Start Button 4-3 sequence
            } else if (button == 3 && sequence == 4) {
                sequence = 5; // Button 4-3 sequence completed, start glowing
                
                // Configure the pin for PWM
                configure_pin();
                
                // Export the PWM
                export_pwm();
                
                // Set the PWM period to 20ms (50Hz)
                set_pwm_period("20000000");
                
                // Enable the PWM
                set_pwm_enable("1");
                
                // Set the duty cycle to 1ms (0 degrees)
                set_pwm_duty_cycle("1000000");
                sleep(1);
                
                // Set the duty cycle to 1.5ms (90 degrees)
                set_pwm_duty_cycle("2000000");
                sleep(1);
                
                // Disable the PWM
                set_pwm_enable("0");
                
            }
             else if (button == 2 && sequence == 5) {
                sequence = 6; //
            } else if (button == 1 && sequence == 6) {
                sequence = 0; 
                // Configure the pin for PWM
                configure_pin();
                
                // Export the PWM
                export_pwm();
                
                // Set the PWM period to 20ms (50Hz)
                set_pwm_period("20000000");
                
                // Enable the PWM
                set_pwm_enable("1");
                
                // Set the duty cycle to 1.5ms (90 degrees)
                set_pwm_duty_cycle("2000000");
                sleep(1);
                
                // Set the duty cycle to 1ms (0 degrees)
                set_pwm_duty_cycle("1000000");
                sleep(1);
                
                // Disable the PWM
                set_pwm_enable("0");

            }
            
            //COLLISION LEFT SIDE
            else if (button == 4 && sequence == 2) {
                sequence = 9; 
            }
            else if (button == 3 && sequence == 9) {
                sequence = 7; // Collision
                soundBuzzerAndBlinkLEDs(18);
            }
            
            //COLLISION RIGHT SIDE
            else if (button == 1 && sequence == 5) {
                sequence = 8; // Start Button 1-2 sequence
            } else if (button == 2 && sequence == 8) {
                sequence = 7; //Collision
                soundBuzzerAndBlinkLEDs(18);
            }
           
            
        }
        pthread_mutex_unlock(&lock);
        usleep(100000); // Debounce delay
    }
}



void *crossingControl(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        // Check if 20 seconds have passed since the last button press
        time_t current_time;
        time(&current_time);
        if (difftime(current_time, last_button_press_time) >= 20) {
            // Reset the system
            printf("Resetting system due to inactivity\n");
            sequence = 0;
            setLEDs(false, false);
            // Configure the pin for PWM
            configure_pin();
            
            // Export the PWM
            export_pwm();
            
            // Set the PWM period to 20ms (50Hz)
            set_pwm_period("20000000");
            
            // Enable the PWM
            set_pwm_enable("1");
            
            // Set the duty cycle to 1ms (0 degrees)
            set_pwm_duty_cycle("1000000");
            sleep(1);
            
            // Disable the PWM
            set_pwm_enable("0");
        }
        
        
        
        
        if (sequence == 2 || sequence == 3 || sequence == 5 || sequence == 6 || sequence == 8 || sequence == 9) {
            // Glow LEDs alternately
            blinkLEDs(true, 1, false); // Keep glowing alternately
        } else if (sequence == 7) {
            // Collision detected
            printf("Collision detected\n");
            sequence = 0; // Reset sequence after collision
        } else if (sequence == 0) {
            // Turn off LEDs
            setLEDs(false, false);
        }
        pthread_mutex_unlock(&lock);
        usleep(500000); // Add a small delay to prevent high CPU usage
    }
}




void setLEDs(bool led1, bool led2) {
    write(fd_led1, led1 ? "1" : "0", 1);
    write(fd_led2, led2 ? "1" : "0", 1);
}

void exportGPIO(const char *pin) {
    char path[35];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s", pin);

    // Check if GPIO is already exported
    if (access(path, F_OK) == 0) {
        printf("GPIO %s already exported\n", pin);
        return;
    }

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd < 0) {
        perror("Failed to open export for writing");
        return;
    }

    if (write(fd, pin, strlen(pin)) < 0) {
        perror("Failed to export GPIO");
    }

        close(fd);
}

void cleanupGPIO() {
    printf("Cleaning up GPIO pins\n");

    // Close the file descriptors
    close(fd_led1);
    close(fd_led2);
    close(fd_buzzer);
    close(fd_button1);
    close(fd_button2);
    close(fd_button3);
    close(fd_button4);
}

void soundBuzzerAndBlinkLEDs(int duration) {
    char path[50];
    sprintf(path, "/sys/class/gpio/gpio%s/value", BUZZER_PIN);
    FILE *f_buzzer = fopen(path, "w");
    if (f_buzzer == NULL) {
        perror("Error opening buzzer value file");
        return;
    }

    for (int i = 0; i < duration; i++) {
        fprintf(f_buzzer, "1");
        fflush(f_buzzer);
        write(fd_led1, "1", 1);
        write(fd_led2, "1", 1);
        usleep(500000); // 0.5 seconds
        fprintf(f_buzzer, "0");
        fflush(f_buzzer);
        write(fd_led1, "0", 1);
        write(fd_led2, "0", 1);
        usleep(500000); // 0.5 seconds
    }

    fclose(f_buzzer);
}


void blinkLEDs(bool alternating, int duration, bool stopAfterDuration) {
    for (int i = 0; i < duration; i++) {
        if (alternating) {
            write(fd_led1, "1", 1);
            usleep(500000); // 0.5 seconds
            write(fd_led1, "0", 1);
            write(fd_led2, "1", 1);
            usleep(500000); // 0.5 seconds
            write(fd_led2, "0", 1);
        } else {
            write(fd_led1, "1", 1);
            write(fd_led2, "1", 1);
            usleep(100000); // 0.1 seconds
            write(fd_led1, "0", 1);
            write(fd_led2, "0", 1);
            usleep(100000); // 0.1 seconds
        }
    }

    // If stopAfterDuration is true, ensure LEDs are off after blinking
    if (stopAfterDuration) {
        write(fd_led1, "0", 1);
        write(fd_led2, "0", 1);
    }
}
