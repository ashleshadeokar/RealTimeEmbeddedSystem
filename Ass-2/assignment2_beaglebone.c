#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <time.h> 
#include <stdint.h>


#define BUFFER_SIZE 256
volatile sig_atomic_t exit_flag = 0;


struct led {
    char red[BUFFER_SIZE];
    char yellow[BUFFER_SIZE];
    char green[BUFFER_SIZE];
};
struct led led1, led2;


void gpio_write(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, value, strlen(value));
        close(fd);
    }
}

void gpio_set_direction(const char *path, const char *direction) {
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, direction, strlen(direction));
        close(fd);
    }
}

void construct_path(char *path, const char *base, const char *gpio, const char *file) {
    snprintf(path, BUFFER_SIZE, "%s%s/%s", base, gpio, file);
}

void initialize_leds_as_output(void){
    gpio_set_direction(led1.red, "out");
    gpio_set_direction(led1.yellow, "out");
    gpio_set_direction(led1.green, "out");
    gpio_set_direction(led2.red, "out");
    gpio_set_direction(led2.yellow, "out");
    gpio_set_direction(led2.green, "out");
}

//to handle interuptions
void signal_handler(int sig) {
    printf("Caught signal %d, exiting gracefully...\n", sig);
    exit_flag = 1;
}

//GPIO pins availability check
int is_gpio_available(const char* gpio_number) {
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s", gpio_number);

    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 1;
    }
    return 0; // GPIO not available
}


void precise_sleep(double seconds) {
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e+9);
    nanosleep(&ts, NULL);
}

int main() {
    char gpio[10];
    float signal_time_minutes, signal_time;
    const float yellow_time = 5.0; // Yellow light duration
    const float transition_time = 2.0; // Transition time with red and yellow
    const float flash_duration = 5.0; // Initial flashing duration

    signal(SIGINT, signal_handler);

    printf("Enter signal time (minutes, can be fractional): ");
    scanf("%f", &signal_time_minutes);
    signal_time = signal_time_minutes * 60.0; // Convert minutes to seconds

    const char* gpio_prompts[] = {
        "Enter GPIO number for LED1 Red: ",
        "Enter GPIO number for LED1 Yellow: ",
        "Enter GPIO number for LED1 Green: ",
        "Enter GPIO number for LED2 Red: ",
        "Enter GPIO number for LED2 Yellow: ",
        "Enter GPIO number for LED2 Green: "
    };
    char* gpio_paths[] = {led1.red, led1.yellow, led1.green, led2.red, led2.yellow, led2.green};

    
    for (int i = 0; i < 6; i++) {
        do {
            printf("%s", gpio_prompts[i]);
            scanf("%s", gpio);
            if (!is_gpio_available(gpio)) {
                printf("GPIO %s is not available. Please enter a valid GPIO number.\n", gpio);
            } else {
                construct_path(gpio_paths[i], "/sys/class/gpio/gpio", gpio, "value");
                break;
            }
        } while (1);
    }

    initialize_leds_as_output();
    
    // For Flashing Lights indicating the start of execution
    for (int i = 0; i < flash_duration; ++i) {
        gpio_write(led1.red, "1");
        gpio_write(led2.red, "1");
        gpio_write(led1.yellow, "1");
        gpio_write(led2.yellow, "1");
        gpio_write(led1.green, "1");
        gpio_write(led2.green, "1");
        
        precise_sleep(1); 
        gpio_write(led1.red, "0");
        gpio_write(led2.red, "0");
        gpio_write(led1.yellow, "0");
        gpio_write(led2.yellow, "0");
        gpio_write(led1.green, "0");
        gpio_write(led2.green, "0");
        
        precise_sleep(1); 
    }

    gpio_write(led1.red, "0");
    gpio_write(led2.red, "0");
    gpio_write(led1.yellow, "0");
    gpio_write(led2.yellow, "0");
    gpio_write(led1.green, "0");
    gpio_write(led2.green, "0");
    
    while (!exit_flag) {
        
    // Setting G1 and R2 = ON
    gpio_write(led1.red, "0");
    gpio_write(led1.yellow, "0");
    gpio_write(led1.green, "1");
    gpio_write(led2.red, "1");
    gpio_write(led2.yellow, "0");
    gpio_write(led2.green, "0");
    
    precise_sleep(signal_time);  //Green light sleep time
    
    
    // Transition from G1 to Y1
    gpio_write(led1.green, "0");
    gpio_write(led1.yellow, "1");
    
    precise_sleep(yellow_time);  //Yellow sleep time
    
    
    // R1, R2, Y2 = ON (Transition Phase from Green to Yellow as mentioned)
    gpio_write(led1.yellow, "0");
    gpio_write(led1.red, "1");
    gpio_write(led1.green, "0");
    gpio_write(led2.red, "1");
    gpio_write(led2.yellow, "1");
    gpio_write(led2.green, "0");
    
    precise_sleep(2); // 2-second wait with Y2 and R2 ON indicating transistion phase from red ligh to green light
 
 
 
 
    
    // Transition to Green on G2
    gpio_write(led1.red, "1");
    gpio_write(led1.yellow, "0");
    gpio_write(led1.green, "0");
    gpio_write(led2.red, "0");
    gpio_write(led2.yellow, "0");
    gpio_write(led2.green, "1");
    
    precise_sleep(signal_time);
    
    
    // Transition from Green2 light to Yellow2 light
    gpio_write(led2.green, "0");
    gpio_write(led2.yellow, "1");
    
    precise_sleep(yellow_time);
    
    
    // Transition Phase having R2, R1 and Y1 = ON
    gpio_write(led2.yellow, "0");
    gpio_write(led2.red, "1");
    gpio_write(led1.red, "1");
    gpio_write(led1.yellow, "1");
    
    precise_sleep(2); 
    
    
    // Transition from Yellow1 light to Green1
    gpio_write(led1.yellow, "0");
    gpio_write(led1.green, "1");
    
    if (exit_flag) break; // Check for exit condition
}

    printf("Program exited gracefully.\n");

    return 0;
}