#define _CRT_SECURE_NO_WARNINGS
#define maxLen 1023
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
	countStudentsAndCourses("studentList.txt", &coursesPerStudent, &numberOfStudents);
	
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	/* studentsToFile(students, coursesPerStudent, numberOfStudents);*/ // this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
	
	// Part B
	/*Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");*/

	// add code to free all arrays of struct Student

	/*_CrtDumpMemoryLeaks();*/ // uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

// Part A
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
	char line[maxLen];
	FILE* pFile = fopen(fileName, "r");

	if (!pFile) // check if the file opened successfully
	{ 
		puts("Failed to open file");
		exit(1);
	}
	
	*numberOfStudents = 0;
	
	// count the number of lines/students in the file
	while (!feof(pFile)) // stop when EOF is reached
		if (fgets(line, maxLen, pFile)) // check if the line is read
			(*numberOfStudents)++;

	rewind(pFile);

	// create a dynamic array at the referenced location
	*coursesPerStudent = (int*)malloc(*numberOfStudents * sizeof(int));

	if (!*coursesPerStudent) // malloc check 
	{
		puts("Memory allocation error");
		exit(1);
	}
	
	int* ptr = *coursesPerStudent;
	
	// count and save the number of pipes/courses in every line
	while (!feof(pFile)) // stop when EOF is reached
		if (fgets(line, maxLen, pFile)) // check if the line is read
			*ptr++ = countPipes(line, maxLen, 0);	
	
	fclose(pFile);	
}
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	//add code here
}
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	//add code here
}
int countPipes(const char* lineBuffer, int maxCount, char terminator) // terminator is an unused parameter
{	
	if (!lineBuffer) // check if lineBuffer is a null pointer
		return -1;

	int i, count = 0;

	// iterate over the string until the null character is read or maxCount characters are read
	// if maxCount is nonpositive the loop is not executed
	for (i = 0; i < maxCount && lineBuffer[i] != 0; i++)
		if (lineBuffer[i] == '|') // check if the character matches
			count++;
			
	return count;
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
	//add code here
}
