#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
  
#define NUMBER 5 // ���μ����� ���� ����
#define TIME_QUANTUM 4 // time_quantum�� 4�� ����


// ���μ��� ����ü ����  
typedef struct
{
	int pid;
	int priority;
	int arr_time;
	int cpu_time;
	int io_time;
	int ta_time;
	int wt_time;
	int remain_time;
	int done;
	int io_done;
	int io_start;
}process; 

// ready_queue�� waiting_queue ���� 
process *ready_queue[NUMBER];
process *waiting_queue[NUMBER];
// ready_queue�� waiting_queue�� �����ϴ� ���μ����� ���� ���� 
int curr_rq = NUMBER;
int curr_wq = 0;

// ���μ����� �����ϴ� �Լ� ���� 
int create_process(process *p[])
{
	int pid, priority, arr_time, cpu_time, io_time, wt_time, ta_time, remain_time;
	int i, k, index, flag;
	
	srand(time(NULL));
	
	for (i = 0; i < NUMBER; i++)
	{	
		scanf("%d", &pid);
		cpu_time = (rand() % 10) + 5;
		arr_time = rand() % 3 + 1;
		io_time = rand() % 2 + 1;
		wt_time = 0;
		ta_time = 0;
		// priority�� ��� 0���� (NUMBER - 1)���� ��ġ�� �ʵ��� ���� 
		while (1)
		{
			priority = rand() % NUMBER;
			flag = 0;
			for (k = 0; k < i; k++)
			{
				if (priority == p[k]->priority)
				{
					flag = 1;
					break;
				}
			}
			if (!flag) break;
		}
		
		p[i] = malloc(sizeof(process));
		
		if (p[i] == NULL)
		{
			printf("Error\n");
			return -1;
		}
		
		p[i]->pid = pid;
		p[i]->priority = priority;
		p[i]->arr_time = arr_time;
		p[i]->cpu_time = cpu_time;
		p[i]->io_time = io_time;
		p[i]->wt_time = 0;
		p[i]->ta_time = 0;
		p[i]->remain_time = p[i]->cpu_time;
		p[i]->done = 0;
		p[i]->io_done = 0;
		p[i]->io_start = 0;
	}
	
	// ���μ��� �ϳ��� random���� ��� arr_time�� 0���� �ʱ�ȭ 
	index = rand() % NUMBER;
	p[index]->arr_time = 0;
}

// ��� �۾��� ������ �� ���� �Ҵ��� �޸𸮸� free�ϴ� �Լ� ���� 
void free_process(process *p[])
{
	int i;
	
	for (i = 0; i < NUMBER; i++)
	{
		free(p[i]);
	}
}

