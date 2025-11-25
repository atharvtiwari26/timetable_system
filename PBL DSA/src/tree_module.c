#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"

/* ===========================================================
   MODULE 8 — Timetable Search & Organization (TREE)
   =========================================================== */

/* ----------- CREATE & INSERT NODE (sorted by faculty name) ----------- */
static TTNode* createNode(const char *faculty, const char *course,
                          const char *section, const char *room,
                          int day, int slot)
{
    TTNode *node = (TTNode*)malloc(sizeof(TTNode));
    strcpy(node->faculty, faculty);
    strcpy(node->course, course);
    strcpy(node->section, section);
    strcpy(node->room, room);
    node->day = day;
    node->slot = slot;
    node->left = node->right = NULL;
    return node;
}

static TTNode* insertNode(TTNode *root, const char *faculty, const char *course,
                          const char *section, const char *room,
                          int day, int slot)
{
    if (root == NULL)
        return createNode(faculty, course, section, room, day, slot);

    if (strcmp(faculty, root->faculty) < 0)
        root->left = insertNode(root->left, faculty, course, section, room, day, slot);
    else
        root->right = insertNode(root->right, faculty, course, section, room, day, slot);

    return root;
}

/* ----------- LOAD TIMETABLE FILE INTO TREE ----------- */
static TTNode* loadTimetableIntoTree(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("⚠️  Could not open file: %s\n", path);
        return NULL;
    }

    TTNode *root = NULL;
    int day, slot, occupied;
    char code[10], subject[50], faculty[50], room[20], group[10], notes[50];

    while (fscanf(fp, "%d %d %d %s %s %s %s %s %s",
                  &day, &slot, &occupied,
                  code, subject, faculty, room, group, notes) == 9)
    {
        if (occupied == 1)
            root = insertNode(root, faculty, code, group, room, day, slot);
    }

    fclose(fp);
    return root;
}

/* ----------- DISPLAY TREE (IN-ORDER SORTED BY FACULTY) ----------- */
static void inorderPrint(TTNode *root)
{
    if (!root) return;
    inorderPrint(root->left);
    printf("Faculty: %-20s | Course: %-8s | Section: %-6s | Day: %d | Slot: %d | Room: %s\n",
           root->faculty, root->course, root->section, root->day, root->slot, root->room);
    inorderPrint(root->right);
}

/* ----------- SEARCH FUNCTIONS ----------- */
static void searchByFaculty(TTNode *root, const char *faculty)
{
    if (!root) return;
    searchByFaculty(root->left, faculty);
    if (strcasecmp(root->faculty, faculty) == 0)
        printf("Day %d Slot %d | Course: %-8s | Section: %-6s | Room: %s\n",
               root->day, root->slot, root->course, root->section, root->room);
    searchByFaculty(root->right, faculty);
}

static void searchByCourse(TTNode *root, const char *course)
{
    if (!root) return;
    searchByCourse(root->left, course);
    if (strcasecmp(root->course, course) == 0)
        printf("Faculty: %-20s | Section: %-6s | Day: %d Slot: %d | Room: %s\n",
               root->faculty, root->section, root->day, root->slot, root->room);
    searchByCourse(root->right, course);
}

static void searchByDay(TTNode *root, int day)
{
    if (!root) return;
    searchByDay(root->left, day);
    if (root->day == day)
        printf("Slot %d | %-20s | %-8s | %-6s | %s\n",
               root->slot, root->faculty, root->course, root->section, root->room);
    searchByDay(root->right, day);
}

/* ----------- FREE TREE MEMORY ----------- */
static void freeTree(TTNode *root)
{
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* ----------- FINAL FIX: treeMenu TAKES FILENAME ----------- */
void treeMenu(const char *filename)
{
    TTNode *root = loadTimetableIntoTree(filename);
    if (!root) {
        printf("⚠️  File not loaded or empty.\n");
        return;
    }

    int ch;
    do {
        printf("\n=== TIMETABLE SEARCH (TREE MODULE) ===\n");
        printf("1. View All (Sorted by Faculty)\n");
        printf("2. Search by Faculty Name\n");
        printf("3. Search by Course Code\n");
        printf("4. View All Classes on Specific Day\n");
        printf("5. Exit to Admin Menu\n");
        printf("Choice: ");
        scanf("%d", &ch);
        while (getchar() != '\n');

        switch (ch) {
            case 1:
                printf("\n--- SORTED TIMETABLE ---\n");
                inorderPrint(root);
                break;

            case 2: {
                char fac[50];
                printf("Enter Faculty Name: ");
                scanf("%49s", fac);
                printf("\n--- Classes by %s ---\n", fac);
                searchByFaculty(root, fac);
                break;
            }

            case 3: {
                char course[20];
                printf("Enter Course Code: ");
                scanf("%19s", course);
                printf("\n--- Faculty for %s ---\n", course);
                searchByCourse(root, course);
                break;
            }

            case 4: {
                int day;
                printf("Enter Day (0–5): ");
                scanf("%d", &day);
                printf("\n--- Classes on Day %d ---\n", day);
                searchByDay(root, day);
                break;
            }

            case 5:
                printf("Returning to Admin Menu...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (ch != 5);

    freeTree(root);
}
