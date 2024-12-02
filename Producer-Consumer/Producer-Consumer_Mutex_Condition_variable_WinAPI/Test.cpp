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
	Задача производителя и потребителя с помощью условной переменной.
*/


class CCriticalSection
{
	CRITICAL_SECTION Section;

	CCriticalSection(const CCriticalSection&);
	CCriticalSection& operator=(const CCriticalSection&);

	friend class CConditionVariable;
public:

	CCriticalSection()
	{
		::InitializeCriticalSection(&Section);
	};

	~CCriticalSection()
	{
		::DeleteCriticalSection(&Section);
	};

	void Enter()
	{
		::EnterCriticalSection(&Section);
	};

	void Leave()
	{
		::LeaveCriticalSection(&Section);
	};

};

class CConditionVariable
{
	CONDITION_VARIABLE V;
public:

	CConditionVariable()
	{
		InitializeConditionVariable(&V);
	}

	void Sleep(CCriticalSection& CriticalSection)
	{
		SleepConditionVariableCS(&V, &CriticalSection.Section, INFINITE);
	}

	void WakeUp()
	{
		WakeConditionVariable(&V);
	}

	void WakeAll()
	{
		WakeAllConditionVariable(&V);
	}

	~CConditionVariable()
	{
	}
};

template <class T>
class CLock
{
public:
	T& Sec;
	
	explicit CLock(T& ASec) : Sec(ASec)
	{
		Sec.Enter();
	}

	~CLock()
	{
		Sec.Leave();
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

CCriticalSection CriticalSection;
CConditionVariable BufferNotFull;
CConditionVariable BufferNotEmpty;

LONGLONG Start = GetTimeMs();

list<int> Queue;

LONGLONG Processed = 0;
size_t MaxItems = 0;

class CProducerThread
{	
	int Produce()
	{
		return 1;
	}

	void ExecuteOne()
	{
		CLock<CCriticalSection> Lock(CriticalSection);

		while (Queue.size() >= N)
		{
			BufferNotFull.Sleep(CriticalSection);
		}

		Queue.push_back(Produce());

		/*
		For queue length analysis.
		*/
		MaxItems = max(MaxItems, Queue.size());

		//if (Queue.size() == 1)
			BufferNotEmpty.WakeUp();
	}

	void Execute()
	{
		while (true)
		{
			ExecuteOne();
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

	void ExecuteOne()
	{
		CLock<CCriticalSection> Lock(CriticalSection);

		while (Queue.size() == 0)
		{
			BufferNotEmpty.Sleep(CriticalSection);
		}

		MaxItems = max(MaxItems, Queue.size());

		Queue.pop_back();

		CheckProcessed();

		//if (Queue.size() < N)
			BufferNotFull.WakeUp();
	}

	void Execute()
	{
		while (true)
		{
			ExecuteOne();
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

	//Because our test threads are infinite, we, in fact, need only one Wait.
	P.Wait();
	C.Wait();

	printf("OK\r\n");
	return 0;
}
