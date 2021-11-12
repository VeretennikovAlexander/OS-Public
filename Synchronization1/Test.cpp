#include <iostream>
#include <list>
#include <thread>
#include <Windows.h>

class CCreateSemaphoreException
{

};

class CDownSemaphoreException
{

};

class CSemaphore
{
    HANDLE H;
public:

    CSemaphore(int Inititial, int Max)
    {
        H = CreateSemaphore(NULL, Inititial, Max, NULL);
        if (!H)
        {
            throw CCreateSemaphoreException();
        }
    }

    void Down()
    {
        while (true)
        {
            DWORD WaitResult = WaitForSingleObject(H, 0);
            if (WaitResult == WAIT_OBJECT_0)
            {
                break;
            }
        }
    }

    void Up()
    {
        ReleaseSemaphore(H, 1, NULL);
    }

    ~CSemaphore()
    {
        CloseHandle(H);
    }
};


using namespace std;

struct CPacket
{
    int Data1;
    int Data2;
};

list<CPacket> Queue;
CSemaphore Empty(10, 10);
CSemaphore Fill(0, 10);
CSemaphore Mutex(1, 1);

CPacket produce()
{
    CPacket Packet = { 0 };
    this_thread::sleep_for(1000ms);
    return Packet;
}

void consume(CPacket Packet)
{
    this_thread::sleep_for(5000ms);
}

void ProducerThreadProc()
{
    printf("Producer has been started\n");
    
    while (1)
    {
        CPacket Packet = produce();
        Empty.Down();
        Mutex.Down();
        Queue.push_back(Packet);
        printf("Push packet %d\n",(int)Queue.size());
        Mutex.Up();
        Fill.Up();
    }
}

void CustomerThreadProc()
{
    printf("Customer has been started\n");
    CPacket Packet;
    while (1)
    {
        Fill.Down();
        Mutex.Down();

        Packet = Queue.front();
        Queue.pop_front();
        printf("Pop packet\n");

        Mutex.Up();
        Empty.Up();

        consume(Packet);
    }
}

int main()
{
    enum { producerCount = 2 };
    enum { customerThread = 5 };

    thread* producers[producerCount];
    thread* consumer[customerThread];
   
    for (int i = 0; i < producerCount; i++)
    {
        producers[i] = new thread(ProducerThreadProc);
    }

    for (int i = 0; i < customerThread; i++)
    {
        consumer[i] = new thread(CustomerThreadProc);
    }

    for (int i = 0; i < producerCount; i++)
    {
        producers[i]->join();
        delete producers[i];
    }

    for (int i = 0; i < producerCount; i++)
    {
        consumer[i]->join();
        delete consumer[i];
    }
    std::cout << "Hello World!\n";
}

