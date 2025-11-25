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

#define COURSE_FILE "data/courses.txt"

/* ---------- Linked-list helpers ---------- */
static Course* makeCourseNode(
    const char *branch, int sem, const char *code,
    const char *name, const char *fid, const char *fname, const char *dept)
{
    Course *c = (Course*)malloc(sizeof(Course));
    strcpy(c->branch, branch);
    c->semester = sem;
    strcpy(c->code, code);
    strcpy(c->name, name);
    strcpy(c->facultyID, fid);
    strcpy(c->facultyName, fname);
    strcpy(c->department, dept);
    c->next = NULL;
    return c;
}

/* ---------- Load/Save ---------- */
Course* loadCourses(void) {
    FILE *fp = fopen(COURSE_FILE, "r");
    if (!fp) return NULL;

    Course *head=NULL, *tail=NULL;
    while (!feof(fp)) {
        Course *c = (Course*)malloc(sizeof(Course));
        if (fscanf(fp, "%19s %d %9s %49s %9s %49s %29s",
                   c->branch, &c->semester, c->code, c->name,
                   c->facultyID, c->facultyName, c->department) != 7) {
            free(c); break;
        }
        c->next=NULL;
        if(!head) head=tail=c;
        else { tail->next=c; tail=c; }
    }
    fclose(fp);
    return head;
}

void saveCourses(Course *head) {
    FILE *fp = fopen(COURSE_FILE, "w");
    if (!fp) { printf("Error opening %s\n", COURSE_FILE); return; }
    for (Course *c=head; c; c=c->next)
        fprintf(fp,"%s %d %s %s %s %s %s\n",
                c->branch, c->semester, c->code, c->name,
                c->facultyID, c->facultyName, c->department);
    fclose(fp);
}

/* ---------- Helpers ---------- */
static Faculty* selectFaculty(Department *deptHead) {
    char dept[30];
    printf("Enter Department to choose faculty from: ");
    scanf("%29s", dept);
    Department *d = deptHead;
    while (d && CI_CMP(d->name, dept)!=0) d=d->next;
    if (!d || !d->facultyHead) { printf(" Dept not found or empty.\n"); return NULL; }

    printf("\nFaculty in %s:\n", dept);
    for (Faculty *f=d->facultyHead; f; f=f->next)
        printf("%s : %s\n", f->id, f->name);

    char fid[10];
    printf("Enter Faculty ID: ");
    scanf("%9s", fid);
    for (Faculty *f=d->facultyHead; f; f=f->next)
        if (strcmp(f->id,fid)==0) return f;

    printf("Invalid ID.\n");
    return NULL;
}

/* ---------- Core Operations ---------- */
static void addCourse(Course **head, Department *deptHead) {
    char branch[20], code[10], name[50];
    int sem;
    printf("Branch (e.g., CSE): "); scanf("%19s", branch);
    printf("Semester: "); scanf("%d", &sem);
    printf("Course Code: "); scanf("%9s", code);
    printf("Course Name (use _ for spaces): "); scanf("%49s", name);

    Faculty *fac = selectFaculty(deptHead);
    Course *n;
    if (fac)
        n = makeCourseNode(branch, sem, code, name, fac->id, fac->name, fac->department);
    else
        n = makeCourseNode(branch, sem, code, name, "-", "Unassigned", "-");

    if (!*head) *head=n;
    else { Course *t=*head; while(t->next) t=t->next; t->next=n; }

    saveCourses(*head);
    printf(" Added %s (%s) → %s.\n", code, name, fac?fac->name:"Unassigned");
}

static void listCourses(Course *head) {
    if(!head){ printf("No courses yet.\n"); return; }
    printf("\n%-6s %-4s %-10s %-25s %-8s %-20s %-15s\n",
           "Branch","Sem","Code","Name","FID","Faculty","Dept");
    printf("--------------------------------------------------------------------------\n");
    for(Course *c=head;c;c=c->next)
        printf("%-6s %-4d %-10s %-25s %-8s %-20s %-15s\n",
               c->branch,c->semester,c->code,c->name,
               c->facultyID,c->facultyName,c->department);
}

static void searchByBranch(Course *head){
    char b[20]; int found=0;
    printf("Enter branch: "); scanf("%19s", b);
    printf("\n%-6s %-4s %-10s %-25s %-8s %-20s %-15s\n",
           "Branch","Sem","Code","Name","FID","Faculty","Dept");
    for(Course *c=head;c;c=c->next)
        if(CI_CMP(c->branch,b)==0){
            printf("%-6s %-4d %-10s %-25s %-8s %-20s %-15s\n",
                   c->branch,c->semester,c->code,c->name,
                   c->facultyID,c->facultyName,c->department);
            found=1;
        }
    if(!found) printf("No courses found for %s.\n", b);
}

static void reassignFaculty(Course *head, Department *deptHead){
    char code[10];
    printf("Course Code to reassign: "); scanf("%9s", code);
    for(Course *c=head;c;c=c->next){
        if(strcmp(c->code,code)==0){
            printf("Current Faculty: %s (%s)\n", c->facultyName, c->facultyID);
            Faculty *f=selectFaculty(deptHead);
            if(!f){ printf("Reassignment cancelled.\n"); return; }
            strcpy(c->facultyID,f->id);
            strcpy(c->facultyName,f->name);
            strcpy(c->department,f->department);
            saveCourses(head);
            printf(" Reassigned %s to %s.\n", code, f->name);
            return;
        }
    }
    printf(" Course not found.\n");
}

/* ---------- Main Menu ---------- */
void courseMenu(void){
    Department *deptHead=loadDepartments();
    loadFaculty(&deptHead);
    Course *head=loadCourses();

    int ch;
    do{
        printf("\n=== COURSE & FACULTY LINKING ===\n");
        printf("1. Add Course\n2. View All Courses\n3. View by Branch\n4. Reassign Faculty\n5. Save & Exit\nChoice: ");
        if(scanf("%d",&ch)!=1){ ch=5; }
        while(getchar()!='\n' && !feof(stdin)){}
        switch(ch){
            case 1: addCourse(&head,deptHead); break;
            case 2: listCourses(head); break;
            case 3: searchByBranch(head); break;
            case 4: reassignFaculty(head,deptHead); break;
            case 5: saveCourses(head); printf("Saved.\n"); break;
            default: printf("Invalid.\n");
        }
    }while(ch!=5);

    while(head){ Course *t=head; head=head->next; free(t); }
    while(deptHead){
        Department *d=deptHead; deptHead=deptHead->next;
        Faculty *f=d->facultyHead; while(f){ Faculty *tmp=f; f=f->next; free(tmp); }
        free(d);
    }
}
