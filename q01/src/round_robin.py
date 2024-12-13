# Algoritmo Round Robin
def round_robin(processes, burst_time, quantum, interval, context_switch_time = 1):
    n = len(processes)
    remaning_burst_time = burst_time[:]
    completed_processes = 0
    current_time = 0
    waiting_time = [0] * n
    turnaround_time = [0] * n
    throughput = 0


    while completed_processes < n:
        
        for process in range(n):
            if remaning_burst_time[process] > 0:

                time_slice = min(quantum, remaning_burst_time[process])
                current_time += time_slice
                remaning_burst_time[process] -= time_slice

                if remaning_burst_time[process] == 0:
                    completed_processes += 1
                    turnaround_time[process] = current_time
                    waiting_time[process] = current_time - burst_time[process]
                
                if completed_processes < n:
                    current_time += context_switch_time


    turnaround_time_sorted = sorted(turnaround_time)
    while(throughput < n and interval >= turnaround_time_sorted[throughput]):
        throughput += 1

    average_waiting_time = sum(waiting_time) / n
    average_turnaround_time = sum(turnaround_time) / n

    return average_turnaround_time, average_waiting_time, throughput
