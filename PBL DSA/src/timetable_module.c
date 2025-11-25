#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"

#ifdef _WIN32
#define MKDIR_CMD "if not exist data mkdir data >nul 2>nul"
#else
#define MKDIR_CMD "mkdir -p data >/dev/null 2>&1"
#endif

static void ensure_data_dir(void) {
    system(MKDIR_CMD);
}

/* ---------- Utility Functions ---------- */

static const char* dayName(int d) {
    static const char* names[DAYS] = {"Mon","Tue","Wed","Thu","Fri","Sat"};
    return (d >= 0 && d < DAYS) ? names[d] : "?";
}

static void makeFilePath(const Timetable *tt, char *out, size_t n) {
    snprintf(out, n, "data/timetable_%s_%d_%s.txt", tt->branch, tt->semester, tt->section);
}

 void initTimetable(Timetable *tt) {
    for (int d = 0; d < DAYS; ++d)
        for (int s = 0; s < SLOTS; ++s) {
            TTCell *c = &tt->grid[d][s];
            c->occupied = 0;
            c->code[0] = c->subject[0] = c->faculty[0] = c->room[0] = c->group[0] = c->notes[0] = '\0';
        }
}

/* ---------- File I/O ---------- */

static int loadTimetable(Timetable *tt) {
    ensure_data_dir();
    char path[128]; makeFilePath(tt, path, sizeof(path));
    FILE *fp = fopen(path, "r");
    if (!fp) return 0;
    initTimetable(tt);

    int d, s, occ;
    TTCell cell;
    while (fscanf(fp, "%d %d %d %9s %49s %49s %19s %7s %49s",
                  &d, &s, &occ, cell.code, cell.subject,
                  cell.faculty, cell.room, cell.group, cell.notes) == 9)
    {
        if (d >= 0 && d < DAYS && s >= 0 && s < SLOTS)
            tt->grid[d][s] = cell;
    }
    fclose(fp);
    return 1;
}

static void saveTimetable(const Timetable *tt) {
    ensure_data_dir();
    char path[128]; makeFilePath(tt, path, sizeof(path));
    FILE *fp = fopen(path, "w");
    if (!fp) { printf("Error writing %s\n", path); return; }

    for (int d = 0; d < DAYS; ++d)
        for (int s = 0; s < SLOTS; ++s) {
            TTCell *c = (TTCell*)&tt->grid[d][s];
            fprintf(fp, "%d %d %d %s %s %s %s %s %s\n",
                    d, s, c->occupied,
                    c->code[0]?c->code:"-",
                    c->subject[0]?c->subject:"-",
                    c->faculty[0]?c->faculty:"-",
                    c->room[0]?c->room:"-",
                    c->group[0]?c->group:"-",
                    c->notes[0]?c->notes:"-");
        }
    fclose(fp);
    printf(" Saved timetable to %s\n", path);
}

/* ---------- Display ---------- */

static void printTimetable(const Timetable *tt) {
    printf("\nTimetable for %s Sem-%d %s\n", tt->branch, tt->semester, tt->section);
    printf("Day |");
    for (int s = 0; s < SLOTS; ++s) printf(" P%-2d |", s);
    printf("\n--------------------------------------------------------------\n");

    for (int d = 0; d < DAYS; ++d) {
        printf("%-3s |", dayName(d));
        for (int s = 0; s < SLOTS; ++s) {
            TTCell c = tt->grid[d][s];
            if (!c.occupied) printf("  --  |");
            else printf(" %s |", c.code);
        }
        printf("\n");
    }
}

/* ---------- Modify Slots ---------- */

static void assignSlot(Timetable *tt) {
    int d, s;
    printf("Enter Day (0=Mon..5=Sat): "); scanf("%d", &d);
    printf("Enter Slot (0..%d): ", SLOTS - 1); scanf("%d", &s);
    if (d<0 || d>=DAYS || s<0 || s>=SLOTS) { printf("Invalid slot.\n"); return; }

    TTCell *c = &tt->grid[d][s];
    printf("Subject Code: "); scanf("%9s", c->code);
    printf("Subject Name (use _ for spaces): "); scanf("%49s", c->subject);
    printf("Faculty Name (use _ for spaces): "); scanf("%49s", c->faculty);
    printf("Room: "); scanf("%19s", c->room);
    printf("Group (G1/G2/Both/-): "); scanf("%7s", c->group);
    printf("Notes (or -): "); scanf("%49s", c->notes);
    c->occupied = 1;
    printf(" Slot assigned.\n");
}

static void clearSlot(Timetable *tt) {
    int d, s;
    printf("Enter Day (0..5) and Slot (0..%d): ", SLOTS - 1);
    scanf("%d %d", &d, &s);
    if (d<0 || d>=DAYS || s<0 || s>=SLOTS) { printf("Invalid slot.\n"); return; }
    TTCell *c = &tt->grid[d][s];
    c->occupied = 0;
    printf("Slot cleared.\n");
}

/* ---------- Menu ---------- */

void timetableMenu(void) {
    Timetable tt;
    printf("\n=== TIMETABLE MANAGEMENT ===\n");
    printf("Enter Branch (e.g., BTECH_CSE): "); scanf("%19s", tt.branch);
    printf("Enter Semester: "); scanf("%d", &tt.semester);
    printf("Enter Section (e.g., A1, B2, ML-1): "); scanf("%19s", tt.section);

    if (!loadTimetable(&tt)) {
        printf("Creating new timetable...\n");
        initTimetable(&tt);
    }

    int ch;
    do {
        printf("\n1. View Timetable\n2. Assign Slot\n3. Clear Slot\n4. Save & Exit\nChoice: ");
        scanf("%d", &ch);
        while (getchar() != '\n');
        switch (ch) {
            case 1: printTimetable(&tt); break;
            case 2: assignSlot(&tt); break;
            case 3: clearSlot(&tt); break;
            case 4: saveTimetable(&tt); break;
            default: printf("Invalid choice.\n");
        }
    } while (ch != 4);
}

void viewFacultyTimetable(const char *facultyName) {
    printf("\n--- Timetable for %s ---\n", facultyName);


    FILE *fp;
    char filename[256];
    char buffer[256];

    for (int sem = 1; sem <= 8; sem++) {
        for (char sec = 'A'; sec <= 'G'; sec++) {
            snprintf(filename, sizeof(filename), "data/timetable_BTECH_CSE_%d_%c1.txt", sem, sec);
            fp = fopen(filename, "r");
            if (!fp) continue;

            int found = 0;
            while (fgets(buffer, sizeof(buffer), fp)) {
                if (strstr(buffer, facultyName)) {
                    if (!found) {
                        printf("\n Found in %s\n", filename);
                        found = 1;
                    }
                    printf("%s", buffer);
                }
            }
            fclose(fp);
        }
    }
}