// arriving time�� �������� ���μ����� �����ϴ� �Լ� ���� 
void arr_time_sort(process *p[])
{
	int i, j;
	process *temp;
	for (i = 1; i < NUMBER; i++)
	{
		for (j = 0; j < NUMBER - 1; j++)
		{
			if (p[j]->arr_time > p[j + 1]->arr_time)
			{
				temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
			// ���� arriving time�� ���ٸ� pid�� ���� ������ ���� 
			else if (p[j]->arr_time == p[j + 1]->arr_time)
			{
				if (p[j]->pid > p[j + 1]->pid)
				{
					temp = p[j];
					p[j] = p[j + 1];
					p[j + 1] = temp;
				}
			}
		}
	}
}

// priority�� ���� ������� ���� 
void priority_sort(process *p[])
{
	int i, j;
	process *temp;
	for (i = 1; i < NUMBER; i++)
	{
		for (j = 0; j < NUMBER - 1; j++)
		{
			if (p[j]->priority > p[j + 1]->priority)
			{
				temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
			// ���� priority�� ���ٸ� pid�� ���� ������� ���� 
			else if (p[j]->priority == p[j + 1]->priority)
			{
				if (p[j]->pid > p[j + 1]->pid)
				{
					temp = p[j];
					p[j] = p[j + 1];
					p[j + 1] = temp;
				}
			}
		}
	}
}

// cpu burst time�� ���� ������ ���� 
void cpu_time_sort(process *p[])
{
	int i, j;
	process *temp;
	for (i = 1; i < NUMBER; i++)
	{
		for (j = 0; j < NUMBER - 1; j++)
		{
			if (p[j]->cpu_time > p[j + 1]->cpu_time)
			{
				temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
			// ���� cpu_burst_time�� ���ٸ� pid�� ���� ������ ���� 
			else if (p[j]->cpu_time == p[j + 1]->cpu_time)
			{
				if (p[j]->pid > p[j + 1]->pid)
				{
					temp = p[j];
					p[j] = p[j + 1];
					p[j + 1] = temp;
				}
			}
		}
	}
}

// pid�� ���� ������ ���� 
void pid_sort(process *p[])
{
	int i, j;
	process *temp;
	for (i = 1; i < NUMBER; i++)
	{
		for (j = 0; j < NUMBER - 1; j++)
		{
			if (p[j]->pid > p[j + 1]->pid)
			{
				temp = p[j];
				p[j] = p[j + 1];
				p[j + 1] = temp;
			}
		}
	}
}

// ready_queue�� �������� ���μ����� insert�ϴ� �Լ� ���� 
void insert_rq(process* p[], process* process)
{
	if (curr_rq >= NUMBER)
	{
		printf("Ready Queue is full\n");
		return;
	}
	p[curr_rq] = process;
	curr_rq++;
}

// waiting_queue�� �������� ���μ����� insert�ϴ� �Լ� ���� 
void insert_wq(process* p[], process* process)
{
	if (curr_wq >= NUMBER)
	{
		printf("Waiting Queue is full\n");
		return;
	}
	p[curr_wq] = process;
	curr_wq++;
}

// ready_queue���� Ư�� ���μ����� delete�ϴ� �Լ� ���� 
void delete_rq(process* p[], process* process)
{
	int i, j, index;
	
	for (i = 0; i < curr_rq; i++)
	{
		if (p[i] == process)
		{
			index = i;
			break;
		}
	}
	
	if (i == curr_rq)
	{
		printf("Process is not found in Ready Queue\n");
		return;
	}
	
	j = index;
	
	while (j < curr_rq - 1)
	{
		p[j] = p[j + 1];
		j++;
	}
	curr_rq--;
}

// waiting_queue ������ Ư�� ���μ����� delete�ϴ� �Լ� ���� 
void delete_wq(process *p[], process* process)
{
	int i, j, index;
	
	for (i = 0; i < curr_wq; i++)
	{
		if (p[i] == process)
		{
			index = i;
			break;
		}
	}
	
	if (i == curr_wq)
	{
		printf("Process is not found in Waiting Queue\n");
		return;
	}
	
	j = index;
	
	while (j < curr_wq - 1)
	{
		p[j] = p[j + 1];
		j++;
	}
	curr_wq--;
}

void print_line(int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		printf("-");
	}
}

// i/o operation�� �����ϴ� �Լ� ���� 
void io_operation(process* p[], process* io_process, int time)
{
	if (io_process == NULL)
	{
		printf("Invalid Process Pointer in I/O operation\n");
		return;
	}
	// Ư�� ���μ����� ������ i/o operation�� ������ ���� ���� cpu burst time�� 3��ŭ ������ ��� 
	if (io_process->io_done == 0 && io_process->remain_time == io_process->cpu_time - 3)
	{
		delete_rq(p, io_process);
		insert_wq(waiting_queue, io_process);
		 
		io_process->io_start = time;
		io_process->io_done = 1;
	}
	
	// Ư�� ���μ����� i/o burst time��ŭ i/o operation�� �������� �� 
	if (io_process->io_done == 1 && time == io_process->io_start + io_process->io_time)
	{
		insert_rq(p, io_process);
		delete_wq(waiting_queue, io_process);
		
		pid_sort(p);
	}
}

// queue���� Ư�� ���μ����� �����Ѵٸ� 1��, �������� �ʴ´ٸ� 0�� ��ȯ�ϴ� �Լ� ����  
int find_process(process *p[], process *target)
{
	int i;
	
	if (target == NULL)
	{
		printf("Invalid Process Pointer in find_process\n");
		return -1;
	}
	
	for (i = 0; i < curr_rq; i++)
	{
		if (p[i] == target) return 1;
	}
	
	return 0;
}

// ��� ���μ����� �ʱⰪ���� �ǵ����� �Լ� ���� 
void initialize_process(process *p[])
{
	int i;
	
	for (i = 0; i < NUMBER; i++)
	{
		p[i]->remain_time = p[i]->cpu_time;
		p[i]->wt_time = 0;
		p[i]->ta_time = 0;
		p[i]->done = 0;
		p[i]->io_done = 0;
		p[i]->io_start = 0;
	}
	
	pid_sort(p);
}

void fcfs(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 1, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *curr, *prev = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	// i/o operation�� ���� ���μ������� ���ο� �迭�� ���� 
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("FCFS Scheduling\n\n");
	
	arr_time_sort(p);
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
		
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0�� �ƴ� ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// ���� ������ �����ϴ� ���μ����� ������ �ʰ� readyqueue ���� �����Ѵٸ� �̾ ���� 
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// ������ ���μ����� ���� �� 
		if (i == curr_rq)
		{
			// ������ �����ϴ� ���μ����� �����ϰ� idle_len 1�� ���� 
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
				len = 1;
				prev = curr;
				curr->remain_time--;
			}
			
			idle_len++;
			curr->remain_time++;
			len--;
		}
		else
		{
			// idle_len�� ����鼭 readyqueue ���� ������ ���μ����� ������ ��� idle �۾� �Ϸ� 
			if (idle_len > 0)
			{
				index[cnt] = 0;
				length[cnt] = idle_len;
				total_idle += idle_len;
				cnt++;
				idle_len = 0;
				time++;
			}
		}
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// ���� �ð� 1 ���� 
		curr->remain_time--;
		
		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time�� ����� ���� idle time�� ���� 
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		
		// ���� �ð� ������Ʈ 
		time++;
		
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ���� 
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation�� ���� �� readyqueue�� �ٽ� ���� 
		arr_time_sort(p);
	}
	
	// ���������� ����� ���μ��� ó�� 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// ��� waiting time�� turnaround time ���		
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ��� 
	cpu_util = (float) (time - total_idle) / time;
	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 1; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ��� 
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 1; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);

	// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void np_priority(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 1, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *curr, *prev = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	//
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Non-Preemptive Priority Scheduling\n\n");
	
	// priority number�� ���� ������� ���� 
	priority_sort(p);
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
		
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0�� �ƴ� ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// ���� ������ �����ϴ� ���μ����� ������ �ʰ� readyqueue ���� �����Ѵٸ� �̾ ����
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// ������ ���μ����� ���� �� 
		if (i == curr_rq)
		{
			// ������ �����ϴ� ���μ����� �����ϰ� idle_len 1�� ����
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
				len = 1;
				prev = curr;
				curr->remain_time--;
			}
			
			idle_len++;
			curr->remain_time++;
			len--;
		}
		else
		{
			// idle_len�� ����鼭 readyqueue ���� ������ ���μ����� ������ ��� idle �۾� �Ϸ�
			if (idle_len > 0)
			{
				index[cnt] = 0;
				length[cnt] = idle_len;
				total_idle += idle_len;
				cnt++;
				idle_len = 0;
				time++;
			}
		}
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// ���� �ð� 1 ���� 
		curr->remain_time--;
		
		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time�� ����� ���� idle time�� ����
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// ���� �ð� ������Ʈ 
		time++;
		
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ����
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation�� ���� �� readyqueue�� �ٽ� ����
		priority_sort(p);
	}
	
	// ���������� ����� ���μ��� ó�� 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// ��� waiting time�� turnaround time ���
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ���
	cpu_util = (float)(time - total_idle) / time;

	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 1; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ���
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 1; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);
	
	/// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void np_sjf(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 1, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *curr, *prev = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	// i/o operation�� ���� ���μ������� ���ο� �迭�� ����
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Non-preemptive SJF scheduling\n\n");
	
	// cpu busrt time�� ���� ������� ���� 
	cpu_time_sort(p);
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
		
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0�� �ƴ� ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// ���� ������ �����ϴ� ���μ����� ������ �ʰ� readyqueue ���� �����Ѵٸ� �̾ ����
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// ������ ���μ����� ���� ��
		if (i == curr_rq)
		{
			// ������ �����ϴ� ���μ����� �����ϰ� idle_len 1�� ����
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
				len = 1;
				prev = curr;
				curr->remain_time--;
			}
			
			idle_len++;
			curr->remain_time++;
			len--;
		}
		else
		{
			// idle_len�� ����鼭 readyqueue ���� ������ ���μ����� ������ ��� idle �۾� �Ϸ�
			if (idle_len > 0)
			{
				index[cnt] = 0;
				length[cnt] = idle_len;
				total_idle += idle_len;
				cnt++;
				idle_len = 0;
				time++;
			}
		}
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// ���� �ð� 1 ���� 
		curr->remain_time--;
		
		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time�� ����� ���� idle time�� ����
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// ���� �ð� ������Ʈ 
		time++;
		
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ����
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation�� ���� �� readyqueue�� �ٽ� ����
		cpu_time_sort(p);
	}
	
	// ���������� ����� ���μ��� ó�� 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// ��� waiting time�� turnaround time ���
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ���
	cpu_util = (float)(time - total_idle) / time;

	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 1; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ���
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 1; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);

	// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void pr_sjf(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 1, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *smallest, *prev_smallest = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	// i/o operation�� ���� ���μ������� ���ο� �迭�� ����
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Preemptive SJF Scheduling\n\n");
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
		
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0���� ū ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				smallest = p[i];
				break;
			}
		}
		
		// ������ ���μ����� ���� ��
		if (i == curr_rq)
		{
			// ������ �����ϴ� ���μ����� �����ϰ� idle_len 1�� ����
			if (idle_len == 0)
			{
				index[cnt] = prev_smallest->pid;
				length[cnt] = len;
				cnt++;
				// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
				len = 1;
				prev_smallest = smallest;
				smallest->remain_time--;
			}
			
			idle_len++;
			smallest->remain_time++;
			len--;
		}
		else
		{
			// idle_len�� ����鼭 readyqueue ���� ������ ���μ����� ������ ��� idle �۾� �Ϸ�
			if (idle_len > 0)
			{
				index[cnt] = 0;
				length[cnt] = idle_len;
				total_idle += idle_len;
				cnt++;
				idle_len = 0;
				time++;
			}
		}
		
		// ó���� ������ ���μ������� remain time�� �� ���� ���μ��� ���� 
		for (i = 0; i < curr_rq; i++)
		{
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				if (smallest->remain_time > p[i]->remain_time)
				{
					smallest = p[i];
				}
			}
		}
		
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (prev_smallest != smallest)
		{
			index[cnt] = prev_smallest->pid;
			length[cnt] = len;
			cnt++;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 1;
			prev_smallest = smallest;
		}
		else
		{
			len++;
			prev_smallest = smallest;
		}
		
		// ���� �ð� 1 ���� 
		smallest->remain_time--;
		
		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (smallest->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			smallest->ta_time = total_time - smallest->arr_time;
			// waiting time�� ����� ���� idle time�� ����
			smallest->wt_time = total_time - smallest->cpu_time - smallest->arr_time - total_idle;
			total_wait += smallest->wt_time;
			total_turn += smallest->ta_time;
		}
		
		// ���� �ð� ������Ʈ 
		time++;
		
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ����
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
	}
	
	// ���������� ����� ���μ��� ó�� 
	index[cnt] = smallest->pid;
	length[cnt] = len;
	cnt++;
	
	// ��� waiting time�� turnaround time ���
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ���
	cpu_util = (float) (time - total_idle) / time;
	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 1; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ���
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 1; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);

	// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void pr_priority(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 1, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *smallest, *prev_smallest = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	// i/o operation�� ���� ���μ������� ���ο� �迭�� ����
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Preemptive Priority Scheduling\n\n");
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
		
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0�� �ƴ� ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				smallest = p[i];
				break;
			}
		}
		
		// ������ ���μ����� ���� ��
		if (i == curr_rq)
		{
			// ������ �����ϴ� ���μ����� �����ϰ� idle_len 1�� ����
			if (idle_len == 0)
			{
				index[cnt] = prev_smallest->pid;
				length[cnt] = len;
				cnt++;
				// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
				len = 1;
				prev_smallest = smallest;
				smallest->remain_time--;
			}
			
			idle_len++;
			smallest->remain_time++;
			len--;
		}
		else
		{
			// idle_len�� ����鼭 readyqueue ���� ������ ���μ����� ������ ��� idle �۾� �Ϸ�
			if (idle_len > 0)
			{
				index[cnt] = 0;
				length[cnt] = idle_len;
				total_idle += idle_len;
				cnt++;
				idle_len = 0;
				time++;
			}
		}
		
		// ó���� ������ ���μ������� remain time�� �� ���� ���μ��� ���� 
		for (i = 0; i < curr_rq; i++)
		{
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				if (smallest->priority > p[i]->priority)
				{
					smallest = p[i];
				}
			}
		}
		
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (prev_smallest != smallest)
		{
			index[cnt] = prev_smallest->pid;
			length[cnt] = len;
			cnt++;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 1;
			prev_smallest = smallest;
		}
		else
		{
			len++;
			prev_smallest = smallest;
		}
		
		// ���� �ð� 1 ���� 
		smallest->remain_time--;
		
		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (smallest->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			smallest->ta_time = total_time - smallest->arr_time;
			// waiting time�� ����� ���� idle time�� ����
			smallest->wt_time = total_time - smallest->cpu_time - smallest->arr_time - total_idle;
			total_wait += smallest->wt_time;
			total_turn += smallest->ta_time;
		}
		
		// ���� �ð� ������Ʈ 
		time++;
		
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ���� 
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
	}
	
	// ���������� ����� ���μ��� ó�� 
	index[cnt] = smallest->pid;
	length[cnt] = len;
	cnt++;
	
	// ��� waiting time�� turnaround time ���
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ���
	cpu_util = (float)(time - total_idle) / time;
	
	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 1; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ���
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 1; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);

	// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void round_robin(process *p[])
{
	int i = 0, j = 0, k = 0, tt = 0, total_turn = 0, total_wait = 0, total_time = 0, total_idle = 0;
	int complete = 0, time = 0, len = 0, cnt = 0;
	int index[50] = {0}, length[50] = {0};
	float average_wait = 0.0, average_turn = 0.0, r = 0.0, cpu_util = 0.0;
	process basic_process = {0};
	process *curr = &basic_process;	
	int idle_len = 0;
	process *copy_queue[NUMBER];
	
	// i/o operation�� ���� ���μ������� ���ο� �迭�� ����
	for (i = 0; i < NUMBER; i++)
	{	
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Round Robin Scheduling\n\n");
	
	// ���� �ð��� ���� ������� ���� 
	arr_time_sort(p);
	
	// ���� �ð��� 0���� �ʱ�ȭ 
	time = 0;
	
	
	// ��� ���μ����� ������ ���� ������ �ݺ�
	while(complete != NUMBER)
	{	
		// i/o operation �Լ��� ���ǿ� �´� ���μ����� i/o operation ����
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation�� ���� �� readyqueue�� �ٽ� ����
		arr_time_sort(p);
		
		// ������ �����ϴ� ���μ����� readyqueue�� �� �̻� �������� �ʴ´ٸ� �۾��� �Ϸ� 
		if (find_process(p, curr) == 0 && len > 0)
		{
			index[cnt] = curr->pid;
			length[cnt] = len;
			cnt++;
			curr->done = 1;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 0;
		}
		
		for (i = 0; i < curr_rq; i++)
		{
			// ���� �ð��� ���� �ð����� �۰� remain time�� 0���� ū ù ��° ���μ��� ����	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0 && p[i]->done == 0)
			{
				curr = p[i];
				break;
			}
		}
				
		// readyqueue �� ��� ���μ����� �ѹ��� �������� �� 
		if (i == curr_rq)
		{
			for (j = 0; j < NUMBER; j++)
			{
				p[j]->done = 0;
			}
			// �ٽ� ���� Ž��		
			for (j = 0; j < curr_rq; j++)
			{
				if (p[j]->arr_time <= time && p[j]->remain_time > 0 && p[j]->done == 0)
				{
					curr = p[j];
					break;
				}
			}
		}
		
		// ���� �ð� 1 ���� 
		curr->remain_time--;
		len++;
		
		// ������ ������ ���μ����� �ٸ� ���μ������ index�� length �迭�� pid�� ���� �ð��� �Է� 
		if (len == TIME_QUANTUM || curr->remain_time == 0)
		{
			index[cnt] = curr->pid;
			length[cnt] = len;
			cnt++;
			curr->done = 1;
			// len ������ �ٽ� �ʱⰪ���� �ʱ�ȭ 
			len = 0;
		}

		// ���� ���μ����� ������ �����ٸ� complete 1 ���� 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// ���� �ð� ������Ʈ 
		time++;
	}
	
	// ��� waiting time�� turnaround time ���
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization ���
	cpu_util = (float)(time - total_idle) / time;

	
	// ��Ʈ ��Ʈ ��� 
	printf("Gantt Chart\n");
	
	// ���μ����� ����Ǵ� ������ ����� ������ �� ���� �ð��� ����Ͽ� ����
	r = 70.0 / total_time;

	print_line(115);
	printf("\n|");
	for (i = 0; i < cnt; i++)
	{
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		// idle ������ ��� XX ���
		if (index[i] == 0) printf("XX");
		else printf("P%d", index[i]);
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("|");
	}
	printf("\n");
	print_line(115);
	printf("\n0");
	
	tt = 0;
	for (i = 0; i < cnt; i++)
	{
		tt = tt + length[i];
		k = length[i] * r;
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		for (j = 0; j < k/2; j++)
		{
			printf(" ");
		}
		printf("%3d", tt);
	}
	printf("\n");
	
	printf("Average Waiting Time: %.2f / Average Turnaround Time: %.2f\n", average_wait, average_turn);
	printf("CPU Utilization : %.2f %%\n\n\n", cpu_util * 100);

	// ��� ���μ����� ó�� ������ �ٽ� �ʱ�ȭ
	initialize_process(p);
}

void print_process_table(process *p[])
{
	int i;
	printf("Process Table\n");
	printf("---------------------------------------------------------------------------------\n");
	printf("|	PID	|    priority   | arriving time | cpu burst time| i/o burst time|\n");
	printf("---------------------------------------------------------------------------------\n");
	for (i = 0; i < NUMBER; i++)
	{
		printf("|	P%d	|	%d	|	%d	|	%d	|	%d	|\n", p[i]->pid, p[i]->priority, p[i]->arr_time, p[i]->cpu_time, p[i]->io_time);
		printf("---------------------------------------------------------------------------------\n");
	}
}


int main(void)
{
	create_process(ready_queue);
	print_process_table(ready_queue);

	fcfs(ready_queue);
	
	np_sjf(ready_queue);
	
	pr_sjf(ready_queue);
	
	np_priority(ready_queue);
	
	pr_priority(ready_queue);
	
	round_robin(ready_queue);
	
	free_process(ready_queue);
		
	return 0;
}
