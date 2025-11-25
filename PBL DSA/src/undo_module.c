#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C:\\Users\\athar\\Desktop\\PBL DSA\\include\\headers.h.c"

/* ================================================================
   MODULE 5 – UNDO / REDO SYSTEM (FINAL VERSION)
   ================================================================ */

#define MAX_HISTORY 50

// Global stacks so history is not lost between menu calls
static Stack undoStack;
static Stack redoStack;

/* -------------------- Stack Utility Functions -------------------- */
void initStack(Stack *s) { s->top = -1; }
int isEmpty(Stack *s) { return s->top == -1; }
int isFull(Stack *s) { return s->top == MAX_HISTORY - 1; }

/* Pushes a snapshot (timetable state) to stack */
void push(Stack *s, Timetable *t) {
    if (isFull(s)) {
        // shift all states up (remove oldest)
        for (int i = 1; i <= s->top; i++)
            s->stack[i - 1] = s->stack[i];
        s->top--;
    }
    s->stack[++(s->top)].state = *t;
}

/* Pops top snapshot into out */
int pop(Stack *s, Timetable *out) {
    if (isEmpty(s)) return 0;
    *out = s->stack[(s->top)--].state;
    return 1;
}

/* Shows compact timetable info for debug */
static void printMini(const Timetable *tt) {
    printf("Snapshot: %s Sem-%d %s\n", tt->branch, tt->semester, tt->section);
}

/* ================================================================
   INITIALIZE STACKS ONCE (CALL AT STARTUP)
   ================================================================ */
void initUndoRedo(void) {
    initStack(&undoStack);
    initStack(&redoStack);
}

/* ================================================================
   PUSH CURRENT STATE BEFORE ANY CHANGE
   ================================================================ */
void recordTimetableSnapshot(Timetable *current) {
    push(&undoStack, current);
    redoStack.top = -1; // new change clears redo history
}

/* ================================================================
   UNDO / REDO IMPLEMENTATION
   ================================================================ */
void performUndo(Timetable *current) {
    if (isEmpty(&undoStack)) {
        printf("⚠️  Nothing to undo.\n");
        return;
    }

    Timetable prev;
    if (pop(&undoStack, &prev)) {
        push(&redoStack, current);
        *current = prev;
        printf(" Undo successful.\n");
    }
}

void performRedo(Timetable *current) {
    if (isEmpty(&redoStack)) {
        printf("  Nothing to redo.\n");
        return;
    }

    Timetable next;
    if (pop(&redoStack, &next)) {
        push(&undoStack, current);
        *current = next;
        printf(" Redo successful.\n");
    }
}

/* ================================================================
   ADMIN MENU FOR UNDO / REDO
   ================================================================ */
void undoMenu(void) {
    Timetable current;
    strcpy(current.branch, "BTECH_CSE");
    current.semester = 3;
    strcpy(current.section, "A1");
    initTimetable(&current);

    int ch;
    do {
        printf("\n=== UNDO / REDO MENU ===\n");
        printf("1. Make Change (Simulate)\n");
        printf("2. Undo Last Change\n");
        printf("3. Redo Last Change\n");
        printf("4. Show Stack Status\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &ch);
        while (getchar() != '\n');

        switch (ch) {
            case 1:
                printf("Enter subject code for [Day0][Slot0]: ");
                scanf("%9s", current.grid[0][0].code);
                current.grid[0][0].occupied = 1;
                recordTimetableSnapshot(&current);
                printf("Change recorded in Undo history.\n");
                break;

            case 2:
                performUndo(&current);
                break;

            case 3:
                performRedo(&current);
                break;

            case 4:
                printf("Undo top: %d | Redo top: %d\n", undoStack.top, redoStack.top);
                printMini(&current);
                break;

            case 5:
                printf("Exiting Undo/Redo Module...\n");
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (ch != 5);
}
