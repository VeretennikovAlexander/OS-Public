#include <iostream>
#include <windows.h>


bool Error()
{
    printf("Fatal error\n");
    return false;
}

size_t MaxSize = 5;

struct CPacket
{
    
};

CPacket NewPacket()
{
    CPacket Packet;
    Sleep(1000);
    return Packet;
}

void Consume(CPacket) 
{
    Sleep(2000);
}

struct CMutex
{
    HANDLE Mutex;

    bool Init(const TCHAR* Name)
    {
        Mutex = CreateMutex(NULL, 0, Name);
        if (!Mutex)
        {
            printf("Mutex cannot be created\n");
            return false;
        }

        DWORD LastError = GetLastError();
        if (LastError == ERROR_ALREADY_EXISTS)
        {
            printf("Mutex is already created\n");
        } else
            printf("Mutex has been created\n");
        return true;
    }

    bool Lock()
    {
        DWORD Code = WaitForSingleObject(Mutex, INFINITE);
        if ((Code == WAIT_OBJECT_0) || (Code == WAIT_ABANDONED))
        {
            return true;
        }
        else
        {
            printf("Cannot lock mutex\n");            
            return false;
        }
    }

    bool Unlock()
    {
        return ReleaseMutex(Mutex);
    }

};

struct CEvent
{
    HANDLE Event;

    bool Init(const TCHAR* Name)
    {
        Event = CreateEvent(NULL, TRUE, FALSE, Name);

        if (!Event)
        {
            printf("Event %S cannot be created\n", Name);
            return false;
        }

        DWORD LastError = GetLastError();
        if (LastError == ERROR_ALREADY_EXISTS)
        {
            printf("Event %S is already created\n", Name);
        }
        else
            printf("Event %S has been created\n", Name);
        return true;
    }

    bool Wait()
    {
        DWORD Code = WaitForSingleObject(Event, INFINITE);
        if (Code == WAIT_OBJECT_0)
        {
            return true;
        }
        else
        {
            printf("Cannot wait event\n");
            return false;
        }
    }

    void Reset()
    {
        ResetEvent(Event);
    }

    void Set()
    {
        SetEvent(Event);
    }

};

#pragma pack(push,1)

struct CQueue
{
    size_t First;
    size_t Last;
    size_t Size;   

    CPacket Data[1];

    size_t Increment(size_t Index) 
    {
        Index++;
        if (Index >= MaxSize)
            Index = 0;
        return Index;
    }    
  
    void Push(CPacket Packet)
    {
        Data[Last] = Packet;
        Last = Increment(Last);
        Size++;
    }

    CPacket Pop()
    {
        CPacket Result = Data[First];
        First = Increment(First);
        Size--;
        return Result;
    }
};

#pragma pack(pop)

CMutex Mutex;
CEvent BufferNotFull;
CEvent BufferNotEmpty;

struct CFileMapping
{
    HANDLE File;
    void* Ptr;
    bool Created = false;
    CQueue* Queue;

    bool Init(const TCHAR* Name)
    {
        size_t BufferSize = sizeof(size_t) + sizeof(size_t) + sizeof(CPacket) * MaxSize;
        LARGE_INTEGER Size;
        Size.QuadPart = BufferSize;

        File = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, 
            PAGE_READWRITE, Size.HighPart, Size.LowPart, Name);

        if (!File)
        {
            printf("FileMapping cannot be created\n");
            return false;
        }

        DWORD LastError = GetLastError();
        if (LastError == ERROR_ALREADY_EXISTS)
        {
            printf("FileMapping is already created\n");
        }
        else
        {
            printf("FileMapping has been created\n");
            Created = true;
        }

        Ptr = MapViewOfFile(File, FILE_MAP_ALL_ACCESS, 0, 0, BufferSize);
        if (!Ptr)
        {
            printf("Cannot map view of file\n");
            return false;
        }
        Queue = (CQueue*)Ptr;

        printf("Map view of file OK\n");

        if (Created)
        {
            if (!Mutex.Lock())
                return Error();

            Queue->First = 0;
            Queue->Last = 0;
            Queue->Size = 0;
            Mutex.Unlock();
            printf("Queue has been initialized, size %d\n", (int)Queue->Size);
        }
        return true;
    }

};


CFileMapping Queue;

bool Producer()
{
    while (1)
    {
        CPacket Packet = NewPacket();
        
        printf("Packet has been created\n");

        if (!Mutex.Lock())
            return Error();

        while (true)
        {
            printf("Check queue %d\n", (int)Queue.Queue->Size);

            if (Queue.Queue->Size >= MaxSize)
            {
                BufferNotFull.Reset();
                Mutex.Unlock();
                BufferNotFull.Wait();

                if (!Mutex.Lock())
                    return Error();
            }
            else
                break;
        }

        printf("Push\n");

        Queue.Queue->Push(Packet);

        printf("Push, queue size %d\n", (int)Queue.Queue->Size);

        BufferNotEmpty.Set();

        Mutex.Unlock();
    }

    return true;
}

bool Consumer()
{
    while (1)
    {        
        printf("Waiting for a packet\n");

        if (!Mutex.Lock())
            return Error();

        while (true)
        {
            printf("Check queue %d\n", (int)Queue.Queue->Size);

            if (Queue.Queue->Size == 0)
            {
                BufferNotEmpty.Reset();
                Mutex.Unlock();
                BufferNotEmpty.Wait();

                if (!Mutex.Lock())
                    return Error();
            }
            else
                break;
        }

        printf("Pop\n");

        CPacket Packet = Queue.Queue->Pop();

        printf("Pop, queue size %d\n", (int)Queue.Queue->Size);

        BufferNotFull.Set();

        Mutex.Unlock();

        Consume(Packet);
    }

    return true;
}

void Work(bool IsConsumer)
{
    if (!Mutex.Init(TEXT("MyMutex2021_12_10")))
        return;

    if (!Queue.Init(TEXT("MyFileMapping2021_12_10")))
        return;  

    if (!BufferNotFull.Init(TEXT("MyEvent_021_12_10_BufferNotFull")))
        return;

    if (!BufferNotEmpty.Init(TEXT("MyEvent_021_12_10_BufferNotEmpty")))
        return;

    if (IsConsumer)
        Consumer(); else
        Producer();

    printf("OK\n");

    getchar();

    printf("Unlock\n");

    Mutex.Unlock();
}


int main(unsigned int Count, char** Args)
{
    bool IsConsumer = (Count > 1) && (strcmp(Args[1], "consumer") == 0);
    printf("Consumer %d\n", (int)IsConsumer);

    Work(IsConsumer);

    getchar();
}
