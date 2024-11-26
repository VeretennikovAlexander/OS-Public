
#include <stdio.h>

#include <list>

#include <semaphore>

#include <thread>

#include <chrono>


using namespace std;

enum {N = 100};

counting_semaphore<N> queueEmpty(N);
counting_semaphore<N> queueFull(0);
binary_semaphore m(1);

struct CItem
{

};

list<CItem> queue;

CItem produce()
{
      return CItem();
};

void consume(CItem Item)
{
};

time_t getCurrentTimeMs()
{
    auto now = chrono::system_clock::now();

    auto duration = now.time_since_epoch();

    auto milliseconds
        = chrono::duration_cast<chrono::milliseconds>(
              duration)
              .count();

    return milliseconds;
};

time_t startTime = getCurrentTimeMs();
long processed = 0;

void stat()
{
  time_t currentTime = getCurrentTimeMs();
  if (currentTime - startTime < 1000)
     return;
  startTime = currentTime;

 printf("Queue size %d, processed %d\r\n", (int)queue.size(), (int)processed);
}

void producer()
{
 printf("Starting producer\r\n");
 while (true)
 {
       CItem item = produce();
       queueEmpty.acquire();
       m.acquire();
       queue.push_back(item);
       stat();
       m.release();
       queueFull.release();
 }
}

void consumer()
{
 printf("Starting consumer\r\n");

 while (true)
 {
       CItem item;
       queueFull.acquire();
       m.acquire();
       item = queue.front();
       queue.pop_back();
       processed++;
       stat();
       m.release();
       queueEmpty.release();
       consume(item);
 }
}


int main(){
    printf("Starting\r\n");
    list<thread*> threads;
    for (int i = 0; i<10; i++)
    {
     thread *t1 = new thread(producer);
     threads.push_back(t1);
    }
    for (int i = 0; i<10; i++)
    {
     thread *t1 = new thread(consumer);
     threads.push_back(t1);
    }
    for (auto item : threads)
        item->join();
    for (auto item : threads)
        delete item;
    return 0;
}