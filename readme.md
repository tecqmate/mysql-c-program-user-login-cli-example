# rename the file env to .env
- update your db connections
- ensure your db is up

# compile option for linux & mac 
```
gcc secure_login.c -o secure_login $(mysql_config --cflags --libs)
```

# compile option for mac users using brew
```
gcc secure_login.c -o secure_login -I/opt/homebrew/opt/mysql-client/include \
-L/opt/homebrew/opt/mysql-client/lib -lmysqlclient
```
