#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Time quantum for various queues
int time_q1 = 3;
int time_q2 = 2;
int time_q3 = 1;

// Global clock
int clock = 0;

struct priorityqueue {
    int pid;
    int priority;
    int arrtime;
    int burst;
    int burst_copy;
    struct priorityqueue *left;
    struct priorityqueue *right;
} *q1 = NULL, *q2 = NULL, *q3 = NULL;

struct result {
    int pid_result;
    int arrtime_result;
    int burst_result;
    int clock_result;
    struct result *left;
    struct result *right;
} *r = NULL;

// Insert into priority queue
struct priorityqueue* insert(struct priorityqueue* node, struct priorityqueue* q) {
    struct priorityqueue *newnode = (struct priorityqueue*) malloc(sizeof(struct priorityqueue));
    *newnode = *node; // Copying data
    newnode->left = NULL;
    newnode->right = NULL;

    if (!q) {
        q = newnode;
        q->right = q;
        q->left = q;
    } else {
        struct priorityqueue *last = q;
        while (last->right != q) {
            last = last->right;
        }
        last->right = newnode;
        newnode->left = last;
        newnode->right = q;
        q->left = newnode;
    }
    printf("\nInsertion completed.....\n");
    return q;
}

// Insert into result queue
struct result* insert_result(struct result* node, struct result* q) {
    struct result *newnode = (struct result*) malloc(sizeof(struct result));
    *newnode = *node;
    newnode->left = NULL;
    newnode->right = NULL;

    if (!q) {
        q = newnode;
    } else {
        struct result *last = q;
        while (last->right) {
            last = last->right;
        }
        last->right = newnode;
        newnode->left = last;
    }
    return q;
}

// Run simulation
void run_processes() {
    struct priorityqueue *current;
    int time_slice;

    printf("\nRunning Processes...\n");
    while (q1 || q2 || q3) {
        if (q1) {
            current = q1;
            time_slice = time_q1;
        } else if (q2) {
            current = q2;
            time_slice = time_q2;
        } else {
            current = q3;
            time_slice = time_q3;
        }

        if (current->arrtime <= clock) {
            int execute_time = (current->burst < time_slice) ? current->burst : time_slice;
            clock += execute_time;
            current->burst -= execute_time;

            if (current->burst == 0) {
                struct result res = {current->pid, current->arrtime, current->burst_copy, clock};
                r = insert_result(&res, r);
                printf("Process %d completed at time %d\n", current->pid, clock);

                if (current == q1) q1 = (q1->right == q1) ? NULL : q1->right;
                else if (current == q2) q2 = (q2->right == q2) ? NULL : q2->right;
                else q3 = (q3->right == q3) ? NULL : q3->right;
                
                free(current);
            } else {
                current = current->right;
            }
        } else {
            clock++;
        }
    }
    printf("\nRunning process is completed.\n");
}

// Display result
void display_result(struct result* r) {
    if (!r) {
        printf("No results to display.\n");
        return;
    }

    printf("\n\t\t\t RESULTS\n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\tWaiting Time\n");

    struct result *head = r;
    int wait_average = 0, completion_time_average = 0, count = 0;
    float cpu_util = 0;

    while (head) {
        int wait_time = head->clock_result - head->burst_result - head->arrtime_result;
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\n", head->pid_result, head->arrtime_result, head->burst_result, head->clock_result, wait_time);
        wait_average += wait_time;
        completion_time_average += head->clock_result;
        cpu_util += ((float)head->burst_result / head->clock_result);
        count++;
        head = head->right;
    }

    printf("\nAverage Waiting Time --> %.2f\n", (float)wait_average / count);
    printf("Average Completion Time (TAT) --> %.2f\n", (float)completion_time_average / count);
    printf("CPU Utilization --> %.2f%%\n", cpu_util * 100);
}

// Main function
int main() {
    int ch, n;
    struct priorityqueue node;

    while (1) {
        printf("\n\tADAPTIVE REAL TIME SCHEDULER\n");
        printf("1. Insert Process\t2. Run\t3. Print Results\t4. Exit\nEnter your choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1:
                printf("Enter number of processes: ");
                scanf("%d", &n);

                for (int i = 0; i < n; i++) {
                    printf("Enter PID, Arrival Time, Burst Time, Priority for Process-%d: ", i + 1);
                    scanf("%d %d %d %d", &node.pid, &node.arrtime, &node.burst, &node.priority);
                    node.burst_copy = node.burst;

                    if (node.priority > 100) {
                        q1 = insert(&node, q1);
                    } else if (node.priority > 50) {
                        q2 = insert(&node, q2);
                    } else {
                        q3 = insert(&node, q3);
                    }
                }
                break;

            case 2:
                run_processes();
                break;

            case 3:
                display_result(r);
                break;

            case 4:
                exit(0);

            default:
                printf("Invalid choice!\n");
        }
    }
}
