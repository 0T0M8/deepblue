#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define DB_PATH "db/users.db"

int db_init(sqlite3 **db)
{
    if (sqlite3_open(DB_PATH, db) != SQLITE_OK)
        return -1;

    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "email TEXT NOT NULL UNIQUE);";

    char *err = NULL;
    if (sqlite3_exec(*db, sql, NULL, NULL, &err) != SQLITE_OK) {
        sqlite3_free(err);
        return -1;
    }

    return 0;
}

void db_close(sqlite3 *db)
{
    sqlite3_close(db);
}

int db_add_user(sqlite3 *db, const char *name, const char *email)
{
    const char *sql = "INSERT INTO users(name,email) VALUES(?,?)";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE ? 0 : -1;
}

char *db_get_users(sqlite3 *db)
{
    const char *sql = "SELECT id,name,email FROM users";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    char *json = malloc(4096);
    strcpy(json, "[");

    int first = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (!first) strcat(json, ",");
        first = 0;

        char row[256];
        snprintf(row, sizeof(row),
            "{\"id\":%d,\"name\":\"%s\",\"email\":\"%s\"}",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_text(stmt, 1),
            sqlite3_column_text(stmt, 2)
        );
        strcat(json, row);
    }

    strcat(json, "]");
    sqlite3_finalize(stmt);
    return json;
}

int db_delete_user(sqlite3 *db, int id)
{
    const char *sql = "DELETE FROM users WHERE id=?";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE ? 0 : -1;
}

int db_update_user(sqlite3 *db, int id, const char *name, const char *email)
{
    const char *sql =
        "UPDATE users SET name=?, email=? WHERE id=?";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, id);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE ? 0 : -1;
}
