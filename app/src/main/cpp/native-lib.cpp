#include <jni.h>
#include <string>
#include <android/log.h>
#include "sqlite3.h"

// Logging tag for log messages
#define LOG_TAG "SQLiteDatabase"

// Macros for logging information and errors
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


// Database object
sqlite3 *db;

// create and initializing the database
extern "C" JNIEXPORT jint JNICALL
Java_com_example_e_1contact_DbHelper_initDB(JNIEnv *env, jobject /* this */, jstring dbPath) {
    const char *dbPathCStr = env->GetStringUTFChars(dbPath, nullptr);
    int rc = sqlite3_open(dbPathCStr, &db);
    if (rc) {
        LOGE("Can't open database: %s", sqlite3_errmsg(db));
        return rc;
    } else {
        LOGI("Opened database successfully");
    }


    return SQLITE_OK;
}
