#ifndef DB_H
#define DB_H

#include <sqlite3.h>

int db_init(sqlite3 **db);
void db_close(sqlite3 *db);

int db_add_user(sqlite3 *db, const char *name, const char *email);
char *db_get_users(sqlite3 *db);
int db_delete_user(sqlite3 *db, int id);
int db_update_user(sqlite3 *db, int id, const char *name, const char *email);

#endif
