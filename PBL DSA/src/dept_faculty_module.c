#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"

#ifdef _WIN32
#define CI_CMP _stricmp
#else
#include <strings.h>
#define CI_CMP strcasecmp
#endif

#define DEPT_FILE    "data/departments.txt"
#define FACULTY_FILE "data/faculty.txt"

static void ensure_data_folder(void) {
#ifdef _WIN32
    system("if not exist data mkdir data >nul 2>nul");
#else
    system("mkdir -p data >/dev/null 2>&1");
#endif
}

/* ---------- Linked list creators ---------- */
static Department* makeDeptNode(const char *name) {
    Department *d = (Department*)malloc(sizeof(Department));
    strcpy(d->name, name);
    d->facultyHead = NULL;
    d->next = NULL;
    return d;
}

static Faculty* makeFacultyNode(const char *id, const char *name, const char *dept) {
    Faculty *f = (Faculty*)malloc(sizeof(Faculty));
    strcpy(f->id, id);
    strcpy(f->name, name);
    strcpy(f->department, dept);
    f->next = NULL;
    return f;
}

/* ---------- Load / Save departments ---------- */
Department* loadDepartments(void) {
    ensure_data_folder();
    FILE *fp = fopen(DEPT_FILE, "r");
    if (!fp) {
        // create default file
        fp = fopen(DEPT_FILE, "w");
        if (fp) {
            fprintf(fp, "CSE\nECE\nEE\nME\nCE\nIT\nAI\nMathematics\nPhysics\nChemistry\nEnglish\n");
            fclose(fp);
        }
        fp = fopen(DEPT_FILE, "r");
        if (!fp) return NULL;
    }

    Department *head = NULL, *tail = NULL;
    char line[64];
    while (fgets(line, sizeof(line), fp)) {
        char name[30];
        if (sscanf(line, "%29s", name) != 1) continue;
        Department *d = makeDeptNode(name);
        if (!head) head = tail = d;
        else { tail->next = d; tail = d; }
    }
    fclose(fp);
    return head;
}

void saveDepartments(Department *head) {
    ensure_data_folder();
    FILE *fp = fopen(DEPT_FILE, "w");
    if (!fp) return;
    for (Department *d = head; d; d = d->next)
        fprintf(fp, "%s\n", d->name);
    fclose(fp);
}

/* ---------- Load / Save faculty ---------- */
void loadFaculty(Department **deptHead) {
    ensure_data_folder();
    FILE *fp = fopen(FACULTY_FILE, "r");
    if (!fp) return;

    char id[10], name[50], dept[30];
    while (fscanf(fp, "%9s %49s %29s", id, name, dept) == 3) {
        Department *d = *deptHead;
        while (d && CI_CMP(d->name, dept) != 0)
            d = d->next;
        if (!d) continue;  // department not found
        Faculty *f = makeFacultyNode(id, name, dept);
        if (!d->facultyHead) d->facultyHead = f;
        else {
            Faculty *t = d->facultyHead;
            while (t->next) t = t->next;
            t->next = f;
        }
    }
    fclose(fp);
}

void saveFaculty(Department *deptHead) {
    ensure_data_folder();
    FILE *fp = fopen(FACULTY_FILE, "w");
    if (!fp) return;
    for (Department *d = deptHead; d; d = d->next)
        for (Faculty *f = d->facultyHead; f; f = f->next)
            fprintf(fp, "%s %s %s\n", f->id, f->name, f->department);
    fclose(fp);
}

/* ---------- Utility helpers ---------- */
Department* findDepartment(Department *head, const char *name) {
    for (Department *d = head; d; d = d->next)
        if (CI_CMP(d->name, name) == 0)
            return d;
    return NULL;
}

static void addDepartmentInteractive(Department **head) {
    char name[30];
    printf("Enter new Department name: ");
    scanf("%29s", name);
    if (findDepartment(*head, name)) {
        printf("Department already exists.\n");
        return;
    }
    Department *d = makeDeptNode(name);
    d->next = *head;
    *head = d;
    saveDepartments(*head);
    printf("Department '%s' added.\n", name);
}

static void displayDepartments(Department *head) {
    if (!head) { printf("No departments found.\n"); return; }
    printf("\nDepartments:\n");
    int i = 1;
    for (Department *d = head; d; d = d->next)
        printf("%2d. %s\n", i++, d->name);
}

static void addFacultyInteractive(Department *deptHead) {
    char id[10], name[50], dept[30];
    printf("Faculty ID (e.g., F123): "); scanf("%9s", id);
    printf("Faculty Name (use _ for spaces): "); scanf("%49s", name);
    printf("Department: "); scanf("%29s", dept);

    Department *d = findDepartment(deptHead, dept);
    if (!d) {
        printf("Department not found.\n");
        return;
    }

    Faculty *f = makeFacultyNode(id, name, dept);
    if (!d->facultyHead) d->facultyHead = f;
    else {
        Faculty *t = d->facultyHead;
        while (t->next) t = t->next;
        t->next = f;
    }
    saveFaculty(deptHead);
    printf("Faculty '%s' added to %s.\n", name, dept);
}

static void displayFacultyByDept(Department *deptHead) {
    char dept[30];
    printf("Enter department: ");
    scanf("%29s", dept);
    Department *d = findDepartment(deptHead, dept);
    if (!d) { printf("Department not found.\n"); return; }
    if (!d->facultyHead) { printf("No faculty in %s.\n", dept); return; }

    printf("\nFaculty in %s:\n", dept);
    printf("%-10s %-20s\n", "ID", "Name");
    for (Faculty *f = d->facultyHead; f; f = f->next)
        printf("%-10s %-20s\n", f->id, f->name);
}

/* ---------- Menu ---------- */
void deptFacultyMenu(void) {
    Department *deptHead = loadDepartments();
    loadFaculty(&deptHead);
    int ch;
    do {
        printf("\n=== DEPARTMENT & FACULTY MANAGEMENT ===\n");
        printf("1. Add Department\n");
        printf("2. Show Departments\n");
        printf("3. Add Faculty\n");
        printf("4. Show Faculty by Department\n");
        printf("5. Save & Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &ch) != 1) { ch = 5; }
        while (getchar() != '\n');
        switch (ch) {
            case 1: addDepartmentInteractive(&deptHead); break;
            case 2: displayDepartments(deptHead); break;
            case 3: addFacultyInteractive(deptHead); break;
            case 4: displayFacultyByDept(deptHead); break;
            case 5:
                saveDepartments(deptHead);
                saveFaculty(deptHead);
                printf("Data saved. Exiting...\n");
                break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 5);

    // free memory
    while (deptHead) {
        Department *d = deptHead;
        deptHead = deptHead->next;
        Faculty *f = d->facultyHead;
        while (f) { Faculty *t = f; f = f->next; free(t); }
        free(d);
    }
}
