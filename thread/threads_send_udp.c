/*
inspired by
https://stackoverflow.com/questions/17330435/what-happens-if-more-than-one-pthread-uses-a-same-function

-setup multiple threads
-re-use thread function
-let threads call a method at almost the same (?) time
-increment counter in that method
-send osc message
-use / don't use locks at different places

//tb/1708

gcc -o threads_send_udp threads_send_udp.c -lpthread -llo
*/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "lo/lo.h"

lo_server_thread st;
lo_server srv;
lo_address addr;
const char *local_port="14141";

const char *remote_host="127.0.0.1";
const char *remote_port="14142";

int thread_usleep=50;

static uint64_t common_counter=0;

static pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t increment_and_send_lock = PTHREAD_MUTEX_INITIALIZER;

void *thread1_function();
void *thread2_function();
void msg_send(char *message, int i);
void error(int num, const char *m, const char *path)
{
	fprintf(stderr,"error. liblo says: %d %s %s\n",num,m,path);
}

//=============================================================================
int main(void)
{
	st = lo_server_thread_new(local_port, error);
	lo_server_thread_start(st);
	srv=lo_server_thread_get_server(st);
	addr=lo_address_new(remote_host,remote_port);

	fprintf(stderr,"sending messages: localhost:%s -> %s:%s\n",local_port,remote_host,remote_port);

	pthread_t thread1, thread2, thread3, thread4, thread5;
	pthread_create( &thread1, NULL, thread1_function, NULL);
	pthread_create( &thread2, NULL, thread2_function, NULL);
	//re-use world function
	pthread_create( &thread3, NULL, thread2_function, NULL);
	pthread_create( &thread4, NULL, thread2_function, NULL);
	pthread_create( &thread5, NULL, thread2_function, NULL);

	while(1){usleep(10000);}///need stop condition
	lo_server_thread_free(st);
	return 0;
}

//=============================================================================
void *thread1_function()
{
	while(1)
	{
		msg_send("hello",1);
		usleep(thread_usleep);
	}
	pthread_exit(NULL);
}

//=============================================================================
void *thread2_function()
{
	while(1)
	{
		msg_send("world",2);
		usleep(thread_usleep);
	}
	pthread_exit(NULL);
}

//=============================================================================
void msg_send(char message[],int i)
{
	//blocking (if locked, other threads need to wait)
	// /!\ not locking this section can result in a non-linear series of "common_counter"
	pthread_mutex_lock (&increment_and_send_lock);
	lo_message msg=lo_message_new();
	lo_message_add_int32(msg,i);
	lo_message_add_string(msg,message);

	// /!\ not locking this section can result in gaps (unused counter values) and messages having the same "common_counter" value
	//==========
//	pthread_mutex_lock (&counter_lock);
	lo_message_add_int64(msg,common_counter);
	common_counter++;
//	pthread_mutex_unlock (&counter_lock);
	//==========

	lo_send_message_from (addr, srv, "/thread", msg);
	lo_message_free(msg);
	pthread_mutex_unlock (&increment_and_send_lock);
}

//=============================================================================
/*
output using no lock at all

oscdump 14142
...
127.0.0.1:14141 /thread ish 2 "world" 154350
127.0.0.1:14141 /thread ish 2 "world" 154351
127.0.0.1:14141 /thread ish 2 "world" 154352
127.0.0.1:14141 /thread ish 2 "world" 154354 <--- out of order
127.0.0.1:14141 /thread ish 2 "world" 154353 <---
127.0.0.1:14141 /thread ish 1 "hello" 154355
127.0.0.1:14141 /thread ish 2 "world" 154356 <--- double occurence /!\
127.0.0.1:14141 /thread ish 2 "world" 154356 <---
127.0.0.1:14141 /thread ish 2 "world" 154358 <--- 154357 missing /!\
127.0.0.1:14141 /thread ish 2 "world" 154359
127.0.0.1:14141 /thread ish 1 "hello" 154359


output using just &counter_lock
...
127.0.0.1:14141 /thread ish 2 "world" 249895
127.0.0.1:14141 /thread ish 2 "world" 249896
127.0.0.1:14141 /thread ish 2 "world" 249897
127.0.0.1:14141 /thread ish 2 "world" 249898
127.0.0.1:14141 /thread ish 1 "hello" 249899
127.0.0.1:14141 /thread ish 2 "world" 249900 
127.0.0.1:14141 /thread ish 2 "world" 249902 <--- out of order
127.0.0.1:14141 /thread ish 2 "world" 249903
127.0.0.1:14141 /thread ish 2 "world" 249901 <---
127.0.0.1:14141 /thread ish 1 "hello" 249904
127.0.0.1:14141 /thread ish 2 "world" 249905
127.0.0.1:14141 /thread ish 2 "world" 249906
127.0.0.1:14141 /thread ish 2 "world" 249907
127.0.0.1:14141 /thread ish 2 "world" 249908
127.0.0.1:14141 /thread ish 1 "hello" 249909

-"world" appears more often as expected
-counter values must not necessarily appear strictly linear +1 to previous

output using &increment_and_send_lock shows linear counter values

*/

//EOF
