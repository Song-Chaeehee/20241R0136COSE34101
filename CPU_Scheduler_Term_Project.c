#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
  
#define NUMBER 5 // 프로세스의 개수 정의
#define TIME_QUANTUM 4 // time_quantum을 4로 정의


// 프로세스 구조체 정의  
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

// ready_queue와 waiting_queue 생성 
process *ready_queue[NUMBER];
process *waiting_queue[NUMBER];
// ready_queue와 waiting_queue에 존재하는 프로세스의 개수 정의 
int curr_rq = NUMBER;
int curr_wq = 0;

// 프로세스를 생성하는 함수 정의 
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
		// priority의 경우 0부터 (NUMBER - 1)까지 겹치지 않도록 설정 
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
	
	// 프로세스 하나를 random으로 골라서 arr_time을 0으로 초기화 
	index = rand() % NUMBER;
	p[index]->arr_time = 0;
}

// 모든 작업을 수행한 후 동적 할당한 메모리를 free하는 함수 생성 
void free_process(process *p[])
{
	int i;
	
	for (i = 0; i < NUMBER; i++)
	{
		free(p[i]);
	}
}

// arriving time을 기준으로 프로세스를 정렬하는 함수 정의 
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
			// 만약 arriving time이 같다면 pid가 작은 순서로 정렬 
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

// priority가 작은 순서대로 정렬 
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
			// 만약 priority가 같다면 pid가 작은 순서대로 정렬 
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

// cpu burst time이 적은 순서로 정렬 
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
			// 만약 cpu_burst_time이 같다면 pid가 작은 순서로 정렬 
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

// pid가 작은 순서로 정렬 
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

// ready_queue의 마지막에 프로세스를 insert하는 함수 생성 
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

// waiting_queue의 마지막에 프로세스를 insert하는 함수 생성 
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

// ready_queue에서 특정 프로세스를 delete하는 함수 생성 
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

// waiting_queue 내에서 특정 프로세스를 delete하는 함수 생성 
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

// i/o operation을 수행하는 함수 생성 
void io_operation(process* p[], process* io_process, int time)
{
	if (io_process == NULL)
	{
		printf("Invalid Process Pointer in I/O operation\n");
		return;
	}
	// 특정 프로세스가 이전에 i/o operation을 수행한 적이 없고 cpu burst time을 3만큼 수행한 경우 
	if (io_process->io_done == 0 && io_process->remain_time == io_process->cpu_time - 3)
	{
		delete_rq(p, io_process);
		insert_wq(waiting_queue, io_process);
		 
		io_process->io_start = time;
		io_process->io_done = 1;
	}
	
	// 특정 프로세스가 i/o burst time만큼 i/o operation을 수행했을 때 
	if (io_process->io_done == 1 && time == io_process->io_start + io_process->io_time)
	{
		insert_rq(p, io_process);
		delete_wq(waiting_queue, io_process);
		
		pid_sort(p);
	}
}

// queue내에 특정 프로세스가 존재한다면 1을, 존재하지 않는다면 0을 반환하는 함수 생성  
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

