#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include "router.h"
#include "db.h"

#define PORT 8080

int main(void)
{
    sqlite3 *db;
    db_init(&db);

    int server = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server, (struct sockaddr *)&addr, sizeof(addr));
    listen(server, 10);

    printf("🚀 DirtBlue running at http://localhost:%d\n", PORT);

    while (1) {
        int client = accept(server, NULL, NULL);
        char req[4096] = {0};
        read(client, req, sizeof(req) - 1);

        route(client, req, db);
        close(client);
    }

    db_close(db);
    return 0;
}
