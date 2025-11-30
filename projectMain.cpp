#include <bits/stdc++.h>
using namespace std;

// ---------------------------------------------------------
// STRUCT FOR A PROCESS
// ---------------------------------------------------------
struct Process {
    int pid, at, bt, ct, tat, wt;
    bool completed;

    Process(int _pid=0, int _at=0, int _bt=0) {
        pid = _pid;
        at = _at;
        bt = _bt;
        ct = tat = wt = 0;
        completed = false;
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

    cout << "\nEnter (PID ArrivalTime BurstTime):\n";
    for(int i=0; i<n; i++) {
        int pid, at, bt;
        cin >> pid >> at >> bt;

        if(at < 0 || bt <= 0) {
            cout << "Invalid input! Arrival >= 0, Burst > 0.\n";
            i--;
            continue;
        }
        pList.emplace_back(pid, at, bt);
    }
    return pList;
}

// ---------------------------------------------------------
// FCFS Scheduling
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
// SJF NON-PREEMPTIVE + IDLE TIME
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
// IMPROVED GANTT CHART
// ---------------------------------------------------------
void generateGanttChart(const vector<int>& execOrder,
                        const vector<pair<int,int>>& timeline) {
    cout << "\n\n---------- GANTT CHART ----------\n";

    // Top bar
    for(int i=0; i<execOrder.size(); i++) {
        cout << "+-------";
    }
    cout << "+\n";

    // Process blocks
    for(int i=0; i<execOrder.size(); i++) {
        cout << "|  P" << execOrder[i] << "  ";
    }
    cout << "|\n";

    // Bottom bar
    for(int i=0; i<execOrder.size(); i++) {
        cout << "+-------";
    }
    cout << "+\n";

    // Timeline
    for(int i=0; i<timeline.size(); i++){
        cout << timeline[i].first << "\t";
    }
    cout << timeline.back().second << "\n";
}

// ---------------------------------------------------------
// DISPLAY RESULTS + UTILIZATION + THROUGHPUT
// ---------------------------------------------------------
void displayResults(
    const vector<Process>& p,
    double avgWT,
    double avgTAT,
    int idleTime,
    int totalTime
){
    cout << "\n\n========== FINAL TABLE ==========\n";
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\n";

    for(const auto &x : p){
        cout << x.pid << "\t"
             << x.at << "\t"
             << x.bt << "\t"
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
// MAIN — MENU SYSTEM
// ---------------------------------------------------------
int main() {

    vector<Process> pList = inputProcesses();

    cout << "\nChoose Scheduling Algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SJF Non-Preemptive\n";
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
    else{
        cout << "Invalid choice. Exiting.\n";
        return 0;
    }

    displayResults(pList, avgWT, avgTAT, idleTime, totalTime);
    generateGanttChart(execOrder, timeline);

    return 0;
}
