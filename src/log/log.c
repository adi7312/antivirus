#include "log.h"

void log(const char* filename, int type, const char* format,...){
    FILE *ifp = fopen(filename,"a");
    if (ifp == NULL){
        fprintf(stderr, "Failed to initialize logging file. Aborting...");
        exit(1);
    }
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", timeinfo);
    fprintf(ifp, "%s ", buffer);
    
    va_list args;
    va_start(args, format);

    switch (type)
    {
    case 0:
        if (fprintf(ifp, "[INFO] ") < 0) { 
            fprintf(stderr, "Failed to write log to file.");
        }
        break;
    case 1:
        if (fprintf(ifp, "[WARN] ") < 0){
            fprintf(stderr, "Failed to write log to file.");
        }
        break;
    case 2:
        if (fprintf(ifp, "[CRITICAL] ") < 0){
            fprintf(stderr, "Failed to write log to file.");
        }
        break;
    case 3:
        if (fprintf(ifp, "[SUCCESS] ") < 0){
            fprintf(stderr, "Failed to write log to file.");
        }
        break;
    case 4:
        if (fprintf(ifp, "[ERROR] ") < 0){
            fprintf(stderr, "Failed to write log to file.");
        }
        break;
    default:
        fprintf(stderr, "Unknown type. Aborting...\n");
        fclose(ifp); 
        va_end(args);
        exit(2);
        break;
    }
    vfprintf(ifp, format, args);
    va_end(args);
    fclose(ifp);
}