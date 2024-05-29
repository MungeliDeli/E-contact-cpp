#include <jni.h>
#include <string>
#include <android/log.h>
#include "sqlite3.h"

// Logging tag for log messages
#define LOG_TAG "SQLiteDatabase"

// Macros for logging information and errors
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


