
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[50];
    int age;
    float gpa;
    int rollno;
} Student;

Student* create_student_list(int n) {
    Student *studs = (Student*) malloc(n * sizeof(Student));
    return studs;
}

Student *students;
int count = 0;
int capacity = 0;

int check_input(int i){
    if(strlen(students[i].name) < 0 || strlen(students[i].name) > 49)
      return -1;

    if(students[i].age > 100 || students[i].age < 16)
      return -1;

    if(students[i].gpa < 0.00 || students[i].gpa > 4.00)
      return -1;

    if(students[i].rollno <= 0)
      return -1;

    return 0;
}

void add_student(int i, char *name, int age, float gpa, int rollno) {
    strcpy(students[i].name, name);
    students[i].age = age;
    students[i].gpa = gpa;
    students[i].rollno = rollno;
}

void display_students() {
    for (int i = 0; i < count; i++) {
        printf("Name: %s, Roll Number: %d, Age: %d, GPA: %.2f\n", students[i].name, students[i].rollno, students[i].age, students[i].gpa);
    }
}

int search_students( int rollno) {
    for (int i = 0; i < count; i++) {
        if(students[i].rollno == rollno)
          return i;
    }
    return -1;
}
  
int update_student( int rollno, char *new_name, int new_age, float new_gpa)
{
    int idx = search_students( rollno);
    if (idx == -1)
        return -1;                          

    if (strlen(new_name) > 49)
        return -1;

    Student temp;
    strcpy(temp.name, new_name);
    temp.age    = new_age;
    temp.gpa    = new_gpa;
    temp.rollno = rollno;                   

    if (check_input(idx) == -1)
        return -1;                          

    students[idx] = temp;                      
    return 0;
}

int delete_students( int rollno){
    int idx = search_students(rollno);
    if (idx == -1)
        return -1;                      

    for (int i = idx; i < count - 1; i++)
        students[i] = students[i + 1];        

    (count)--;                         
    return 0;
}

int expand(){
    int new_cap = capacity * 2;
    //if(count == capacity)
    Student *temp = realloc(students, new_cap * sizeof(Student));
    if(temp == NULL)
      return -1;
    
    students = temp;
    capacity = new_cap;
    return 0;
}

void display_summary(){
    if (count == 0) {
        printf("No records.\n");
        return;
    }

    int best = 0, worst = 0;
    float total = 0;

    for (int i = 0; i < count; i++) {
        if (students[i].gpa > students[best].gpa) 
            best = i;
        if (students[i].gpa < students[worst].gpa)
            worst = i;
        total += students[i].gpa;
    }

    printf("Highest: %s %.2f\n", students[best].name, students[best].gpa);
    printf("Lowest : %s %.2f\n", students[worst].name, students[worst].gpa);
    printf("Average: %.2f\n", total / count);
}

void display_sorted(){
Student *copy = malloc(count * sizeof(Student));

for(int i = 0; i < count; i++)
   copy[i] = students[i];

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count - 1; j++) {
            if (copy[j].gpa < copy[j+1].gpa) {
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
            else if (copy[j].gpa == copy[j+1].gpa && copy[j].rollno > copy[j+1].rollno){
                Student temp = copy[j];
                copy[j] = copy[j + 1];
                copy[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < count; i++)
        printf("%d %s %d %.2f\n", copy[i].rollno, copy[i].name, copy[i].age, copy[i].gpa);
}

void free_student_list() {
    free(students);
}

int main() {
    int ch, age, rollno, idx;
    char name[100];
    float gpa;
 
    printf("Enter capacity of the Structure.");
    scanf("%d", &capacity);
    if (capacity <= 0 || capacity > 1000000) {
        printf("Invalid capacity.\n");
        return 0;
    }
    students = create_student_list(capacity);
    if (students == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }
 
    do {
        printf("\n1.Add  2.Display  3.Search  4.Update  5.Delete 6.Sorted by GPA  7.GPA summary  0.Exit\n");
        printf("Enter your choice");
        if (scanf("%d", &ch) != 1)
            break;
 
        switch(ch){
            case 1:
                printf("Enter roll number: ");
                scanf("%d", &rollno);
                printf("Enter name: ");
                scanf(" %99[^\n]", name);
                printf("Enter age: ");
                scanf("%d", &age);
                printf("Enter gpa: ");
                scanf("%f", &gpa);
                if (strlen(name) > 49) {
                    printf("Name is too long.\n");
                }
                else if (search_students(rollno) != -1) {
                    printf("Roll number already exists.\n");
                }
                else {
                    if (count == capacity) {
                        if (expand() == -1) {
                            printf("Could not expand memory.\n");
                            break;
                        }
                    }
                    add_student(count, name, age, gpa, rollno);
                    if (check_input(count) == -1) {
                        printf("Invalid input.\n");
                    }
                    else {
                        count++;
                        printf("Student added.\n");
                    }
                }
                break;
            case 2:
                if (count == 0)
                    printf("No records.\n");
                else
                    display_students();
                break;
            case 3:
                printf("Enter roll number: ");
                scanf("%d", &rollno);
                idx = search_students(rollno);
                if (idx == -1)
                    printf("Roll number not found.\n");
                else
                    printf("Name: %s, Roll Number: %d, Age: %d, GPA: %.2f\n", students[idx].name, students[idx].rollno, students[idx].age, students[idx].gpa);
                break;
            case 4:
                printf("Enter roll number to update: ");
                scanf("%d", &rollno);
                printf("Enter new name: ");
                scanf(" %99[^\n]", name);
                printf("Enter new age: ");
                scanf("%d", &age);
                printf("Enter new gpa: ");
                scanf("%f", &gpa);
                if (strlen(name) > 49 || age < 16 || age > 100 || gpa < 0 || gpa > 4)
                    printf("Invalid input.\n");
                else if (update_student(rollno, name, age, gpa) == -1)
                    printf("Roll number not found.\n");
                else
                    printf("Student updated.\n");
                break;
            case 5:
                printf("Enter roll number to delete: ");
                scanf("%d", &rollno);
                if (delete_students(rollno) == -1)
                    printf("Roll number not found.\n");
                else
                    printf("Student deleted.\n");
                break;
            case 6:
                if (count == 0)
                    printf("No records.\n");
                else
                    display_sorted();
                break;
            case 7:
                display_summary();
                break;
            case 0:
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (ch != 0);
 
    free_student_list();
    printf("Goodbye!\n");
    return 0;
}
