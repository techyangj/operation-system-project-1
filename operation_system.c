#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX 256

struct Process {
    int pid;
    int arrival;
    int burst;
    int priority;
};

/* ---------- printing helpers ---------- */
static void print_usage(const char *prog) {
    printf("Usage: %s [-h] [-a {fcfs|sjf|both}] [-f <input_file>]\n", prog);
    printf("  -h, --help          Show this help and exit\n");
    printf("  -a <algo>           Scheduling algorithm to run (default: both)\n");
    printf("                      fcfs | sjf | both\n");
    printf("  -f <file>           Input file path (default: processes.txt)\n");
}

static void print_gantt(int seg_pid[], int seg_s[], int seg_e[], int m) {
    if (m <= 0) return;
    printf("\nGantt chart:\n| ");
    for (int i = 0; i < m; ++i) {
        if (seg_pid[i] == -1) printf("Idle | ");
        else printf("P%d | ", seg_pid[i]);
    }
    printf("\n");
    printf("%d", seg_s[0]);
    for (int i = 0; i < m; ++i) printf("   %d", seg_e[i]);
    printf("\n");
}

/* ---------- FCFS ---------- */
static void fcfs(struct Process procs[], int n) {
    struct Process a[MAX];
    for (int i = 0; i < n; ++i) a[i] = procs[i];

    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (a[j].arrival > a[j+1].arrival ||
               (a[j].arrival == a[j+1].arrival && a[j].pid > a[j+1].pid)) {
                struct Process t = a[j]; a[j] = a[j+1]; a[j+1] = t;
            }

    int t = 0, C[MAX], WT[MAX], TAT[MAX];
    int seg_pid[MAX*2], seg_s[MAX*2], seg_e[MAX*2], m = 0;
    int order[MAX], ordn = 0;

    printf("\n----- FCFS -----\nprocessing sequence:\n");
    for (int i = 0; i < n; ++i) {
        if (t < a[i].arrival) { seg_pid[m]=-1; seg_s[m]=t; seg_e[m]=a[i].arrival; m++; t=a[i].arrival; }
        int start = t;
        t += a[i].burst;
        C[i]   = t;
        WT[i]  = start - a[i].arrival;
        TAT[i] = C[i] - a[i].arrival;

        seg_pid[m]=a[i].pid; seg_s[m]=start; seg_e[m]=t; m++;
	order[ordn++] = i;
        printf("P%d%s", a[i].pid, (i==n-1? "\n":" -> "));
    }

    printf("PID\tArr\tBurst\tWT\tTAT\tC\n");
    double sumWT=0, sumTAT=0;
    for (int k = 0; k < ordn; ++k) {
	int i = order[k];
        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               a[i].pid, a[i].arrival, a[i].burst, WT[i], TAT[i], C[i]);
        sumWT += WT[i]; sumTAT += TAT[i];
    }
    printf("Avg WT=%.2f  Avg TAT=%.2f\n", sumWT/n, sumTAT/n);

    print_gantt(seg_pid, seg_s, seg_e, m);
}

/* ---------- SJF (non-preemptive) ---------- */
static void sjf(struct Process procs[], int n) {
    struct Process a[MAX];
    for (int i = 0; i < n; ++i) a[i] = procs[i];

    int done[MAX]={0}, C[MAX], WT[MAX], TAT[MAX];
    int finished=0, t=0;
    int seg_pid[MAX*2], seg_s[MAX*2], seg_e[MAX*2], m=0;

    int order[MAX], ordn = 0;
    printf("\n----- SJF (non-preemptive) -----\nprocessing sequence:\n");

    while (finished < n) {
        int pick = -1;
        for (int i = 0; i < n; ++i) {
            if (done[i] || a[i].arrival > t) continue;
            if (pick == -1 ||
                a[i].burst < a[pick].burst ||
               (a[i].burst == a[pick].burst &&
                (a[i].arrival < a[pick].arrival ||
                (a[i].arrival == a[pick].arrival && a[i].pid < a[pick].pid))))
                pick = i;
        }
        if (pick == -1) {
            int soon = INT_MAX;
            for (int i = 0; i < n; ++i)
                if (!done[i] && a[i].arrival < soon) soon = a[i].arrival;
            seg_pid[m]=-1; seg_s[m]=t; seg_e[m]=soon; m++;
            t = soon;
            continue;
        }
        int start = t;
        t += a[pick].burst;
        C[pick]   = t;
        WT[pick]  = start - a[pick].arrival;
        TAT[pick] = C[pick] - a[pick].arrival;
        done[pick]=1; finished++;

        seg_pid[m]=a[pick].pid; seg_s[m]=start; seg_e[m]=t; m++;
	order[ordn++] = pick;
        printf("P%d%s", a[pick].pid, (finished==n? "\n":" -> "));
    }

    printf("PID\tArr\tBurst\tWT\tTAT\tC\n");
    double sumWT=0, sumTAT=0;
    for (int k = 0; k < ordn; ++k) {
	int i = order[k];
        printf("%d\t%d\t%d\t%d\t%d\t%d\n",
               a[i].pid, a[i].arrival, a[i].burst, WT[i], TAT[i], C[i]);
        sumWT += WT[i]; sumTAT += TAT[i];
    }
    printf("Avg WT=%.2f  Avg TAT=%.2f\n", sumWT/n, sumTAT/n);

    print_gantt(seg_pid, seg_s, seg_e, m);
}

/* ---------- input ---------- */
static int read_processes(const char *path, struct Process procs[], int *n) {
    FILE *fp = fopen(path, "r");
    if (!fp) return -1;

    int count = 0;
    char line[MAX];
    while (fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p=='\0' || *p=='#' || (p[0]=='/' && p[1]=='/')) continue;

        struct Process t;
        if (sscanf(p, "%d %d %d %d", &t.pid, &t.arrival, &t.burst, &t.priority) == 4) {
            if (count < MAX) procs[count++] = t;
        }
    }
    fclose(fp);
    *n = count;
    return 0;
}

/* ---------- main ---------- */
int main(int argc, char *argv[]) {
    const char *alg  = "both";             // Default run all alg
    const char *file = "processes.txt";    // Default input file

    // parameter parsing
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            print_usage(argv[0]);
            return 0;
        } else if (!strcmp(argv[i], "-a") && i+1 < argc) {
            alg = argv[++i];
        } else if (!strcmp(argv[i], "-f") && i+1 < argc) {
            file = argv[++i];
        } else {
            printf("Unknown or incomplete option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    struct Process procs[MAX]; int n = 0;
    if (read_processes(file, procs, &n) != 0 || n == 0) {
        printf("Error: cannot read processes from '%s'\n", file);
        return 1;
    }

    // print .txt content
    printf("Read %d processes from %s\n", n, file);
    printf("PID\tArrival\tBurst\tPriority\n");
    for (int i = 0; i < n; ++i)
        printf("%d\t%d\t%d\t%d\n", procs[i].pid, procs[i].arrival, procs[i].burst, procs[i].priority);

    // function about alg
    if (!strcmp(alg, "fcfs"))       fcfs(procs, n);
    else if (!strcmp(alg, "sjf"))   sjf(procs, n);
    else if (!strcmp(alg, "both")) { fcfs(procs, n); sjf(procs, n); }
    else {
        printf("Unknown algorithm: %s\n", alg);
        print_usage(argv[0]);
        return 1;
    }
    return 0;
}
