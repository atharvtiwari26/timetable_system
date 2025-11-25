#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"
void detectConflicts(const char *filename); /* Graph module (option 8) */
void treeMenu(const char *filename);        /* Tree module (option 9) */

/* =================== Configuration =================== */
#define USERS_FILE "data/users.txt"
#define SHIFT      2   /* Caesar +2 shift for simple obfuscation */

/* ===== External module functions (link-ready hooks) ===== */
void deptFacultyMenu(void);                 /* Module 2A */
void courseMenu(void);                      /* Module 2B */
void timetableMenu(void);                   /* Module 4: Timetable core */
void reportMenu(const char *role);	    /* Module 6+: Reports/Extra class */
void undoMenu(void);                        /* Module 5: Undo/Redo (stacks) */
void viewFacultyTimetable(const char *id);  /* Faculty timetable viewer (read-only) */

/* Optional: if your Undo module exposes this, it's safe to call once at startup. */
void initUndoRedo(void);

/* ========== Helper: data dir ========== */
static void ensure_data_dir(void) {
#ifdef _WIN32
    system("if not exist data mkdir data >nul 2>nul");
#else
    system("mkdir -p data >/dev/null 2>&1");
#endif
}

static int file_exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

/* ========== Helper: input flush ========== */
static void flush_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { /* discard */ }
}

/* ========== Encoding Functions ========== */
static void encode(const char *in, char *out) {
    while (*in) *out++ = (char)(*in++ + SHIFT);
    *out = '\0';
}

static void decode(const char *in, char *out) {
    while (*in) *out++ = (char)(*in++ - SHIFT);
    *out = '\0';
}

/* ========== File Handling ========== */
static int load_users(User *arr, int maxn) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return 0;
    int n = 0;
    while (n < maxn) {
        User u;
        if (fscanf(fp, "%31s %63s %9s", u.id, u.enc_pw, u.role) != 3) break;
        arr[n++] = u;
    }
    fclose(fp);
    return n;
}

static int append_user(const User *u) {
    FILE *fp = fopen(USERS_FILE, "a");
    if (!fp) return 0;
    fprintf(fp, "%s %s %s\n", u->id, u->enc_pw, u->role);
    fclose(fp);
    return 1;
}

/* ========== Admin Bootstrap (First Run) ========== */
static void bootstrap_admin_if_needed(void) {
    if (file_exists(USERS_FILE)) return;
    ensure_data_dir();
    FILE *fp = fopen(USERS_FILE, "w");
    if (!fp) return;
    char enc[MAX_PW];
    encode("admin123", enc);
    fprintf(fp, "admin %s admin\n", enc);
    fclose(fp);
    printf(" Created default admin: ID = admin, Password = admin123\n");
}

/* ========== Search & Validation ========== */
static int find_user_idx(const User *arr, int n, const char *id) {
    for (int i = 0; i < n; i++)
        if (strcmp(arr[i].id, id) == 0) return i;
    return -1;
}

static int creds_ok(const User *arr, int n, const char *id, const char *pw) {
    int idx = find_user_idx(arr, n, id);
    if (idx < 0) return 0;
    char decoded[MAX_PW];
    decode(arr[idx].enc_pw, decoded);
    return strcmp(decoded, pw) == 0;
}

/* ========== Admin ops ========== */
static void show_all_users(const User *arr, int n) {
    printf("\n%-15s %-10s\n", "UserID", "Role");
    printf("--------------------------\n");
    for (int i = 0; i < n; i++)
        printf("%-15s %-10s\n", arr[i].id, arr[i].role);
}

static void admin_add_user(User *arr, int *n) {
    if (*n >= MAX_USERS) { printf(" User limit reached.\n"); return; }

    User u; char pw[MAX_PW];
    printf("Enter role (faculty/student): ");
    if (scanf("%9s", u.role) != 1) { printf("Invalid input.\n"); flush_line(); return; }
    flush_line();

    if (strcmp(u.role, "faculty") && strcmp(u.role, "student")) {
        printf(" Only 'faculty' or 'student' allowed.\n");
        return;
    }

    printf("Enter new user ID: ");
    if (scanf("%31s", u.id) != 1) { printf("Invalid input.\n"); flush_line(); return; }
    flush_line();

    if (find_user_idx(arr, *n, u.id) >= 0) {
        printf(" ID already exists.\n");
        return;
    }

    printf("Enter password: ");
    if (scanf("%63s", pw) != 1) { printf("Invalid input.\n"); flush_line(); return; }
    flush_line();

    encode(pw, u.enc_pw);

    if (!append_user(&u)) {
        printf(" Failed to write user file.\n");
        return;
    }
    arr[(*n)++] = u;
    printf(" Added %s account for '%s'\n", u.role, u.id);
}