// 모든 프로세스를 초기값으로 되돌리는 함수 생성 
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
	
	// i/o operation을 위해 프로세스들을 새로운 배열에 복사 
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("FCFS Scheduling\n\n");
	
	arr_time_sort(p);
	
	// 현재 시간을 0으로 초기화 
	time = 0;
		
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0이 아닌 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// 만약 이전에 실행하던 프로세스가 끝나지 않고 readyqueue 내에 존재한다면 이어서 실행 
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// 실행할 프로세스가 없을 때 
		if (i == curr_rq)
		{
			// 이전에 실행하던 프로세스를 종료하고 idle_len 1씩 증가 
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len 변수는 다시 초기값으로 초기화 
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
			// idle_len이 양수면서 readyqueue 내에 실행할 프로세스가 존재할 경우 idle 작업 완료 
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
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len 변수는 다시 초기값으로 초기화 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// 남은 시간 1 감소 
		curr->remain_time--;
		
		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time을 계산할 때는 idle time을 제외 
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		
		// 현재 시간 업데이트 
		time++;
		
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행 
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation이 끝난 후 readyqueue를 다시 정렬 
		arr_time_sort(p);
	}
	
	// 마지막으로 실행된 프로세스 처리 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// 평균 waiting time과 turnaround time 계산		
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산 
	cpu_util = (float) (time - total_idle) / time;
	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력 
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

	// 모든 프로세스를 처음 값으로 다시 초기화
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
	
	// priority number가 작은 순서대로 정렬 
	priority_sort(p);
	
	// 현재 시간을 0으로 초기화 
	time = 0;
		
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0이 아닌 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// 만약 이전에 실행하던 프로세스가 끝나지 않고 readyqueue 내에 존재한다면 이어서 실행
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// 실행할 프로세스가 없을 때 
		if (i == curr_rq)
		{
			// 이전에 실행하던 프로세스를 종료하고 idle_len 1씩 증가
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len 변수는 다시 초기값으로 초기화 
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
			// idle_len이 양수면서 readyqueue 내에 실행할 프로세스가 존재할 경우 idle 작업 완료
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
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len 변수는 다시 초기값으로 초기화 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// 남은 시간 1 감소 
		curr->remain_time--;
		
		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time을 계산할 때는 idle time을 제외
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// 현재 시간 업데이트 
		time++;
		
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation이 끝난 후 readyqueue를 다시 정렬
		priority_sort(p);
	}
	
	// 마지막으로 실행된 프로세스 처리 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// 평균 waiting time과 turnaround time 계산
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산
	cpu_util = (float)(time - total_idle) / time;

	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력
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
	
	/// 모든 프로세스를 처음 값으로 다시 초기화
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
	
	// i/o operation을 위해 프로세스들을 새로운 배열에 복사
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Non-preemptive SJF scheduling\n\n");
	
	// cpu busrt time이 작은 순서대로 정렬 
	cpu_time_sort(p);
	
	// 현재 시간을 0으로 초기화 
	time = 0;
		
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0이 아닌 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				curr = p[i];
				// 만약 이전에 실행하던 프로세스가 끝나지 않고 readyqueue 내에 존재한다면 이어서 실행
				if (prev->remain_time > 0 && find_process(p, prev) == 1)
				{
					curr = prev;
				}
				break;
			}
		}
		
		// 실행할 프로세스가 없을 때
		if (i == curr_rq)
		{
			// 이전에 실행하던 프로세스를 종료하고 idle_len 1씩 증가
			if (idle_len == 0)
			{
				index[cnt] = prev->pid;
				length[cnt] = len;
				cnt++;
				// len 변수는 다시 초기값으로 초기화 
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
			// idle_len이 양수면서 readyqueue 내에 실행할 프로세스가 존재할 경우 idle 작업 완료
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
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (prev != curr)
		{
			index[cnt] = prev->pid;
			length[cnt] = len;
			cnt++;
			// len 변수는 다시 초기값으로 초기화 
			len = 1;
			prev = curr;
		}
		else
		{
			len++;
			prev = curr;
		}
		
		// 남은 시간 1 감소 
		curr->remain_time--;
		
		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			// waiting time을 계산할 때는 idle time을 제외
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time - total_idle;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// 현재 시간 업데이트 
		time++;
		
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation이 끝난 후 readyqueue를 다시 정렬
		cpu_time_sort(p);
	}
	
	// 마지막으로 실행된 프로세스 처리 
	index[cnt] = curr->pid;
	length[cnt] = len;
	cnt++;
	
	// 평균 waiting time과 turnaround time 계산
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산
	cpu_util = (float)(time - total_idle) / time;

	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력
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

	// 모든 프로세스를 처음 값으로 다시 초기화
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
	
	// i/o operation을 위해 프로세스들을 새로운 배열에 복사
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Preemptive SJF Scheduling\n\n");
	
	// 현재 시간을 0으로 초기화 
	time = 0;
		
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0보다 큰 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				smallest = p[i];
				break;
			}
		}
		
		// 실행할 프로세스가 없을 때
		if (i == curr_rq)
		{
			// 이전에 실행하던 프로세스를 종료하고 idle_len 1씩 증가
			if (idle_len == 0)
			{
				index[cnt] = prev_smallest->pid;
				length[cnt] = len;
				cnt++;
				// len 변수는 다시 초기값으로 초기화 
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
			// idle_len이 양수면서 readyqueue 내에 실행할 프로세스가 존재할 경우 idle 작업 완료
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
		
		// 처음에 선택한 프로세스보다 remain time이 더 적은 프로세스 선택 
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
		
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (prev_smallest != smallest)
		{
			index[cnt] = prev_smallest->pid;
			length[cnt] = len;
			cnt++;
			// len 변수는 다시 초기값으로 초기화 
			len = 1;
			prev_smallest = smallest;
		}
		else
		{
			len++;
			prev_smallest = smallest;
		}
		
		// 남은 시간 1 감소 
		smallest->remain_time--;
		
		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (smallest->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			smallest->ta_time = total_time - smallest->arr_time;
			// waiting time을 계산할 때는 idle time을 제외
			smallest->wt_time = total_time - smallest->cpu_time - smallest->arr_time - total_idle;
			total_wait += smallest->wt_time;
			total_turn += smallest->ta_time;
		}
		
		// 현재 시간 업데이트 
		time++;
		
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
	}
	
	// 마지막으로 실행된 프로세스 처리 
	index[cnt] = smallest->pid;
	length[cnt] = len;
	cnt++;
	
	// 평균 waiting time과 turnaround time 계산
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산
	cpu_util = (float) (time - total_idle) / time;
	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력
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

	// 모든 프로세스를 처음 값으로 다시 초기화
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
	
	// i/o operation을 위해 프로세스들을 새로운 배열에 복사
	for (i = 0; i < NUMBER; i++)
	{
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Preemptive Priority Scheduling\n\n");
	
	// 현재 시간을 0으로 초기화 
	time = 0;
		
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0이 아닌 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0)
			{
				smallest = p[i];
				break;
			}
		}
		
		// 실행할 프로세스가 없을 때
		if (i == curr_rq)
		{
			// 이전에 실행하던 프로세스를 종료하고 idle_len 1씩 증가
			if (idle_len == 0)
			{
				index[cnt] = prev_smallest->pid;
				length[cnt] = len;
				cnt++;
				// len 변수는 다시 초기값으로 초기화 
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
			// idle_len이 양수면서 readyqueue 내에 실행할 프로세스가 존재할 경우 idle 작업 완료
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
		
		// 처음에 선택한 프로세스보다 remain time이 더 적은 프로세스 선택 
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
		
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (prev_smallest != smallest)
		{
			index[cnt] = prev_smallest->pid;
			length[cnt] = len;
			cnt++;
			// len 변수는 다시 초기값으로 초기화 
			len = 1;
			prev_smallest = smallest;
		}
		else
		{
			len++;
			prev_smallest = smallest;
		}
		
		// 남은 시간 1 감소 
		smallest->remain_time--;
		
		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (smallest->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			smallest->ta_time = total_time - smallest->arr_time;
			// waiting time을 계산할 때는 idle time을 제외
			smallest->wt_time = total_time - smallest->cpu_time - smallest->arr_time - total_idle;
			total_wait += smallest->wt_time;
			total_turn += smallest->ta_time;
		}
		
		// 현재 시간 업데이트 
		time++;
		
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행 
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
	}
	
	// 마지막으로 실행된 프로세스 처리 
	index[cnt] = smallest->pid;
	length[cnt] = len;
	cnt++;
	
	// 평균 waiting time과 turnaround time 계산
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산
	cpu_util = (float)(time - total_idle) / time;
	
	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력
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

	// 모든 프로세스를 처음 값으로 다시 초기화
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
	
	// i/o operation을 위해 프로세스들을 새로운 배열에 복사
	for (i = 0; i < NUMBER; i++)
	{	
		if (p[i] == NULL) return;
		copy_queue[i] = p[i];
	}
		
	printf("Round Robin Scheduling\n\n");
	
	// 도착 시간이 작은 순서대로 정렬 
	arr_time_sort(p);
	
	// 현재 시간을 0으로 초기화 
	time = 0;
	
	
	// 모든 프로세스가 실행을 끝낼 때까지 반복
	while(complete != NUMBER)
	{	
		// i/o operation 함수로 조건에 맞는 프로세스는 i/o operation 수행
		for (i = 0; i < NUMBER; i++)
		{
			io_operation(p, copy_queue[i], time);
		}
		
		// i/o operation이 끝난 후 readyqueue를 다시 정렬
		arr_time_sort(p);
		
		// 이전에 실행하던 프로세스가 readyqueue에 더 이상 존재하지 않는다면 작업을 완료 
		if (find_process(p, curr) == 0 && len > 0)
		{
			index[cnt] = curr->pid;
			length[cnt] = len;
			cnt++;
			curr->done = 1;
			// len 변수는 다시 초기값으로 초기화 
			len = 0;
		}
		
		for (i = 0; i < curr_rq; i++)
		{
			// 도착 시간이 현재 시간보다 작고 remain time이 0보다 큰 첫 번째 프로세스 선택	
			if (p[i]->arr_time <= time && p[i]->remain_time > 0 && p[i]->done == 0)
			{
				curr = p[i];
				break;
			}
		}
				
		// readyqueue 내 모든 프로세스를 한번씩 수행했을 때 
		if (i == curr_rq)
		{
			for (j = 0; j < NUMBER; j++)
			{
				p[j]->done = 0;
			}
			// 다시 조건 탐색		
			for (j = 0; j < curr_rq; j++)
			{
				if (p[j]->arr_time <= time && p[j]->remain_time > 0 && p[j]->done == 0)
				{
					curr = p[j];
					break;
				}
			}
		}
		
		// 남은 시간 1 감소 
		curr->remain_time--;
		len++;
		
		// 이전에 선택한 프로세스와 다른 프로세스라면 index와 length 배열에 pid와 실행 시간을 입력 
		if (len == TIME_QUANTUM || curr->remain_time == 0)
		{
			index[cnt] = curr->pid;
			length[cnt] = len;
			cnt++;
			curr->done = 1;
			// len 변수는 다시 초기값으로 초기화 
			len = 0;
		}

		// 만약 프로세스의 실행이 끝났다면 complete 1 증가 
		if (curr->remain_time == 0)
		{
			complete++;
			total_time = time + 1;
			curr->ta_time = total_time - curr->arr_time;
			curr->wt_time = total_time - curr->cpu_time - curr->arr_time;
			total_wait += curr->wt_time;
			total_turn += curr->ta_time;
		}
		
		// 현재 시간 업데이트 
		time++;
	}
	
	// 평균 waiting time과 turnaround time 계산
	average_wait = (float)total_wait / NUMBER;
	average_turn = (float)total_turn / NUMBER;
	// CPU utilization 계산
	cpu_util = (float)(time - total_idle) / time;

	
	// 간트 차트 출력 
	printf("Gantt Chart\n");
	
	// 프로세스가 실행되는 간격을 출력할 비율을 총 실행 시간을 사용하여 정의
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
		// idle 상태인 경우 XX 출력
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

	// 모든 프로세스를 처음 값으로 다시 초기화
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
