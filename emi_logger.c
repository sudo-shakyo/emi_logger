#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOG_FILE "emi_diagnostic.log"
#define TRIGGER_PHRASE "nvidia-drmdrmfb"

void log_event(const char* message) {
    FILE *logfile = fopen(LOG_FILE, "a");
    if (logfile == NULL) {
        perror("File error");
        return;
    }

    time_t rawtime;
    struct tm * timeinfo;
    char time_buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(time_buffer, sizeof(time_buffer), "[%Y-%m-%d %H:%M:%S]", timeinfo);

    fprintf(logfile, "%s WARNING: %s\n", time_buffer, message);
    fflush(logfile); 
    fclose(logfile);
}

int main() {
    printf("Starting EMI Logger...\n");
    printf("Monitoring for display drops...\n");

    FILE *fp = popen("dmesg -w", "r");
    if (fp == NULL) {
        perror("Failed to read system logs");
        return 1;
    }

    char line[1024];
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, TRIGGER_PHRASE) != NULL) {
            log_event("Display disconnected. Possible EMI drop detected.");
            sleep(3); 
        }
    }

    pclose(fp);
    return 0;
}