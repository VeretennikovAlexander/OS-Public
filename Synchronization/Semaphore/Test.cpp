/*

Пример для курса Операционные системы, УрФУ, 2017-2018. Веретенников А. Б.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <windows.h>
#include <stdio.h>
#include <atomic>
#include <list>
#include <time.h>

using namespace std;

/*
	Задача производителя и потребителя с помощью семафора.
	(Мьютекс - семафор с максимальным значением 1).
*/

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

LONGLONG GetTimeMs()
{
	SYSTEMTIME S = { 0 };
	FILETIME F = { 0 };
	GetSystemTime(&S);
	SystemTimeToFileTime(&S, &F);
	LARGE_INTEGER L;
	L.HighPart = F.dwHighDateTime;
	L.LowPart = F.dwLowDateTime;
	return L.QuadPart / (1000 * 10);
}

size_t N = 1000;

CSemaphore Mutex(1, 1);
CSemaphore Empty(N, N);
CSemaphore Full(0, N);

LONGLONG Start = GetTimeMs();

list<int> Queue;

size_t Processed = 0;
size_t MaxItems = 0;

class CProducerThread
{	
	int Produce()
	{
		return 1;
	}

	void Execute()
	{
		while (true)
		{
			Empty.Down();
			Mutex.Down();
			Queue.push_back(Produce());

			/*
			For queue length analysis.
			*/
			MaxItems = max(MaxItems, Queue.size());

			Mutex.Up();
			Full.Up();
		}
	}

	static DWORD WINAPI ThreadProc(
		LPVOID lpParameter
	)
	{
		CProducerThread* Args = (CProducerThread*)lpParameter;

		Args->Execute();

		return (DWORD)0;
	};

	HANDLE Handle;

public:

	CProducerThread()
	{
		DWORD ThreadId;
		Handle = CreateThread(0, 0, ThreadProc, (LPVOID)this, 0, &ThreadId);
	}

	void Wait()
	{
		if (Handle != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(Handle, INFINITE);
		}
	}

};

class CConsumerThread
{
	void CheckProcessed()
	{
		Processed++;

		if (Processed % 100000 == 0)
		{
			LONGLONG Delta = GetTimeMs() - Start;
			LONGLONG PerSecond = (Processed * 1000) / Delta;

			printf("Processed %d, max %d, current %d, time %d, per second %d\r\n",
				(int)Processed, (int)MaxItems, Queue.size(), (int)(Delta), (int)PerSecond);
		}
	}

	void Execute()
	{
		while (true)
		{
			Full.Down();
			Mutex.Down();

			MaxItems = max(MaxItems, Queue.size());

			Queue.pop_back();

			CheckProcessed();

			Mutex.Up();
			Empty.Up();
		}
	}

	static DWORD WINAPI ThreadProc(
		LPVOID lpParameter
	)
	{
		CConsumerThread* Args = (CConsumerThread*)lpParameter;

		Args->Execute();
		
		return (DWORD)0;
	};

	HANDLE Handle;

public:

	CConsumerThread()
	{
		DWORD ThreadId;
		Handle = CreateThread(0, 0, ThreadProc, (LPVOID)this, 0, &ThreadId);
	}

	void Wait()
	{
		if (Handle != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(Handle, INFINITE);
		}
	}

};

int main(char* args)
{	
	CProducerThread P;
//	CProducerThread P1;
//	CProducerThread P3;
	CConsumerThread C;
//	CConsumerThread C1;
//	CConsumerThread C2;


	P.Wait();
	C.Wait();

	printf("OK\r\n");
	return 0;
}
