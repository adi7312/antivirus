#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

#define INFO 0
#define WARN 1
#define CRITICAL 2
#define SUCCESS 3
#define ERROR 4
#define AV_LOG "/var/lib/av/logs/av.log"

void log(const char* filename, int type, const char* format, ...);