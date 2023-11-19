#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>

namespace Networks {
    MYSQL mysql_init() {
        MYSQL mysql;

        if (mysql_init(&mysql) == NULL) {
            printf("Cannot init mysql!\n");
            exit(-1);
        }

        MYSQL* ret = mysql_real_connect(
            &mysql,
            MYSQL_IP,
            "debian-sys-maint",
            "kUpaezk5soMAwGA0",
            "USERS",
            0, NULL, 0
        );

        if (ret == NULL) {
            printf("%s\n", mysql_error(&mysql));
            exit(-1);
        }
        else
            printf("Mysql Access Successful.\n");

        return mysql;
    }

}
#endif