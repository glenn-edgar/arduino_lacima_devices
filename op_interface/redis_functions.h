#ifndef REDIS_SETUP_H__
#define REDIS_SETUP_H__

#define SERVER_QUEUE 0


void display_redis_keys(void);
void display_redis_config(void);
void setup_redis_service(void);


void redis_stream_add(const char *key, char *data );

void redis_job_queue_push(const char *key, char *data,int queue_length);


int redis_job_queue_length(int queue_type);

String redis_job_queue_pop(int queue_type, int *valid_flag);

    
void redis_message_send_heart_beat(void);   

void redis_message_send_push_message(void);

void redis_send_internal_stream(int length, char **keys, double *values);

void test_function(void);
#endif
