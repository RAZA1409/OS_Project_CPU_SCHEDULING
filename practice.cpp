#include <bits/stdc++.h>
using namespace std;

// ---------------------------------------------------------
// STRUCT FOR A PROCESS
// ---------------------------------------------------------
struct Process {
    int pid, at, bt, ct, tat, wt, priority;
    int remaining;
    bool completed;

    Process(int _pid=0, int _at=0, int _bt=0, int _pr=0) {
        pid = _pid;
        at = _at;
        bt = _bt;
        priority = _pr;
        ct = tat = wt = 0;
        completed = false;
        remaining = bt;
    }
};

// ---------------------------------------------------------
// MEMBER 1 — INPUT
// ---------------------------------------------------------
vector<Process> inputProcesses() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> pList;
    pList.reserve(n);

    cout << "\nEnter (PID ArrivalTime BurstTime Priority):\n";
    for(int i=0; i<n; i++) {
        int pid, at, bt, pr;
        cin >> pid >> at >> bt >> pr;

        if(at < 0 || bt <= 0) {
            cout << "Invalid input! Arrival >= 0, Burst > 0.\n";
            i--;
            continue;
        }

        pList.emplace_back(pid, at, bt, pr);
    }
    return pList;
}

// ---------------------------------------------------------
// FCFS Scheduling (your existing code)
// ---------------------------------------------------------
void fcfsScheduling(
    vector<Process>& p,
    vector<int>& execOrder,
    vector<pair<int,int>>& timeline,
    double& avgWT,
    double& avgTAT,
    int& idleTime,
    int& totalTime
){
    sort(p.begin(), p.end(), [](auto &a, auto &b){
        return a.at < b.at;
    });

    int n = p.size();
    int time = 0;
    idleTime = 0;

    for(int i=0; i<n; i++){
        if(time < p[i].at){
            idleTime += (p[i].at - time);
            time = p[i].at;
        }

        int start = time;
        int finish = time + p[i].bt;

        p[i].ct = finish;
        p[i].tat = p[i].ct - p[i].at;
        p[i].wt = p[i].tat - p[i].bt;

        execOrder.push_back(p[i].pid);
        timeline.push_back({start, finish});

        time = finish;
    }

    totalTime = time;

    long long totalWT = 0, totalTAT = 0;
    for(auto &x : p){
        totalWT += x.wt;
        totalTAT += x.tat;
    }

    avgWT = (double)totalWT / n;
    avgTAT = (double)totalTAT / n;
}

// ---------------------------------------------------------
// SJF NON-PREEMPTIVE (your existing code)
// ---------------------------------------------------------
void sjfNonPreemptive(
    vector<Process>& p,
    vector<int>& execOrder,
    vector<pair<int,int>>& timeline,
    double& avgWT,
    double& avgTAT,
    int& idleTime,
    int& totalTime
){
    int n = p.size();
    int completed = 0;
    int time = 0;
    idleTime = 0;

    while(completed < n) {
        int idx = -1;
        int minBT = INT_MAX;

        for(int i=0; i<n; i++){
            if(!p[i].completed && p[i].at <= time && p[i].bt < minBT){
                minBT = p[i].bt;
                idx = i;
            }
        }

        if(idx == -1){
            time++;
            idleTime++;
            continue;
        }

        int start = time;
        int finish = time + p[idx].bt;

        p[idx].ct = finish;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].completed = true;

        execOrder.push_back(p[idx].pid);
        timeline.push_back({start, finish});

        time = finish;
        completed++;
    }

    totalTime = time;

    long long totalWT = 0, totalTAT = 0;
    for(auto &x : p){
        totalWT += x.wt;
        totalTAT += x.tat;
    }

    avgWT = (double)totalWT / n;
    avgTAT = (double)totalTAT / n;
}

// ---------------------------------------------------------
//  NEW FUNCTION 1 — ROUND ROBIN
// ---------------------------------------------------------
void roundRobin(
    vector<Process>& p,
    vector<int>& execOrder,
    vector<pair<int,int>>& timeline,
    double& avgWT,
    double& avgTAT,
    int& idleTime,
    int& totalTime,
    int quantum
){
    int n = p.size();
    queue<int> q;
    idleTime = 0;
    int time = 0;
    int completed = 0;

    // Push all initially arriving processes
    while(true) {
        bool pushed = false;
        for(int i=0; i<n; i++) {
            if(p[i].at == time && p[i].remaining == p[i].bt) {
                q.push(i);
                pushed = true;
            }
        }
        if(pushed || !q.empty()) break;
        time++;
        idleTime++;
    }

    while(completed < n) {
        if(q.empty()) {
            time++;
            idleTime++;
            for(int i=0;i<n;i++){
                if(p[i].at == time)
                    q.push(i);
            }
            continue;
        }

        int idx = q.front(); q.pop();

        int start = time;
        int run = min(quantum, p[idx].remaining);
        time += run;
        p[idx].remaining -= run;
        int finish = time;

        execOrder.push_back(p[idx].pid);
        timeline.push_back({start, finish});

        for(int i=0;i<n;i++){
            if(p[i].at > start && p[i].at <= time)
                q.push(i);
        }

        if(p[idx].remaining > 0)
            q.push(idx);
        else {
            completed++;
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
        }
    }

    totalTime = time;

    long long TWT = 0, TTAT = 0;
    for(auto &x : p){
        TWT += x.wt;
        TTAT += x.tat;
    }

    avgWT = (double)TWT / n;
    avgTAT = (double)TTAT / n;
}

