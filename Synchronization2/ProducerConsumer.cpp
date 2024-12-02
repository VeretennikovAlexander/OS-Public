#include <iostream>

#include <list>
#include <thread>
#include <mutex>

#include <condition_variable>

using namespace std;

struct CPacket
{
    CPacket()
    {
    }

    ~CPacket()
    {

    }
};

CPacket produce()
{
    this_thread::sleep_for(500ms);
    CPacket Packet;
    return Packet;
}

void consume(CPacket packet)
{
    this_thread::sleep_for(500ms);
}

mutex criticalSection;
condition_variable bufferNotEmpty;
condition_variable bufferNotFull;
list<CPacket> queue;
size_t maxSize = 100;

void producer(size_t index)
{
    printf("Producer %d has been started\n", (int)index);
    CPacket packet;

    while (1)
    {
        packet = produce();

        {
            unique_lock<mutex> lock(criticalSection);

            while (queue.size() >= maxSize)
                bufferNotFull.wait(lock);

            queue.push_back(packet);

            printf("Producer %d, queue size %d\n", (int)index, (int)queue.size());

            bufferNotEmpty.notify_one();
        }
    }
   
}

void consumer(size_t index)
{
    printf("Consumer %d has been started\n", (int)index);
    CPacket packet;

    while (1)
    {
        {
            unique_lock<mutex> lock(criticalSection);

            while (queue.size() == 0)
                bufferNotEmpty.wait(lock);

            packet = queue.front();
            queue.pop_front();
            
            bufferNotFull.notify_one();
        }

        consume(packet);
    }
}

int main()
{
    enum { producerCount = 10, consumerCount = 10 };
   
    thread* producers[producerCount];
    thread* consumers[consumerCount];

    for (size_t i = 0; i< producerCount; i++)
    {
        producers[i] = new thread(producer, i);
    }
    for (size_t i = 0; i < consumerCount; i++)
    {
        consumers[i] = new thread(consumer, i);
    }

    for (auto& thr : producers)
    {
        thr->join();
    }
    for (auto& thr : consumers)
    {
        thr->join();
    }

    std::cout << "Hello World!\n";

    for (auto& thr : producers)
    {
        delete thr;
    }
    for (auto& thr : consumers)
    {
        delete thr;
    }
}
