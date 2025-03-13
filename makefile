CFLAGS = -pthread -lpthread -std=gnu99 -lrt


.PHONY: all clean

all: task1_client task1_server task2_client task2_server task3_client task3_server 

clean:
	$(RM) task1_client task1_server task2_client task2_server task3_client task3_server 

task1_client: task1_client.c
	$(CC) $(CFLAGS) $^ -o $@
  
task1_server: task1_server.c
	$(CC) $(CFLAGS) $^ -o $@

task2_client: task2_client.c
	$(CC) $(CFLAGS) $^ -o $@

task2_server: task2_server.c 
	$(CC) $(CFLAGS) $^ -o $@

task3_client: task3_client.c
	$(CC) $(CFLAGS) $^ -o $@

task3_server: task3_server.c
	$(CC) $(CFLAGS) $^ -o $@
  