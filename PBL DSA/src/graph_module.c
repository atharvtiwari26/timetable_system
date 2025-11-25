#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"

#define MAX_DAYS 6
#define MAX_SLOTS 10

/* ===========================================================
   MODULE 7 — Timetable Conflict & Idle Slot Detection (Graph)
   =========================================================== */

static int adj[MAX_NODES][MAX_NODES];
static GraphNode nodes[MAX_NODES];
static int nodeCount = 0;

/* ---------------- UTILITY: Add a class node ---------------- */
static void addNode(const char *faculty, const char *course,
                    const char *room, int day, int slot)
{
    strcpy(nodes[nodeCount].faculty, faculty);
    strcpy(nodes[nodeCount].course, course);
    strcpy(nodes[nodeCount].room, room);
    nodes[nodeCount].day = day;
    nodes[nodeCount].slot = slot;
    nodeCount++;
}

/* ---------------- BUILD GRAPH FROM TIMETABLE FILE ---------------- */
static void buildGraphFromFile(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("  Could not open file: %s\n", path);
        return;
    }

    char faculty[50], course[20], room[20], group[10], notes[50];
    int day, slot, occupied;

    while (fscanf(fp, "%d %d %d %s %s %s %s %s %s",
                  &day, &slot, &occupied,
                  course, notes, faculty, room, group, notes) == 9)
    {
        if (occupied == 1)
            addNode(faculty, course, room, day, slot);
    }

    fclose(fp);

    // initialize adjacency
    for (int i = 0; i < nodeCount; i++)
        for (int j = 0; j < nodeCount; j++)
            adj[i][j] = 0;

    // build conflict edges
    for (int i = 0; i < nodeCount; i++)
        for (int j = i + 1; j < nodeCount; j++) {
            if (nodes[i].day == nodes[j].day &&
                nodes[i].slot == nodes[j].slot) {

                if (strcmp(nodes[i].faculty, nodes[j].faculty) == 0 ||
                    strcmp(nodes[i].room, nodes[j].room) == 0) {

                    adj[i][j] = adj[j][i] = 1;
                }
            }
        }
}

/* ---------------- DETECT CONFLICTS (NOW TAKES FILENAME) ---------------- */
void detectConflicts(const char *filename)
{
    nodeCount = 0;
    buildGraphFromFile(filename);

    if (nodeCount == 0) {
        printf("  No timetable entries found to analyze.\n");
        return;
    }

    printf("\n=== Conflict Report ===\n");
    int found = 0;
    for (int i = 0; i < nodeCount; i++) {
        for (int j = i + 1; j < nodeCount; j++) {
            if (adj[i][j] == 1) {
                printf("  Conflict: %s (%s) and %s (%s) overlap "
                       "on Day %d Slot %d [Room: %s]\n",
                       nodes[i].faculty, nodes[i].course,
                       nodes[j].faculty, nodes[j].course,
                       nodes[i].day, nodes[i].slot, nodes[i].room);
                found = 1;
            }
        }
    }
    if (!found) printf(" No timetable conflicts detected.\n");
}

/* ---------------- DETECT IDLE SLOTS ---------------- */
static void detectIdleSlots(void)
{
    int used[MAX_DAYS][MAX_SLOTS] = {0};

    for (int i = 0; i < nodeCount; i++)
        used[nodes[i].day][nodes[i].slot] = 1;

    printf("\n=== Idle Slot Report ===\n");
    for (int d = 0; d < MAX_DAYS; d++) {
        int hasIdle = 0;
        for (int s = 0; s < MAX_SLOTS; s++) {
            if (!used[d][s]) {
                if (!hasIdle) {
                    printf("Day %d: ", d);
                    hasIdle = 1;
                }
                printf("[%d] ", s);
            }
        }
        if (hasIdle) printf("\n");
    }
    printf(" End of idle slot scan.\n");
}

/* ---------------- MAIN MENU ---------------- */
void graphMenu(void)
{
    char filename[128];
    printf("\nEnter timetable filename (e.g., data/timetable_BTECH_CSE_3_A1.txt): ");
    scanf("%127s", filename);

    nodeCount = 0;
    buildGraphFromFile(filename);

    if (nodeCount == 0) {
        printf("  No data found or file empty.\n");
        return;
    }

    int ch;
    do {
        printf("\n=== GRAPH MODULE ===\n");
        printf("1. View Conflicts\n");
        printf("2. View Idle Slots\n");
        printf("3. Exit to Admin Menu\n");
        printf("Choice: ");
        scanf("%d", &ch);
        while (getchar() != '\n');

        switch (ch) {
            case 1:
                detectConflicts(filename);
                break;

            case 2:
                detectIdleSlots();
                break;

            case 3:
                printf("Returning to Admin Menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (ch != 3);
}