/* ---------------- ADMIN MENU (FINAL) ---------------- */
static void admin_menu(void) {
    User users[MAX_USERS];
    int n = load_users(users, MAX_USERS);

    int ch;
    do {
        printf("\n=========== ADMIN MENU ===========\n");
        printf("1. Add User (Faculty/Student)\n");
        printf("2. View All Users\n");
        printf("3. Department & Faculty Management (Module 2A)\n");
        printf("4. Course & Faculty Linking      (Module 2B)\n");
        printf("5. Timetable Management          (Module 4)\n");
        printf("6. Reports & Extra Classes       (Module 6+)\n");
        printf("7. Undo / Redo (Timetable)       (Module 5)\n");
	printf("8. Timetable Conflict / Idle Slot Detection (Graph)\n");
	printf("9. Timetable Search & Organization (Tree)\n");
	printf("10. Logout\n");


        printf("=================================\n");
        printf("Enter choice: ");
        if (scanf("%d", &ch) != 1) { ch = 0; }
        flush_line();

        switch (ch) {
            case 1: admin_add_user(users, &n); break;
            case 2: show_all_users(users, n); break;
            case 3: deptFacultyMenu(); break;   /* Linked List module */
            case 4: courseMenu(); break;        /* Course mapping module */
            case 5: timetableMenu(); break;     /* Timetable grid module */
            case 6: reportMenu("admin"); break;        /* Reports & extra classes (queue) */
            case 7: undoMenu(); break;          /* Stack-based undo/redo */
	    case 8:
    {
        char file[200];
        printf("Enter timetable filename (e.g., data/timetable_xxx.txt): ");
        scanf("%s", file);
        detectConflicts(file);
    }
    break;

    case 9:
    {
        char file[200];
        printf("Enter timetable filename (e.g., data/timetable_xxx.txt): ");
        scanf("%s", file);
        treeMenu(file);
    }
    break;

	    case 10: printf("Logging out...\n"); return;
            default: printf("Invalid choice.\n"); break;
        }
    } while (1);
}

/* ---------------- FACULTY MENU ---------------- */
static void faculty_menu(const User *u) {
    int choice;
    do {
        printf("\n=== FACULTY MENU ===\n");
        printf("1. View My Timetable\n");
        printf("2. Request Extra Class\n");
        printf("3. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                viewFacultyTimetable(u->id);
                break;
            case 2:
                reportMenu("faculty");  // Restricted faculty mode
                break;
            case 3:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 3);
}

/* ---------------- STUDENT MENU ---------------- */
 void student_menu(const User *u) {
    (void)u; /* currently unused, kept for future per-student mapping */
    int choice;
    do {
        printf("\n=== STUDENT MENU ===\n");
        printf("1. View My Section Timetable\n");
        printf("2. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { choice = 0; }
        flush_line();

        switch (choice) {
            case 1: {
                char section[16], branch[32] = "BTECH_CSE";
                int semester = 3;
                printf("Enter your section (e.g., A1, A2, ML-1): ");
                if (scanf("%15s", section) != 1) { printf("Invalid input.\n"); flush_line(); break; }
                flush_line();

                char path[256];
                snprintf(path, sizeof(path), "data/timetable_%s_%d_%s.txt", branch, semester, section);

                FILE *fp = fopen(path, "r");
                if (!fp) {
                    printf(" Timetable not found for %s Sem-%d Section %s\n", branch, semester, section);
                    break;
                }

                printf("\n--- Timetable for %s Sem-%d Section %s ---\n", branch, semester, section);
                char line[512];
                while (fgets(line, sizeof(line), fp)) fputs(line, stdout);
                fclose(fp);
                break;
            }
            case 2:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice.\n");
        }
    } while (1);
}

/* ========== Main Entry for User System ========== */
void runUserSystem(void) {
    ensure_data_dir();
    bootstrap_admin_if_needed();
    if (initUndoRedo) { /* safe if provided */
        initUndoRedo();
    }

    User users[MAX_USERS];
    int user_count = load_users(users, MAX_USERS);

    int choice;
    do {
        printf("\n==============================\n");
        printf("  GEHU Timetable - Login Menu \n");
        printf("==============================\n");
        printf("1. Login\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) { choice = 0; }
        flush_line();

        if (choice == 1) {
            char id[MAX_ID], pw[MAX_PW];
            printf("User ID: ");
            if (scanf("%31s", id) != 1) { printf("Invalid input.\n"); flush_line(); continue; }
            flush_line();

            printf("Password: ");
            if (scanf("%63s", pw) != 1) { printf("Invalid input.\n"); flush_line(); continue; }
            flush_line();

            if (!creds_ok(users, user_count, id, pw)) {
                printf(" Invalid credentials.\n");
                continue;
            }

            int idx = find_user_idx(users, user_count, id);
            if (idx < 0) { printf(" Internal error.\n"); continue; }
            User me = users[idx];

            if (strcmp(me.role, "admin") == 0)
                admin_menu();
            else if (strcmp(me.role, "faculty") == 0)
                faculty_menu(&me);
            else if (strcmp(me.role, "student") == 0)
                student_menu(&me);
            else
                printf(" Unknown role for this account.\n");
        }
        else if (choice != 2) {
            printf("Invalid choice.\n");
        }
    } while (choice != 2);

    printf("Goodbye!\n");
}
