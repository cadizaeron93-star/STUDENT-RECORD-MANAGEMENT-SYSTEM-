#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


struct Student {
    int student_id;
    char name[50];
    char course[30];
    int year_level;
    float gpa;
    struct Student *next;
};


struct StackNode {
    char action[10];
    struct Student data;
    struct StackNode *next;
};


struct QueueNode {
    char action[20];
    int student_id;
    char student_name[50];
    char timestamp[30];
    struct QueueNode *next;
};

struct Student    *head       = NULL;
struct StackNode  *stackTop   = NULL;
struct QueueNode  *queueFront = NULL;
struct QueueNode  *queueRear  = NULL;
int                nextID     = 1001;


#define HT_SIZE 128
struct Student *hashTable[HT_SIZE];


void addStudent();
void updateStudent();
void deleteStudent();
void sortRecords();
void searchStudents();
void undo();
void viewAll();
void viewHistory();


int hashFunc(int id) {
    return id % HT_SIZE;
}
void htInsert(struct Student *s) {
    int idx = hashFunc(s->student_id);
    while (hashTable[idx] != NULL && hashTable[idx]->student_id != s->student_id)
        idx = (idx + 1) % HT_SIZE;
    hashTable[idx] = s;
}
struct Student *htSearch(int id) {
    int idx = hashFunc(id);
    int start = idx;
    while (hashTable[idx] != NULL) {
        if (hashTable[idx]->student_id == id) return hashTable[idx];
        idx = (idx + 1) % HT_SIZE;
        if (idx == start) break;
    }
    return NULL;
}
void htRemove(int id) {
    int idx = hashFunc(id);
    int start = idx;
    while (hashTable[idx] != NULL) {
        if (hashTable[idx]->student_id == id) { hashTable[idx] = NULL; return; }
        idx = (idx + 1) % HT_SIZE;
        if (idx == start) break;
    }
}


void stackPush(const char *action, struct Student *s) {
    struct StackNode *node = (struct StackNode *)malloc(sizeof(struct StackNode));
    strcpy(node->action, action);
    node->data = *s;
    node->next = stackTop;
    stackTop = node;
}
struct StackNode *stackPop() {
    if (stackTop == NULL) return NULL;
    struct StackNode *top = stackTop;
    stackTop = stackTop->next;
    return top;
}


void enqueue(const char *action, int id, const char *name) {
    struct QueueNode *node = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    strcpy(node->action, action);
    node->student_id = id;
    strcpy(node->student_name, name);
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(node->timestamp, 30, "%Y-%m-%d %H:%M:%S", tm_info);
    node->next = NULL;
    if (queueRear == NULL) { queueFront = queueRear = node; }
    else { queueRear->next = node; queueRear = node; }
}


void printDivider() {
    printf("------------------------------------------------\n");
}
void printStudent(struct Student *s) {
    printDivider();
    printf(" ID         : %d\n",   s->student_id);
    printf(" Name       : %s\n",   s->name);
    printf(" Course     : %s\n",   s->course);
    printf(" Year Level : %d\n",   s->year_level);
    printf(" GPA        : %.2f\n", s->gpa);
    printDivider();
}


