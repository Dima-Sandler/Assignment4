#define _CRT_SECURE_NO_WARNINGS
/*#define _crtdbg_map_alloc
#include <stdlib.h>
#include <crtdbg.h>*/ // uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
} StudentCourseGrade;

typedef struct Student
{
	char name[35];
	int numberOfCourses;
	StudentCourseGrade* grades; // dynamic array of courses
} Student;


// Part A
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount, char terminator);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);

// Part B
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);

int main()
{
	// Part A
	int numberOfStudents = 0;
	int* coursesPerStudent = NULL;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	// studentsToFile(students, coursesPerStudent, numberOfStudents); // this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
	
	// Part B
	Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");

	// add code to free all arrays of struct Student

	/*_CrtDumpMemoryLeaks();*/ // uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	//add code here
}
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	//add code here
}
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	//add code here
}
int countPipes(const char* lineBuffer, int maxCount, char terminator)
{
	//add code here
}
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	//add code here
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	//add code here
}
Student* readFromBinFile(const char* fileName)
{
	//add code here
}
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	if (students == NULL || coursesPerStudent == NULL) {
		return NULL;
	}
	Student* arr = (Student*)malloc(numberOfStudents*sizeof(Student));
	if (!arr) {
		printf("allocation failed");
		return NULL;
	}
	
	for (int i = 0; i < numberOfStudents; i++) {
		strcpy(arr[i].name, students[i][0]);
		arr[i].grades = (StudentCourseGrade*)malloc(coursesPerStudent[i] * sizeof(StudentCourseGrade));
		if (!arr[i].grades) {
				printf("allocation failed");
				return NULL;
			}
		for (int j = 1; j < coursesPerStudent[i]-1; j+=2){
			strcpy(arr[i].grades[j].courseName, students[i][j]);
			arr[i].grades[j].grade = atoi(students[i][j + 1]);
		}
		arr[i].numberOfCourses = coursesPerStudent[i];
	}
	return arr;
}
