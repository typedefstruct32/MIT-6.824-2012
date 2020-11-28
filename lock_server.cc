// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock::lock(lock_protocol :: lockid_t d, int i) : lid(d), status(i)
{
	pthread_cond_init(&lcond, NULL);
}

lock::lock(lock_protocol::lockid_t d) : lid(d)
{
	pthread_cond_init(&lcond, NULL);
}


lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r) 
{
//  printf("we are in lockserver::acquire");
//  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
//  while ((lockmap.count(lid) > 0) && (lockmap[lid] == locked)) {}
// 
//  pthread_mutex_lock(&mutex);
//  {
//    lockmap[lid] = locked;
//	lockclt[lid] = clt;
//  }//critical section
//  lock_protocol::status ret = lock_protocol::OK;
//  return ret;
	lock_protocol::status ret = lock_protocol::OK;
	//printf("we are in lockserver::acquire");

	std::map<lock_protocol::lockid_t, lock*>::iterator iter;
	pthread_mutex_lock(&mutex);
	iter = lockmap.find(lid);
	if (iter != lockmap.end()) {
		while(iter->second->status !=lock::FREE) {
			pthread_cond_wait(&(iter->second->lcond), &mutex);
		}
		iter->second->status = lock::LOCKED;
		pthread_mutex_unlock(&mutex);
		return ret;
	}else {
		lock *new_lock = new lock(lid, lock::LOCKED);
		lockmap.insert(std::make_pair(lid, new_lock));
		pthread_mutex_unlock(&mutex);
		return ret;
	}
		
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
//  if (lockmap.count(lid) == 0) return lock_protocol::OK;
//  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
//  pthread_mutex_lock(&mutex);
//  {
//  	lockmap[lid] = isfree;
//  }//critical section
//  pthread_mutex_unlock(&mutex);
//  lock_protocol::status ret = lock_protocol::OK;
//  return ret;
	lock_protocol::status ret = lock_protocol::OK;

	//printf("we are in lockserver::release");
	std::map<lock_protocol::lockid_t, lock*>::iterator iter;
	pthread_mutex_lock(&mutex);
	iter = lockmap.find(lid);
	if (iter != lockmap.end()) {
		iter->second->status = lock::FREE;
		pthread_cond_signal(&(iter->second->lcond));
		pthread_mutex_unlock(&mutex);
		return ret;
	} else {
		pthread_mutex_unlock(&mutex);
		return lock_protocol::IOERR;
	}	
	
}



