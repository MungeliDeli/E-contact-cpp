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

    // Function to get all contacts from CONTACT table
extern "C" JNIEXPORT jobject JNICALL
Java_com_example_e_1contact_DbHelper_getAllContacts(JNIEnv *env, jobject /* this */) {
    const char *sqlQuery = "SELECT * FROM CONTACT;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        LOGE("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return nullptr;
    }

    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jobject arrayList = env->NewObject(arrayListClass, env->GetMethodID(arrayListClass, "<init>", "()V"));
    jmethodID arrayListAdd = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

    jclass contactClass = env->FindClass("com/example/e_contact/Contact");
    jmethodID contactConstructor = env->GetMethodID(contactClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;)V");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int contactID = sqlite3_column_int(stmt, 0);
        const char *contactName = (const char *) sqlite3_column_text(stmt, 1);
        const char *contactNumber = (const char *) sqlite3_column_text(stmt, 2);

        jstring contactNameStr = env->NewStringUTF(contactName);
        jstring contactNumberStr = env->NewStringUTF(contactNumber);

        jobject contactObj = env->NewObject(contactClass, contactConstructor, contactID, contactNameStr, contactNumberStr);
        env->CallBooleanMethod(arrayList, arrayListAdd, contactObj);

        env->DeleteLocalRef(contactNameStr);
        env->DeleteLocalRef(contactNumberStr);
        env->DeleteLocalRef(contactObj);
    }

    sqlite3_finalize(stmt);

    return arrayList;
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

// Function to insert a contact into CONTACT table
extern "C" JNIEXPORT jint JNICALL
Java_com_example_e_1contact_DbHelper_insertContact(JNIEnv *env, jobject, jstring contact_name, jstring contact_number) {
    const char *sqlInsert = "INSERT INTO CONTACT(CONTACT_NAME, CONTACT_NUMBER) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    const char *contactNumberCStr = env->GetStringUTFChars(contact_number, nullptr);

    // Check if the contact already exists
    const char *sqlCheckExistence = "SELECT COUNT(*) FROM CONTACT WHERE CONTACT_NUMBER = ?";
    sqlite3_stmt *existenceStmt;

    int contactCount = 0;
    int rc = sqlite3_prepare_v2(db, sqlCheckExistence, -1, &existenceStmt, 0);
    if (rc != SQLITE_OK) {
        LOGE("Failed to prepare existence check statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(existenceStmt, 1, contactNumberCStr, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(existenceStmt) == SQLITE_ROW) {
        contactCount = sqlite3_column_int(existenceStmt, 0);
    }

    sqlite3_finalize(existenceStmt);

    if (contactCount > 0) {
        env->ReleaseStringUTFChars(contact_number, contactNumberCStr);
        return SQLITE_CONSTRAINT;
    }

    rc = sqlite3_prepare_v2(db, sqlInsert, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        LOGE("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    const char *contactNameCStr = env->GetStringUTFChars(contact_name, nullptr);

    sqlite3_bind_text(stmt, 1, contactNameCStr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contactNumberCStr, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("Execution failed: %s", sqlite3_errmsg(db));
    } else {
        LOGI("Record inserted successfully");
    }

    sqlite3_finalize(stmt);

    env->ReleaseStringUTFChars(contact_name, contactNameCStr);
    env->ReleaseStringUTFChars(contact_number, contactNumberCStr);

    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}

// Function to get all contacts from CONTACT table
extern "C" JNIEXPORT jobject JNICALL
Java_com_example_e_1contact_DbHelper_getAllContacts(JNIEnv *env, jobject /* this */) {
    const char *sqlQuery = "SELECT * FROM CONTACT;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        LOGE("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return nullptr;
    }

    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jobject arrayList = env->NewObject(arrayListClass, env->GetMethodID(arrayListClass, "<init>", "()V"));
    jmethodID arrayListAdd = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");

    jclass contactClass = env->FindClass("com/example/e_contact/Contact");
    jmethodID contactConstructor = env->GetMethodID(contactClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;)V");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int contactID = sqlite3_column_int(stmt, 0);
        const char *contactName = (const char *) sqlite3_column_text(stmt, 1);
        const char *contactNumber = (const char *) sqlite3_column_text(stmt, 2);

        jstring contactNameStr = env->NewStringUTF(contactName);
        jstring contactNumberStr = env->NewStringUTF(contactNumber);

        jobject contactObj = env->NewObject(contactClass, contactConstructor, contactID, contactNameStr, contactNumberStr);
        env->CallBooleanMethod(arrayList, arrayListAdd, contactObj);

        env->DeleteLocalRef(contactNameStr);
        env->DeleteLocalRef(contactNumberStr);
        env->DeleteLocalRef(contactObj);
    }

    sqlite3_finalize(stmt);

    return arrayList;
}

// Function to update a contact in the CONTACT table
extern "C" JNIEXPORT jint JNICALL
Java_com_example_e_1contact_DbHelper_updateContact(JNIEnv *env, jobject, jint contactId, jstring contactName, jstring contactNumber) {
    const char *sqlUpdate = "UPDATE CONTACT SET CONTACT_NAME = ?, CONTACT_NUMBER = ? WHERE ID = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        LOGE("Failed to prepare statement: %s", sqlite3_errmsg(db));
        return rc;
    }

    const char *contactNameCStr = env->GetStringUTFChars(contactName, nullptr);
    const char *contactNumberCStr = env->GetStringUTFChars(contactNumber, nullptr);

    sqlite3_bind_text(stmt, 1, contactNameCStr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contactNumberCStr, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, contactId);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        LOGE("Execution failed: %s", sqlite3_errmsg(db));
    } else {
        LOGI("Record updated successfully");
    }

    sqlite3_finalize(stmt);

    env->ReleaseStringUTFChars(contactName, contactNameCStr);
    env->ReleaseStringUTFChars(contactNumber, contactNumberCStr);

    return rc == SQLITE_DONE ? SQLITE_OK : rc;
}






