#ifndef HEADERS_H
#define HEADERS_H

/* =============================================================
   GEHU TIMETABLE MANAGEMENT SYSTEM - MASTER HEADER FILE
   ============================================================= */

/* =================== COMMON CONSTANTS =================== */
#define MAX_USERS   300
#define MAX_ID      32
#define MAX_PW      64
#define MAX_ROLE    10
#define DAYS        6       // Monday–Saturday
#define SLOTS       10      // Up to 6 PM

/* =================== USER STRUCTURE =================== */
typedef struct {
    char id[MAX_ID];        // Username / UserID
    char enc_pw[MAX_PW];    // Encoded password (+2 shift)
    char role[MAX_ROLE];    // admin / faculty / student
} User;

/* =================== USER MODULE =================== */
void runUserSystem(void);

/* =================== DEPARTMENTS / FACULTY =================== */
typedef struct Faculty {
    char id[10];
    char name[50];
    char department[30];
    struct Faculty *next;
} Faculty;

typedef struct Department {
    char name[30];
    Faculty *facultyHead;
    struct Department *next;
} Department;

void deptFacultyMenu(void);
Department* loadDepartments(void);
void loadFaculty(Department **head);

/* =================== COURSE LINKING =================== */
typedef struct Course {
    char branch[20];       // e.g., CSE, ECE
    int semester;          // e.g., 3
    char code[10];         // e.g., TCS302
    char name[50];         // e.g., Data_Structures
    char facultyID[10];    // e.g., F001
    char facultyName[50];  // e.g., Prof_Anjali
    char department[30];   // e.g., CSE
    struct Course *next;
} Course;

void courseMenu(void);
Course* loadCourses(void);
void saveCourses(Course *head);

/* =================== TIMETABLE MODULE =================== */
typedef struct {
    char code[10];
    char subject[50];
    char faculty[50];
    char room[20];
    char group[8];
    char notes[50];
    int  occupied;         // 0 empty, 1 used
} TTCell;

typedef struct {
    char branch[20];       // e.g., BTECH_CSE
    int semester;
    char section[20];
    TTCell grid[DAYS][SLOTS];
} Timetable;

void timetableMenu(void);
void initTimetable(Timetable *tt);
void viewFacultyTimetable(const char *facultyName);

/* =================== UNDO / REDO SYSTEM =================== */
#define MAX_HISTORY 50

typedef struct {
    Timetable state;       // full timetable snapshot
} Snapshot;

typedef struct {
    Snapshot stack[MAX_HISTORY];
    int top;
} Stack;

void initUndoRedo(void);
void initStack(Stack *s);
int isEmpty(Stack *s);
int isFull(Stack *s);
void push(Stack *s, Timetable *t);
int pop(Stack *s, Timetable *out);
void undoMenu(void);

/* =================== EXTRA CLASS / REPORTS =================== */
/* =================== EXTRA CLASS SCHEDULER =================== */
#define MAX_EXTRA 100   // ✅ define BEFORE Queue struct

typedef struct {
    char faculty[50];
    char course[10];
    char section[20];
    char date[15];
    char time[10];
    char room[20];
    char reason[100];
    char status[20];   // ✅ Added for Approved/Rejected/Pending
} ExtraClass;

typedef struct {
    ExtraClass arr[MAX_EXTRA];
    int front, rear;
} Queue;

void reportMenu(const char *role);  // main entry for Extra Class module

/* =================== STUDENT / FACULTY MENUS =================== */
void student_menu(const User *u);


/* =================== GRAPH MODULE (Timetable Conflict & Idle Detection) =================== */

#define MAX_NODES 200   // total classes across timetable
typedef struct {
    char faculty[50];
    char course[20];
    char room[20];
    int day;
    int slot;
} GraphNode;

void graphMenu(void);   // main entry for admin to run conflict detection

/* =================== TREE MODULE (Timetable Search & Organization) =================== */

typedef struct TTNode {
    char faculty[50];
    char course[20];
    char section[10];
    char room[20];
    int day;
    int slot;
    struct TTNode *left, *right;
} TTNode;




/* ========== GRAPH MODULE (Module 8) ========== */
void detectConflicts(const char *filename);

/* ========== TREE MODULE (Module 9) ========== */
void treeMenu(const char *filename);



#endif /* HEADERS_H */
