#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

#define MAX_FLOAT_SECONDS 100.0

#define RED_LED "/sys/class/gpio/gpio66/value"
#define GREEN_LED "/sys/class/gpio/gpio67/value"
#define START_STOP_BUTTON "/sys/class/gpio/gpio68/value"
#define RESET_BUTTON "/sys/class/gpio/gpio69/value"

#define RED_LED_DIRECTION "/sys/class/gpio/gpio66/direction"
#define GREEN_LED_DIRECTION "/sys/class/gpio/gpio67/direction"
#define START_STOP_BUTTON_DIRECTION "/sys/class/gpio/gpio68/direction"
#define RESET_BUTTON_DIRECTION "/sys/class/gpio/gpio69/direction"

pthread_t button_thread, print_thread, reset_thread;

double elapsed = 0.0;
int start_stop_flag = 0;
static pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t elapsed_mutex = PTHREAD_MUTEX_INITIALIZER;
struct timespec start_time, now;

void write_to_file(const char *path, const char *value) {
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, value, 1);
        close(fd);
    }
}

void set_gpio_direction(const char *path, const char *direction) {
    int fd = open(path, O_WRONLY);
    if (fd >= 0) {
        write(fd, direction, strlen(direction));
        close(fd);
    }
}

void *button_status(void *arg) {
    int button_fd = open(START_STOP_BUTTON, O_RDONLY);
    if (button_fd < 0) {
        perror("Failed to open start/stop button");
        return NULL;
    }

    char button_val;
    int button_pressed = 0;
    while (1) {
        lseek(button_fd, 0, SEEK_SET);
        read(button_fd, &button_val, 1);
        if (button_val == '1') {
            if (!button_pressed) {
                button_pressed = 1;
                pthread_mutex_lock(&flag_mutex);
                start_stop_flag = !start_stop_flag;
                pthread_mutex_unlock(&flag_mutex);

                if (start_stop_flag) {
                    write_to_file(GREEN_LED, "1");
                    write_to_file(RED_LED, "0");
                    clock_gettime(CLOCK_MONOTONIC, &start_time);
                } else {
                    write_to_file(GREEN_LED, "0");
                    write_to_file(RED_LED, "1");
                    pthread_mutex_lock(&elapsed_mutex);
                    clock_gettime(CLOCK_MONOTONIC, &now);
                    elapsed += (now.tv_sec - start_time.tv_sec) + (now.tv_nsec - start_time.tv_nsec) / 1e9;
                    pthread_mutex_unlock(&elapsed_mutex);
                }
            }
        } else {
            button_pressed = 0;
        }
        usleep(10000);
    }
    close(button_fd);
    return NULL;
}

void *reset_button(void *arg) {
    int button_fd = open(RESET_BUTTON, O_RDONLY);
    if (button_fd < 0) {
        perror("Failed to open reset button");
        return NULL;
    }

    char button_val;
    int button_pressed = 0;
    while (1) {
        lseek(button_fd, 0, SEEK_SET);
        read(button_fd, &button_val, 1);
        if (button_val == '1') {
            if (!button_pressed) {
                button_pressed = 1;
                pthread_mutex_lock(&elapsed_mutex);
                elapsed = 0.0;
                pthread_mutex_unlock(&elapsed_mutex);
                pthread_mutex_lock(&flag_mutex);
                start_stop_flag = 0;
                pthread_mutex_unlock(&flag_mutex);
                write_to_file(RED_LED, "1");
                write_to_file(GREEN_LED, "0");
            }
        } else {
            button_pressed = 0;
        }
        usleep(10000);
    }
    close(button_fd);
    return NULL;
}

void *time_printer(void *arg) {
    while (1) {
        pthread_mutex_lock(&flag_mutex);
        int flag = start_stop_flag;
        pthread_mutex_unlock(&flag_mutex);

        if (flag) {
            pthread_mutex_lock(&elapsed_mutex);
            clock_gettime(CLOCK_MONOTONIC, &now);
            double curr_elapsed = elapsed + (now.tv_sec - start_time.tv_sec) + (now.tv_nsec - start_time.tv_nsec) / 1e9;
            if (curr_elapsed >= MAX_FLOAT_SECONDS) {
                elapsed = 0;
                clock_gettime(CLOCK_MONOTONIC, &now);
                start_time = now;
            }
            pthread_mutex_unlock(&elapsed_mutex);
            printf("\rElapsed Time: %.1f", curr_elapsed);
            fflush(stdout);
        } else {
            printf("\rStopped Time: %.1f", elapsed);
            fflush(stdout);
        }
        usleep(100000);
    }
    return NULL;
}

int main(void) {
    // Set GPIO directions
    set_gpio_direction(RED_LED_DIRECTION, "out");
    set_gpio_direction(GREEN_LED_DIRECTION, "out");
    set_gpio_direction(START_STOP_BUTTON_DIRECTION, "in");
    set_gpio_direction(RESET_BUTTON_DIRECTION, "in");

    pthread_create(&reset_thread, NULL, reset_button, NULL);
    pthread_create(&button_thread, NULL, button_status, NULL);
    pthread_create(&print_thread, NULL, time_printer, NULL);

    pthread_join(reset_thread, NULL);
    pthread_join(button_thread, NULL);
    pthread_join(print_thread, NULL);

    return 0;
}

