# compile option for mac users
```
gcc secure_login.c -o secure_login -I/opt/homebrew/opt/mysql-client/include \
-L/opt/homebrew/opt/mysql-client/lib -lmysqlclient
```
# compile option for linux & mac 
```
gcc secure_login.c -o secure_login $(mysql_config --cflags --libs)
```
