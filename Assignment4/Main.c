#define _CRT_SECURE_NO_WARNINGS
#define open 0
#define close 1
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

// auxiliary functions
void fcheck(FILE*, _Bool);
void rtoa(char*, char**);
void* xmalloc(unsigned int);

// Part A
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
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

// auxiliary functions
void fcheck(FILE* pFile, _Bool op)
{
	// check if pFile is a null pointer
	if (!pFile)	
	{		
		if (op == open)
			perror("Failed to open file");
		else
			perror("Failed to close file");
	
		exit(1);
	}
}
void rtoa(char* rec, char** arr)
{
	char* token, * delim = "|,";

	// read the first token i.e. the student name
	token = strtok(rec, delim);

	// continue to read tokens until the last one
	do
	{
		// allocate memory for the token + NUL
		*arr = (char*)xmalloc(strlen(token) + 1);
		
		// copy the token to the array of strings
		// increment the array pointer afterwards
		strcpy(*arr++, token); 	
	} while (token = strtok(NULL, delim));
}
void* xmalloc(unsigned int size)
{
	void* ptr = (void*)malloc(size);

	// check if malloc succeeded
	if (ptr)
		return ptr;
	else
	{
		perror("Memory allocation error");
		exit(1);
	}
}

// Part A
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n", students[i][0]);
		
		// improved heading
		for (int j = 0; j < 6 + strlen(students[i][0]); j++)
			putchar('*');
		putchar('\n');
		
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n\n", students[i][j + 1]);
		}
	}
}
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	char line[maxLen];
	
	FILE* pFile = fopen(fileName, "r");
	fcheck(pFile, open);

	*numberOfStudents = 0;
	
	// count the number of lines/students in the file
	while (!feof(pFile)) // stop when EOF is reached
		if (fgets(line, maxLen, pFile)) // read a line and check if it is read
			(*numberOfStudents)++;

	rewind(pFile);

	// create a dynamic array at the referenced location
	*coursesPerStudent = (int*)xmalloc(*numberOfStudents * sizeof(int));
	
	int* ptr = *coursesPerStudent;
	
	// count and save the number of pipes/courses in every line
	while (!feof(pFile)) // stop when EOF is reached
		if (fgets(line, maxLen, pFile)) // read a line and check if it is read
			*ptr++ = countPipes(line, maxLen);	
	
	fclose(pFile);
	fcheck(pFile, close);
}
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	//add code here
}
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	//add code here
}
int countPipes(const char* lineBuffer, int maxCount)
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
{	// assume that filename is not a null pointer nor empty string
	char line[maxLen];
	int stringsPerStudent;

	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	
	// create a dynamic array of arrays of strings 
	char*** students = (char***)xmalloc(*numberOfStudents * sizeof(char**));
	
	FILE* pFile = fopen(fileName, "r");
	fcheck(pFile, open);
	
	// iterate over the students array
	for (int i = 0; i < *numberOfStudents && feof(pFile) == 0;)
	{	
		// calculate the required number of strings
		stringsPerStudent = 1 + 2 * (*coursesPerStudent)[i];
		
		// allocate memory for the array of strings
		students[i] = (char**)xmalloc(stringsPerStudent * sizeof(char*));
			
		if (fgets(line, maxLen, pFile))	// read a line and check if it is read
			// convert the line to an array of strings and save in the students array
			// increment the array index afterwards
			rtoa(line, students[i++]); 
	}
	
	fclose(pFile);
	fcheck(pFile, close);
	
	return students;
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
