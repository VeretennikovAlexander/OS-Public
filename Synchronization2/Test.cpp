#include <iostream>

#include <list>
#include <thread>
#include <mutex>

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
    std::this_thread::sleep_for(1000ms);
    CPacket Packet;
    return Packet;
}

void consume(CPacket packet)
{
    std::this_thread::sleep_for(2000ms);
}

mutex criticalSection;
condition_variable bufferNotEmpty;
condition_variable bufferNotFull;
list<CPacket> queue;
size_t maxSize = 10;

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
                bufferNotEmpty.wait(lock);

            queue.push_back(packet);

            printf("Producer %d, queue size %d\n", (int)index, (int)queue.size());

            bufferNotFull.notify_one();
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
                bufferNotFull.wait(lock);

            packet = queue.front();
            queue.pop_front();
            
            bufferNotEmpty.notify_one();
        }

        consume(packet);
    }
}

int main()
{
    enum { producerCount = 2, consumerCount = 2 };
   
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
