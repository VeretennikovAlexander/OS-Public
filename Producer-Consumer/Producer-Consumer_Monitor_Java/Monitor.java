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

import java.util.List;
import java.util.LinkedList;
import java.util.Date;
import java.text.MessageFormat;

/**
    Задача производителя и потребителя на Java с помощью Монитора.
*/

class Test
{
	private static class Monitor
	{
		private int capacity = 1000;

		private List<Integer> queue = new LinkedList<>();

		private long processed = 0;

		private int max = 0;

		private Date start = new Date();
        
        private void checkProcessed()
        {
            processed++;

			if ((processed % 100000) == 0)
			{
                long delta = new Date().getTime() - start.getTime();
                long perSecond = (1000 * processed) / delta;
				System.out.println(MessageFormat.format("Processed {0}, max {1}, current {2}, time {3} ms, per second {4}",
					processed,max,queue.size(),delta,perSecond));
			}
        }

		void updateMax()
		{
			if (max < queue.size())
			{
				max = queue.size();
			}
		}
	
		public synchronized void push(int Value)
		{
			while (queue.size() >= capacity)
			{
				sleep();
			}
			
			queue.add(Value);

			updateMax();

			//if (Queue.size() == 1)
			{
				notify();
			}
		}
        
		public synchronized int pop()
		{
			while (queue.size() == 0)
			{
				sleep();
			}

			updateMax();

			int result = queue.get(0);
			queue.remove(0);

			checkProcessed();

			//if (queue.size() == (capacity-1))
			{
				notify();
			}
			return result;
		}

		private void sleep()
		{
			try
			{
				wait();
			}
			catch(Throwable tr)
			{
			}
		}
	}

	private Monitor monitor = new Monitor();
	
	private class Producer extends Thread
	{
		@Override
		public void run()
		{
            System.out.println("Start producer");
            
			while(true)
			{
				monitor.push(1);
			}
		}
	};

	private class Consumer extends Thread
	{
		@Override
		public void run()
		{
            System.out.println("Start consumer");
            
			while(true)
			{
				monitor.pop();
			}
		}
	};

	private void test()
	{
		new Producer().start();
		new Consumer().start();
//        new Consumer().start();
	}

	public static void main(String[] args)
	{
		new Test().test();
	}

}
