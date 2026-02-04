CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iincludes
LDFLAGS = -lsqlite3

SRC = src/main.c src/http.c src/router.c src/db.c
OUT = dirtblue

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
