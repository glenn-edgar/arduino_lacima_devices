#include "Redis.h"
#include "RedisInternal.h"

#pragma mark Redis class implemenation

RedisReturnValue Redis::authenticate(const char* password)
{
    if(conn.connected())
    {
        int passwordLength = strlen(password);
        if (passwordLength > 0)
        {
            auto cmdRet = RedisCommand("AUTH", ArgList{password}).issue(conn);
            return cmdRet->type() == RedisObject::Type::SimpleString && (String)*cmdRet == "OK" 
                ? RedisSuccess : RedisAuthFailure;
        }

        return RedisSuccess;
    }

    return RedisNotConnectedFailure;
}

bool Redis::set(const char* key, const char* value)
{
    return ((String)*RedisCommand("SET", ArgList{key, value}).issue(conn)).indexOf("OK") != -1;
}

#define TRCMD(t, c, ...) return RedisCommand(c, ArgList{__VA_ARGS__}).issue_typed<t>(conn)

String Redis::get(const char* key) 
{
    TRCMD(String, "GET", key);
}

bool Redis::del(const char* key)
{
    TRCMD(bool, "DEL", key);
}

int Redis::append(const char* key, const char* value)
{
    TRCMD(int, "APPEND", key, value);
}

int Redis::publish(const char* channel, const char* message)
{
    TRCMD(int, "PUBLISH", channel, message);
}

bool Redis::exists(const char* key)
{
    TRCMD(bool, "EXISTS", key);
}

bool Redis::_expire_(const char* key, int arg, const char* cmd_var)
{
    TRCMD(bool, cmd_var, key, String(arg));
}

bool Redis::persist(const char* key)
{
    TRCMD(bool, "PERSIST", key);
}

int Redis::_ttl_(const char* key, const char* cmd_var)
{
    TRCMD(int, cmd_var, key);
}

bool Redis::_hset_(const char* key, const char* field, const char* value, const char* cmd_var)
{
    TRCMD(int, cmd_var, key, field, value);
}

String Redis::hget(const char* key, const char* field) 
{ 
    TRCMD(String, "HGET", key, field); 
}

bool Redis::hdel(const char* key, const char* field) 
{ 
    TRCMD(bool, "HDEL", key, field); 
}

int Redis::hlen(const char* key) 
{ 
    TRCMD(int, "HLEN", key); 
}

int Redis::hstrlen(const char* key, const char* field)
{
    TRCMD(int, "HSTRLEN", key, field);
}

int Redis::llen(const char* key)
{
  //return ((String)*RedisCommand("LLEN", ArgList{key}).issue(conn));
  TRCMD(int, "LLEN", key);
}

bool Redis::select(const int db )
{
    TRCMD(bool, "SELECT", String(db));
    
}

bool Redis::ltrim(const char* key,const int start, const int stop )
{
    TRCMD(bool, "LTRIM",key, String(start), String(stop));
    
}


bool Redis::lpush(const char* key,const char* value )
{
    TRCMD(bool, "LPUSH", key, value);
    
}

String Redis::job_queue_pop(const char *key )
{

        return this->lpop(key);
        
    
    
    
}  
    
bool Redis::job_queue_push(const char *key ,const char *value, int limit )
{
    this->lpush(key,value);
    this->ltrim(key,0,limit-1);
}
    
bool Redis::stream_add_capped(const char *key, const char *value,int max_length)
{
   
    TRCMD(bool,"XADD" ,key ,"MAXLEN" ,"~", String(max_length) ,"*", "data",value);
     
 
}   

bool Redis::stream_add(const char *key, const char *value)
{
   
    TRCMD(bool,"XADD" ,key  ,"*", "data",value);
     
 
}
 

String Redis::lpop(const char* key ) 
{ 
    return ((String)*RedisCommand("LPOP", ArgList{key}).issue(conn));
} 

String Redis::rpop(const char* key ) 
{ 
    TRCMD(String, "RPOP", key); 
}

bool Redis::hexists(const char* key, const char* field)
{
    TRCMD(bool, "HEXISTS", key, field);
}

std::vector<String> Redis::lrange(const char* key, int start, int stop)
{
    auto rv = RedisCommand("LRANGE", ArgList{key, String(start), String(stop)}).issue(conn);
    return rv->type() == RedisObject::Type::Array 
        ? (std::vector<String>)*((RedisArray*)rv.get()) 
        : std::vector<String>();
}