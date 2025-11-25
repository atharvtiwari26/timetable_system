#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"


#define EXTRA_FILE "data/extra_classes.txt"

/* ============================================================
   MODULE 6 — EXTRA CLASS SCHEDULER (FINAL PRODUCTION VERSION)
   ============================================================ */

/* -------------------- ROOM DATABASE -------------------- */
static const char *availableRooms[] = {
    "LT101", "LT102", "LT201", "LT202",
    "CR101", "CR102", "CR201", "CR202",
    "LAB1", "LAB2"
};
#define ROOM_COUNT (sizeof(availableRooms) / sizeof(availableRooms[0]))

/* -------------------- QUEUE FUNCTIONS -------------------- */
static void initQueue(Queue *q) { q->front = q->rear = -1; }

static int isQueueEmpty(Queue *q) { return q->front == -1; }

static int isQueueFull(Queue *q) { return q->rear == MAX_EXTRA - 1; }

static void enqueue(Queue *q, ExtraClass x) {
    if (isQueueFull(q)) return;
    if (q->front == -1) q->front = 0;
    q->arr[++(q->rear)] = x;
}

static ExtraClass dequeue(Queue *q) {
    ExtraClass temp = {"-", "-", "-", "-", "-", "-", "-", "-"};
    if (isQueueEmpty(q)) return temp;
    temp = q->arr[q->front++];
    if (q->front > q->rear) q->front = q->rear = -1;
    return temp;
}

/* -------------------- FILE HANDLING -------------------- */
static void saveQueue(Queue *q) {
    FILE *fp = fopen(EXTRA_FILE, "w");
    if (!fp) return;
    for (int i = q->front; i <= q->rear && i >= 0; i++) {
        ExtraClass *x = &q->arr[i];
        fprintf(fp, "%s %s %s %s %s %s %s %s\n",
                x->faculty, x->course, x->section,
                x->date, x->time, x->room, x->reason, x->status);
    }
    fclose(fp);
}

static void loadQueue(Queue *q) {
    FILE *fp = fopen(EXTRA_FILE, "r");
    if (!fp) return;
    initQueue(q);
    ExtraClass x;
    while (fscanf(fp, "%49s %9s %19s %14s %9s %19s %99s %9s",
                  x.faculty, x.course, x.section,
                  x.date, x.time, x.room, x.reason, x.status) == 8) {
        enqueue(q, x);
    }
    fclose(fp);
}

/* -------------------- VALIDATION HELPERS -------------------- */
static int isFutureDate(const char *date) {
    int y, m, d;
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (y < 2024 || m < 1 || m > 12 || d < 1 || d > 31) return 0;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    int cy = t->tm_year + 1900, cm = t->tm_mon + 1, cd = t->tm_mday;

    if (y < cy) return 0;
    if (y == cy && m < cm) return 0;
    if (y == cy && m == cm && d < cd) return 0;
    return 1;
}

static int isValidTime(const char *time) {
    if (strlen(time) != 5 || time[2] != ':') return 0;
    int hh, mm;
    if (sscanf(time, "%d:%d", &hh, &mm) != 2) return 0;
    return (hh >= 8 && hh <= 18 && mm >= 0 && mm < 60);
}

/* -------------------- DISPLAY QUEUE -------------------- */
static void viewAll(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("No extra class requests.\n");
        return;
    }

    printf("\n%-3s %-15s %-8s %-8s %-12s %-8s %-8s %-10s %-20s\n",
           "ID", "Faculty", "Course", "Section",
           "Date", "Time", "Room", "Status", "Reason");
    printf("-------------------------------------------------------------------------------\n");

    for (int i = q->front; i <= q->rear; i++) {
        ExtraClass *x = &q->arr[i];
        printf("%-3d %-15s %-8s %-8s %-12s %-8s %-8s %-10s %-20s\n",
               i, x->faculty, x->course, x->section,
               x->date, x->time, x->room, x->status, x->reason);
    }
}