// ---------------------------------------------------------
//  NEW FUNCTION 2 — PRIORITY NON-PREEMPTIVE
// ---------------------------------------------------------
void priorityScheduling(
    vector<Process>& p,
    vector<int>& execOrder,
    vector<pair<int,int>>& timeline,
    double& avgWT,
    double& avgTAT,
    int& idleTime,
    int& totalTime
){
    int n = p.size();
    int time = 0;
    int completed = 0;
    idleTime = 0;

    while(completed < n) {
        int idx = -1;
        int bestPr = INT_MAX;

        for(int i=0; i<n; i++){
            if(!p[i].completed && p[i].at <= time){
                if(p[i].priority < bestPr){
                    bestPr = p[i].priority;
                    idx = i;
                }
            }
        }

        if(idx == -1) {
            time++;
            idleTime++;
            continue;
        }

        int start = time;
        int finish = time + p[idx].bt;

        p[idx].ct = finish;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        p[idx].completed = true;

        execOrder.push_back(p[idx].pid);
        timeline.push_back({start, finish});

        time = finish;
        completed++;
    }

    totalTime = time;
    long long TWT = 0, TTAT = 0;

    for(auto &x : p){
        TWT += x.wt;
        TTAT += x.tat;
    }
    avgWT = (double)TWT / n;
    avgTAT = (double)TTAT / n;
}

// ---------------------------------------------------------
// GANTT CHART (your existing code)
// ---------------------------------------------------------
void generateGanttChart(const vector<int>& execOrder,
                        const vector<pair<int,int>>& timeline) {
    cout << "\n\n---------- GANTT CHART ----------\n";

    for(int i=0; i<execOrder.size(); i++) {
        cout << "+-------";
    }
    cout << "+\n";

    for(int i=0; i<execOrder.size(); i++) {
        cout << "|  P" << execOrder[i] << "  ";
    }
    cout << "|\n";

    for(int i=0; i<execOrder.size(); i++) {
        cout << "+-------";
    }
    cout << "+\n";

    for(int i=0; i<timeline.size(); i++){
        cout << timeline[i].first << "\t";
    }
    cout << timeline.back().second << "\n";
}

// ---------------------------------------------------------
// DISPLAY RESULTS
// ---------------------------------------------------------
void displayResults(
    const vector<Process>& p,
    double avgWT,
    double avgTAT,
    int idleTime,
    int totalTime
){
    cout << "\n\n========== FINAL TABLE ==========\n";
    cout << "PID\tAT\tBT\tPR\tCT\tTAT\tWT\n";

    for(const auto &x : p){
        cout << x.pid << "\t"
             << x.at << "\t"
             << x.bt << "\t"
             << x.priority << "\t"
             << x.ct << "\t"
             << x.tat << "\t"
             << x.wt << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Waiting Time: " << avgWT;
    cout << "\nAverage Turnaround Time: " << avgTAT;

    double cpuUtil = ((totalTime - idleTime) / (double)totalTime) * 100;
    double throughput = p.size() / (double)totalTime;

    cout << "\nCPU Utilization: " << cpuUtil << "%";
    cout << "\nThroughput: " << throughput << " processes/unit time";
    cout << "\nCPU Idle Time: " << idleTime << " units\n";
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
int main() {

    vector<Process> pList = inputProcesses();

    cout << "\nChoose Scheduling Algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SJF Non-Preemptive\n";
    cout << "3. Round Robin\n";
    cout << "4. Priority Scheduling\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    vector<int> execOrder;
    vector<pair<int,int>> timeline;
    double avgWT = 0, avgTAT = 0;
    int idleTime = 0, totalTime = 0;

    if(choice == 1){
        cout << "\n=== FCFS SELECTED ===\n";
        fcfsScheduling(pList, execOrder, timeline, avgWT, avgTAT, idleTime, totalTime);
    }
    else if(choice == 2){
        cout << "\n=== SJF NON-PREEMPTIVE SELECTED ===\n";
        sjfNonPreemptive(pList, execOrder, timeline, avgWT, avgTAT, idleTime, totalTime);
    }
    else if(choice == 3){
        int q;
        cout << "Enter Time Quantum: ";
        cin >> q;
        cout << "\n=== ROUND ROBIN SELECTED ===\n";
        roundRobin(pList, execOrder, timeline, avgWT, avgTAT, idleTime, totalTime, q);
    }
    else if(choice == 4){
        cout << "\n=== PRIORITY SCHEDULING SELECTED ===\n";
        priorityScheduling(pList, execOrder, timeline, avgWT, avgTAT, idleTime, totalTime);
    }
    else{
        cout << "Invalid choice. Exiting.\n";
        return 0;
    }

    displayResults(pList, avgWT, avgTAT, idleTime, totalTime);
    generateGanttChart(execOrder, timeline);

    return 0;
}
