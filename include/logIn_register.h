#ifndef LOGIN_REGISTER_H
#define LOGIN_REGISTER_H

#include "include.h"
#include "basic.h"
#include "Mysql.h"

namespace Networks {
    /*
     *  check uer's id and key
     */
    bool check_user_key(MYSQL& com_mysql, int id, long long& key, std::string& name) {
        std::string sql_command;
        sql_command = "select * from users where id = " + std::to_string(id) + ';';

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        MYSQL_RES* pRes;
        MYSQL_ROW hs;

        if ((pRes = mysql_store_result(&com_mysql)) == NULL) {
            printf("%s\n", mysql_error(&com_mysql));
            return 0;
        }
        while (hs = mysql_fetch_row(pRes)) {
            if (atoi(hs[2]) == key) {
                name = hs[1];
                mysql_free_result(pRes);
                return true;
            }
        }
        mysql_free_result(pRes);
        return false;
    }

    /*
    *  insert a new user and return its id
    */
    bool insert_new_user(MYSQL& com_mysql, int& id, std::string& name, long long& key) {
        std::string sql_command;

        // query the number of users
        sql_command = "select * from users;";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        MYSQL_RES* pRes;

        if ((pRes = mysql_store_result(&com_mysql)) == NULL) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }
        id = mysql_num_rows(pRes) + 1;

        // insert new data
        sql_command = "insert into users values ( id = " + std::to_string(id) + ", name = `" + name + "`, key = " + std::to_string(key) + ");";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }

        return true;
    }

    /*
     *  rename a user
     */
    bool rename_user(MYSQL& com_mysql, int id, std::string& name, long long& key) {
        std::string sql_command;
        sql_command = "update users set name = `" + name + "` where id = " + std::to_string(id) + ");";

        if (mysql_query(&com_mysql, sql_command.c_str()) != 0) {
            printf("%s\n", mysql_error(&com_mysql));
            return false;
        }
        return true;
    }
}
#endif