/* -------------------- FACULTY INTERFACE -------------------- */
static void facultyRequest(void) {
    ExtraClass x;
    FILE *fp = fopen(EXTRA_FILE, "a");
    if (!fp) {
        printf(" Could not open file for writing.\n");
        return;
    }

    printf("Faculty Name (use _): ");
    scanf("%49s", x.faculty);
    printf("Course Code: ");
    scanf("%9s", x.course);
    printf("Section: ");
    scanf("%19s", x.section);

    do {
        printf("Date (YYYY-MM-DD): ");
        scanf("%14s", x.date);
        if (!isFutureDate(x.date)) printf(" Invalid date. Must be future.\n");
    } while (!isFutureDate(x.date));

    do {
        printf("Time (HH:MM, 24-hour between 08:00–18:00): ");
        scanf("%9s", x.time);
        if (!isValidTime(x.time)) printf(" Invalid time format.\n");
    } while (!isValidTime(x.time));

    strcpy(x.room, "Not_Assigned");
    printf("Reason (use _ for spaces): ");
    scanf("%99s", x.reason);
    strcpy(x.status, "Pending");

    fprintf(fp, "%s %s %s %s %s %s %s %s\n",
            x.faculty, x.course, x.section,
            x.date, x.time, x.room, x.reason, x.status);
    fclose(fp);

    printf("\n Request submitted successfully.\n");
    printf("Status: %s | Room: %s\n", x.status, x.room);
}

/* -------------------- ADMIN INTERFACE -------------------- */
static void adminManage(void) {
    Queue q;
    loadQueue(&q);
    int ch;

    do {
        printf("\n=== ADMIN: EXTRA CLASS REQUEST QUEUE ===\n");
        printf("1. View All Requests\n");
        printf("2. Approve / Reject a Request\n");
        printf("3. Clear All Requests\n");
        printf("4. Exit\n");
        printf("Choice: ");
        if (scanf("%d", &ch) != 1) { ch = 4; }
        while (getchar() != '\n');

        switch (ch) {
            case 1:
                viewAll(&q);
                break;

            case 2: {
                if (isQueueEmpty(&q)) {
                    printf("Queue empty.\n");
                    break;
                }

                viewAll(&q);
                int idx;
                printf("\nEnter request ID to manage: ");
                scanf("%d", &idx);
                if (idx < q.front || idx > q.rear) {
                    printf("Invalid ID.\n");
                    break;
                }

                ExtraClass *req = &q.arr[idx];
                if (strcmp(req->status, "Pending") != 0) {
                    printf(" Already processed (%s).\n", req->status);
                    break;
                }

                printf("\nFaculty: %s | Course: %s | Section: %s\n",
                       req->faculty, req->course, req->section);
                printf("Date: %s | Time: %s | Reason: %s\n",
                       req->date, req->time, req->reason);

                printf("\nAvailable Rooms:\n");
                for (int i = 0; i < ROOM_COUNT; i++)
                    printf("%d. %s\n", i + 1, availableRooms[i]);

                int rch;
                printf("\nApprove(1) / Reject(2): ");
                scanf("%d", &rch);

                if (rch == 1) {
                    int roomChoice;
                    printf("Select room number to assign: ");
                    scanf("%d", &roomChoice);
                    if (roomChoice < 1 || roomChoice > ROOM_COUNT)
                        strcpy(req->room, "Manual_Assign");
                    else
                        strcpy(req->room, availableRooms[roomChoice - 1]);
                    strcpy(req->status, "Approved");
                    printf(" Approved and assigned Room %s.\n", req->room);
                } else {
                    strcpy(req->status, "Rejected");
                    strcpy(req->room, "-");
                    printf(" Request rejected.\n");
                }

                saveQueue(&q);
                break;
            }

            case 3:
                initQueue(&q);
                saveQueue(&q);
                printf(" Cleared all requests.\n");
                break;

            case 4:
                printf("Returning to Admin Menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (ch != 4);
}

/* -------------------- MAIN ENTRY POINT -------------------- */
void reportMenu(const char *role) {
    if (strcmp(role, "faculty") == 0) {
        int ch;
        do {
            printf("\n=== FACULTY EXTRA CLASS MENU ===\n");
            printf("1. Request Extra Class\n");
            printf("2. Exit to Faculty Menu\n");
            printf("Choice: ");
            if (scanf("%d", &ch) != 1) { ch = 2; }
            while (getchar() != '\n');

            switch (ch) {
                case 1: facultyRequest(); break;
                case 2: printf("Returning to Faculty Menu...\n"); break;
                default: printf("Invalid choice.\n");
            }
        } while (ch != 2);
    }
    else if (strcmp(role, "admin") == 0) {
        adminManage();
    }
    else {
        printf("Access denied.\n");
    }
}
