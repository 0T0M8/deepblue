#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "router.h"
#include "http.h"
#include "db.h"

void route(int client, const char *req, sqlite3 *db)
{
    char method[8], path[256];
    sscanf(req, "%7s %255s", method, path);

    /* STATIC */
    if (!strcmp(method, "GET") && serve_static(client, path))
        return;

    /* GET */
    if (!strcmp(method, "GET") && !strcmp(path, "/users")) {
        char *json = db_get_users(db);
        send_response(client, "application/json", json);
        free(json);
        return;
    }

    /* POST (JSON) */
    if (!strcmp(method, "POST") && !strcmp(path, "/users")) {
        char *body = get_body(req);
        char name[128], email[128];

        if (body &&
            json_get_string(body, "name", name, sizeof(name)) &&
            json_get_string(body, "email", email, sizeof(email)) &&
            db_add_user(db, name, email) == 0)
            send_response(client, "application/json", "{\"status\":\"created\"}");
        else
            send_response(client, "application/json", "{\"error\":\"bad json\"}");

        free(body);
        return;
    }

    /* PUT (JSON) */
    if (!strcmp(method, "PUT") && !strcmp(path, "/users")) {
        char *body = get_body(req);
        int id;
        char name[128], email[128];

        if (body &&
            json_get_int(body, "id", &id) &&
            json_get_string(body, "name", name, sizeof(name)) &&
            json_get_string(body, "email", email, sizeof(email)) &&
            db_update_user(db, id, name, email) == 0)
            send_response(client, "application/json", "{\"status\":\"updated\"}");
        else
            send_response(client, "application/json", "{\"error\":\"bad json\"}");

        free(body);
        return;
    }

    /* DELETE */
    if (!strcmp(method, "DELETE") && strstr(path, "/users")) {
        char *id_str = get_query_param(path, "id");
        int id = id_str ? atoi(id_str) : -1;
        free(id_str);

        if (id > 0 && db_delete_user(db, id) == 0)
            send_response(client, "application/json", "{\"status\":\"deleted\"}");
        else
            send_response(client, "application/json", "{\"error\":\"delete failed\"}");
        return;
    }

    send_response(client, "text/plain", "404 Not Found");
}
