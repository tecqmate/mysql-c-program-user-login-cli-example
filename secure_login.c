#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LINE 256

// Structure to hold DB config
typedef struct {
    char host[100];
    char user[100];
    char pass[100];
    char db[100];
    unsigned int port;
} DBConfig;

void load_env(DBConfig *config) {
    FILE *file = fopen(".env", "r");
    if (!file) {
        printf("Could not open .env file\n");
        exit(1);
    }

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            if (strcmp(key, "DB_HOST") == 0)
                strcpy(config->host, value);
            else if (strcmp(key, "DB_USER") == 0)
                strcpy(config->user, value);
            else if (strcmp(key, "DB_PASS") == 0)
                strcpy(config->pass, value);
            else if (strcmp(key, "DB_NAME") == 0)
                strcpy(config->db, value);
            else if (strcmp(key, "DB_PORT") == 0)
                config->port = atoi(value);
        }
    }

    fclose(file);
}

void hide_password(char *password) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    fgets(password, 100, stdin);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main() {

    DBConfig config;
    load_env(&config);

    MYSQL *conn = mysql_init(NULL);

    if (!mysql_real_connect(conn,
                            config.host,
                            config.user,
                            config.pass,
                            config.db,
                            config.port,
                            NULL,
                            0)) {
        printf("Connection Failed: %s\n", mysql_error(conn));
        return 1;
    }

    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    const char *query = "SELECT id FROM users WHERE username=? AND password=?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        printf("Prepare failed: %s\n", mysql_stmt_error(stmt));
        return 1;
    }

    char username[50];
    char password[100];

    printf("Enter Username: ");
    scanf("%49s", username);
    getchar();

    printf("Enter Password: ");
    hide_password(password);
    password[strcspn(password, "\n")] = 0;
    printf("\n");

    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = username;
    bind[0].buffer_length = strlen(username);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = password;
    bind[1].buffer_length = strlen(password);

    mysql_stmt_bind_param(stmt, bind);
    mysql_stmt_execute(stmt);

    int id;
    bool is_null;
    unsigned long length;

    MYSQL_BIND result_bind[1];
    memset(result_bind, 0, sizeof(result_bind));

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = &id;
    result_bind[0].is_null = &is_null;
    result_bind[0].length = &length;

    mysql_stmt_bind_result(stmt, result_bind);
    mysql_stmt_store_result(stmt);

    if (mysql_stmt_fetch(stmt) == 0)
        printf("Login Successful!\n");
    else
        printf("Invalid Username or Password.\n");

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return 0;
}
