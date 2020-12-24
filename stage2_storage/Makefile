
CROSS_COMPILE=arm-linux-
CC=$(CROSS_COMPILE)gcc
CFLAGS= -c -g
#LDFLAGS= -lpthread -L ./lib -lsqlite3
LDFLAGS= -lpthread

OBJS=main.o data_global.o pthread_transfer.o \
	 pthread_client_request.o pthread_buzzer.o pthread_led.o\
	 pthread_sqlite.o \
	 pthread_refresh.o pthread_sms.o

monitor_obj :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *o ./obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

install:
	sudo cp monitor_obj ~/source/rootfs/ 

.PHONY:clean
clean:
	rm *.o monitor_obj -rf 

