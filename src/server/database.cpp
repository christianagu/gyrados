#include <mysql/mysql.h>
#include <cppconn/prepared_statement.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

//new
void connectToDatabase() {
    conn = mysql_init(NULL);
    if (conn == NULL) {

        std::cerr << "mysql_init() failed\n";
        exit(EXIT_FAILURE);
    }
    if (mysql_real_connect(conn, "localhost", "username", "password", "gyrados", 0, NULL, 0) == NULL) {
        std::cerr << "mysql_real_connect() failed\n";
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }
}

bool authenticateUser(const std::string& username, const std::string& password){
    std::string query = "SELECT * FROM users WHERE username='" + username + "' AND password='" + password + "';'";

    if (mysql_query(conn, query.c_str())) {
        std::cerr << "SELECT * FROM users failed. " << mysql_error(conn) << "\n";
        return false;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        std::cerr << "mysql_store_result() failed. " << mysql_error(conn) << "\n";
        return false;
    }

    int num_rows = mysql_num_rows(res);
    mysql_free_result(res);

    return num_rows > 0;

}

void closeConnection(){
    mysql_close(conn);
}