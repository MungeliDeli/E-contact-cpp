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





// Function to initialize the USERDATA table
extern "C" JNIEXPORT jint JNICALL
Java_com_example_e_1contact_DbHelper_createTables(JNIEnv *env, jobject /* this */) {


    // Create USERDATA table
    const char *sqlCreateTableUser = "CREATE TABLE IF NOT EXISTS USERDATA("
                                     "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                     "FIRST_NAME TEXT NOT NULL, "
                                     "LAST_NAME TEXT NOT NULL, "
                                     "HOME_ADDRESS TEXT NOT NULL, "
                                     "HOME_PHONE TEXT NOT NULL CHECK(LENGTH(HOME_PHONE) = 10), "
                                     "EMERGENCY_CONTACT_NAME TEXT NOT NULL, "
                                     "EMERGENCY_CONTACT_NUMBER TEXT NOT NULL CHECK(LENGTH(EMERGENCY_CONTACT_NUMBER) = 10));";

    char *errMsg = 0;
    int returnCode = sqlite3_exec(db, sqlCreateTableUser, 0, 0, &errMsg);
    if (returnCode != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg);
        sqlite3_free(errMsg);
        return returnCode;
    } else {
        LOGI("Table created successfully");
    }


    // Create CONTACT table
    const char *sqlCreateTableContact = "CREATE TABLE IF NOT EXISTS CONTACT("
                                        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                        "CONTACT_NAME TEXT NOT NULL,"
                                        "CONTACT_NUMBER TEXT NOT NULL CHECK(LENGTH(CONTACT_NUMBER) = 10));";

    char *errMsg1 = 0;
    int returnCode1 = sqlite3_exec(db, sqlCreateTableContact, 0, 0, &errMsg1);
    if (returnCode1 != SQLITE_OK) {
        LOGE("SQL error: %s", errMsg1);
        sqlite3_free(errMsg1);
        return returnCode1;
    } else {
        LOGI("Table created successfully");
    }

    return SQLITE_OK;
}
