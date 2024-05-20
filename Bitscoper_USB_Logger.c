/* By Abdullah As-Sadeed */

/*
gcc ./Bitscoper_USB_Logger.c -o ./Bitscoper_USB_Logger -ludev
*/

#include <libudev.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TERMINAL_TITLE_START "\033]0;"
#define TERMINAL_TITLE_END "\007"

#define TERMINAL_ANSI_COLOR_RED "\x1b[31m"
#define TERMINAL_ANSI_COLOR_GREEN "\x1b[32m"
#define TERMINAL_ANSI_COLOR_RESET "\x1b[0m"

#define LOG_FILE "USB.log"

bool keep_running = true;

void append_data(char **data_string, size_t *data_length, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    size_t needed = vsnprintf(NULL, 0, format, arguments) + 1;
    va_end(arguments);

    *data_string = (char *)realloc(*data_string, *data_length + needed);
    if (*data_string)
    {
        va_start(arguments, format);
        vsprintf(*data_string + *data_length, format, arguments);
        va_end(arguments);
        *data_length += needed - 1;
    }
    else
    {
        fprintf(stderr, TERMINAL_ANSI_COLOR_RED "Failed to allocate memory.\n" TERMINAL_ANSI_COLOR_RESET);
        exit(EXIT_FAILURE);
    }
}

void log_USB_event(const char *event, struct udev_device *device, const char *time_stamp)
{
    const char *device_node = udev_device_get_devnode(device);

    char *data_string = NULL;
    size_t data_length = 0;

    append_data(&data_string, &data_length, "[%s] Event: %s\nDevice: %s\n", time_stamp, event, device_node);

    struct udev_list_entry *attributes = udev_device_get_sysattr_list_entry(device);
    struct udev_list_entry *entry;
    udev_list_entry_foreach(entry, attributes)
    {
        const char *attribute = udev_list_entry_get_name(entry);
        const char *value = udev_device_get_sysattr_value(device, attribute) ?: "Unknown";
        append_data(&data_string, &data_length, "%s: %s\n", attribute, value);
    }

    append_data(&data_string, &data_length, "\n");

    printf("%s", data_string);

    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file)
    {
        fprintf(log_file, "%s", data_string);
        fclose(log_file);
    }
    else
    {
        fprintf(stderr, TERMINAL_ANSI_COLOR_RED "Failed to open log file.\n" TERMINAL_ANSI_COLOR_RESET);
    }

    free(data_string);
}

void Handle_Signal(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        const char *message;

        if (signal == SIGINT)
        {
            message = TERMINAL_ANSI_COLOR_RED "\n\nYou interrupted me by SIGINT signal.\n" TERMINAL_ANSI_COLOR_RESET;
        }
        else if (signal == SIGTERM)
        {
            message = TERMINAL_ANSI_COLOR_RED "\n\nYou interrupted me by SIGTERM signal.\n" TERMINAL_ANSI_COLOR_RESET;
        }

        keep_running = false;

        write(STDOUT_FILENO, message, strlen(message));
        quick_exit(signal);
    }
}

int main(void)
{
    signal(SIGINT, Handle_Signal);
    signal(SIGTERM, Handle_Signal);

    printf(TERMINAL_TITLE_START "Bitscoper USB Logger" TERMINAL_TITLE_END);

    struct udev *udev = udev_new();
    if (!udev)
    {
        fprintf(stderr, TERMINAL_ANSI_COLOR_RED "Failed to initialize udev.\n" TERMINAL_ANSI_COLOR_RESET);
        return 1;
    }

    struct udev_monitor *monitor = udev_monitor_new_from_netlink(udev, "udev");
    if (!monitor)
    {
        fprintf(stderr, TERMINAL_ANSI_COLOR_RED "Failed to create udev monitor.\n" TERMINAL_ANSI_COLOR_RESET);
        udev_unref(udev);
        return 1;
    }

    udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", NULL);
    udev_monitor_enable_receiving(monitor);

    printf(TERMINAL_ANSI_COLOR_GREEN "Monitoring USB devices has been started." TERMINAL_ANSI_COLOR_RESET "\n\n");

    while (keep_running)
    {
        struct udev_device *device = udev_monitor_receive_device(monitor);
        if (device)
        {
            const char *event = udev_device_get_action(device);

            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            char time_stamp[20];
            strftime(time_stamp, sizeof(time_stamp), "%Y-%m-%d %H:%M:%S", tm_info);

            if (event)
            {
                log_USB_event(event, device, time_stamp);
            }

            udev_device_unref(device);
        }
    }

    udev_monitor_unref(monitor);
    udev_unref(udev);

    return 0;
}