int main() {
	memset(hashTable, 0, sizeof(hashTable));
	int menu;

	do {
		system(CLEAR);

		printf ("\n****** STUDENT RECORD MANAGEMENT SYSTEM ******");
		printf ("\nMAIN MENU:\n");
		printf ("1. Add Student\n");
		printf ("2. Update Student\n");
		printf ("3. Delete Student\n");
		printf ("4. Sort Records\n");
		printf ("5. Search Student\n");
		printf ("6. Undo Last Action\n");
		printf ("7. View all Students\n");
		printf ("8. View History\n");
		printf ("9. Exit\n");

		printf ("Select your transaction: ");
		scanf ("%d", &menu);


		switch (menu){
			case 1:
				addStudent ();
				break;
			case 2:
				updateStudent ();
				break;
			case 3:
				deleteStudent ();
				break;
			case 4:
				sortRecords ();
				break;
			case 5:
				searchStudents ();
				break;
			case 6:
				undo ();
				break;
			case 7:
				viewAll ();
				break;
			case 8:
				viewHistory ();
				break;
			case 9:
				printf ("Exiting .... Thank you!");
				break;
			default:
				printf ("Invalid Input! Please try again!");
		}

		if (menu != 9) {
			printf ("\nPress Enter to return to menu...");
			getchar (); getchar ();
		}

	} while (menu != 9);

	return 0;
}

	void addStudent (){
		struct Student *s = (struct Student *)malloc(sizeof(struct Student));
		s->student_id = nextID++;
		s->next = NULL;

		printf ("\n[ ADD STUDENT ]\n");
		printDivider ();
		printf ("Name       : "); scanf (" %[^\n]", s->name);
		printf ("Course     : "); scanf (" %[^\n]", s->course);
		printf ("Year Level : "); scanf ("%d", &s->year_level);
		printf ("GPA        : "); scanf ("%f", &s->gpa);

	
		s->next = head;
		head = s;

		
		htInsert (s);

	
		stackPush ("ADD", s);

	
		enqueue ("ADD", s->student_id, s->name);

		printf ("\n[OK] Student added! ID assigned: %d\n", s->student_id);
	}

	void updateStudent (){
		int id;
		printf ("\n[ UPDATE STUDENT ]\n");
		printDivider ();
		printf ("Enter Student ID to update: ");
		scanf ("%d", &id);

		
		struct Student *s = htSearch (id);
		if (!s) { printf ("[!] Student ID %d not found.\n", id); return; }

		stackPush ("UPDATE", s);

		printf ("New Name        [%s]: ", s->name);       scanf (" %[^\n]", s->name);
		printf ("New Course      [%s]: ", s->course);     scanf (" %[^\n]", s->course);
		printf ("New Year Level  [%d]: ", s->year_level); scanf ("%d", &s->year_level);
		printf ("New GPA         [%.2f]: ", s->gpa);      scanf ("%f", &s->gpa);

		enqueue ("UPDATE", s->student_id, s->name);
		printf ("\n[OK] Student %d updated.\n", id);
	}

	void deleteStudent (){
		int id;
		printf ("\n[ DELETE STUDENT ]\n");
		printDivider ();
		printf ("Enter Student ID to delete: ");
		scanf ("%d", &id);

		struct Student *s = htSearch (id);
		if (!s) { printf ("[!] Student ID %d not found.\n", id); return; }

		
		stackPush ("DELETE", s);
		enqueue ("DELETE", s->student_id, s->name);

		
		struct Student *curr = head, *prev = NULL;
		while (curr) {
			if (curr->student_id == id) {
				if (prev) prev->next = curr->next;
				else head = curr->next;
				htRemove (id);
				free (curr);
				printf ("\n[OK] Student %d deleted.\n", id);
				return;
			}
			prev = curr;
			curr = curr->next;
		}
	}

	void sortRecords (){
		if (!head) { printf ("\n[!] No records to sort.\n"); return; }

		int choice;
		printf ("\n[ SORT RECORDS ]\n");
		printDivider ();
		printf ("Sort by:\n");
		printf (" 1. Name\n");
		printf (" 2. GPA\n");
		printf (" 3. Student ID\n");
		printf ("Choice: ");
		scanf ("%d", &choice);
		if (choice < 1 || choice > 3) { printf ("[!] Invalid choice.\n"); return; }

	
		int swapped;
		struct Student *ptr, *last = NULL;
		do {
			swapped = 0;
			ptr = head;
			while (ptr->next != last) {
				int doSwap = 0;
				if (choice == 1) doSwap = strcmp (ptr->name, ptr->next->name) > 0;
				else if (choice == 2) doSwap = ptr->gpa > ptr->next->gpa;
				else doSwap = ptr->student_id > ptr->next->student_id;

				if (doSwap) {
					
					int   tmpID = ptr->student_id;
					char  tmpN[50], tmpC[30];
					int   tmpY  = ptr->year_level;
					float tmpG  = ptr->gpa;
					strcpy (tmpN, ptr->name);
					strcpy (tmpC, ptr->course);

					ptr->student_id = ptr->next->student_id;
					strcpy (ptr->name,   ptr->next->name);
					strcpy (ptr->course, ptr->next->course);
					ptr->year_level = ptr->next->year_level;
					ptr->gpa        = ptr->next->gpa;

					ptr->next->student_id = tmpID;
					strcpy (ptr->next->name,   tmpN);
					strcpy (ptr->next->course, tmpC);
					ptr->next->year_level = tmpY;
					ptr->next->gpa        = tmpG;

					swapped = 1;
				}
				ptr = ptr->next;
			}
			last = ptr;
		} while (swapped);

		
		memset (hashTable, 0, sizeof(hashTable));
		struct Student *s = head;
		while (s) { htInsert (s); s = s->next; }

		enqueue ("SORT", 0, "all records");
		printf ("\n[OK] Records sorted.\n");
	}

	void searchStudents (){
		int choice;
		printf ("\n[ SEARCH STUDENT ]\n");
		printDivider ();
		printf ("Search by:\n");
		printf (" 1. Student ID\n");
		printf (" 2. Name\n");
		printf ("Choice: ");
		scanf ("%d", &choice);

		struct Student *result = NULL;

		if (choice == 1) {
			int id;
			printf ("Enter Student ID: ");
			scanf ("%d", &id);
			result = htSearch (id);    
			enqueue ("SEARCH-ID", id, "");
		} else if (choice == 2) {
			char name[50];
			printf ("Enter Name: ");
			scanf (" %[^\n]", name);
			/* linear search by name */
			struct Student *curr = head;
			while (curr) {
				if (strcmp (curr->name, name) == 0) { result = curr; break; }
				curr = curr->next;
			}
			enqueue ("SEARCH-NAME", 0, name);
		} else {
			printf ("[!] Invalid choice.\n");
			return;
		}

		if (result) {
			printf ("\n[FOUND]\n");
			printStudent (result);
		} else {
			printf ("\n[!] Student not found.\n");
		}
	}

	void undo (){
		struct StackNode *top = stackPop ();
		if (!top) { printf ("\n[!] Nothing to undo.\n"); return; }

		printf ("\n[ UNDO ] Reversing: %s on ID %d (%s)\n",
				top->action, top->data.student_id, top->data.name);

		if (strcmp (top->action, "ADD") == 0) {
			
			struct Student *curr = head, *prev = NULL;
			while (curr) {
				if (curr->student_id == top->data.student_id) {
					if (prev) prev->next = curr->next;
					else head = curr->next;
					htRemove (curr->student_id);
					free (curr);
					break;
				}
				prev = curr;
				curr = curr->next;
			}
			printf ("[OK] Undo ADD — student removed.\n");

		} else if (strcmp (top->action, "DELETE") == 0) {
			
			struct Student *s = (struct Student *)malloc(sizeof(struct Student));
			*s = top->data;
			s->next = head;
			head = s;
			htInsert (s);
			printf ("[OK] Undo DELETE — student restored.\n");

		} else if (strcmp (top->action, "UPDATE") == 0) {
			
			struct Student *s = htSearch (top->data.student_id);
			if (s) {
				strcpy (s->name,   top->data.name);
				strcpy (s->course, top->data.course);
				s->year_level = top->data.year_level;
				s->gpa        = top->data.gpa;
				printf ("[OK] Undo UPDATE — previous data restored.\n");
			}
		}

		enqueue ("UNDO", top->data.student_id, top->data.name);
		free (top);
	}

	void viewAll (){
		if (!head) { printf ("\n[!] No student records found.\n"); return; }

		printf ("\n[ ALL STUDENTS ]\n");
		printf ("%-8s %-25s %-15s %-6s %-5s\n",
				"ID", "Name", "Course", "Year", "GPA");
		printDivider ();

		struct Student *curr = head;
		int count = 0;
		while (curr) {
			printf ("%-8d %-25s %-15s %-6d %.2f\n",
					curr->student_id, curr->name,
					curr->course, curr->year_level, curr->gpa);
			curr = curr->next;
			count++;
		}
		printDivider ();
		printf ("Total records: %d\n", count);
	}

	void viewHistory (){
		if (!queueFront) { printf ("\n[!] History is empty.\n"); return; }

		printf ("\n[ OPERATION HISTORY ]\n");
		printf ("%-20s %-8s %-25s %-20s\n",
				"Action", "ID", "Student", "Timestamp");
		printDivider ();

		struct QueueNode *curr = queueFront;
		while (curr) {
			printf ("%-20s %-8d %-25s %-20s\n",
					curr->action, curr->student_id,
					curr->student_name, curr->timestamp);
			curr = curr->next;
		}
		printDivider ();
	}
