main: lsyslog.c test.c lsyslogdaemon.c 
	gcc -c -Wall -Werror -fpic lsyslog.c
	gcc -shared -o libsyslog.so lsyslog.o 
	gcc -Wall -o out/lsyslogdaemon lsyslogdaemon.c
	gcc -o test test.c
