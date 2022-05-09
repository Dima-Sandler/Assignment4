#define _CRT_SECURE_NO_WARNINGS
#define OPEN 0
#define CLOSE 1
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
void fcheck(FILE*, _Bool), rtoa(char*, char**), * xmalloc(unsigned int);
int constrainGrade(int);

// Part A
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);

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
		
	//factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", 5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);	
	studentsToFile(students, coursesPerStudent, numberOfStudents); // this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
	
	// Part B
	/*Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");*/

	// add code to free all arrays of struct Student

	/*_CrtDumpMemoryLeaks();*/ // uncomment this line to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
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
	fcheck(pFile, OPEN);

	*numberOfStudents = 0;
	
	// count the number of lines/students in the file
	while (!feof(pFile)) // stop when EOF is reached
		if (fgets(line, maxLen, pFile)) // read a line and check if it is read
			(*numberOfStudents)++;

	rewind(pFile);

	// create a dynamic array at the referenced location
	*coursesPerStudent = (int*)xmalloc(*numberOfStudents * sizeof(int));
	
	// initialize a local pointer to the array
	int* arr = *coursesPerStudent;
	
	// count and save the number of pipes/courses in every line
	for (int i = 0; i < *numberOfStudents && !feof(pFile);)
		if (fgets(line, maxLen, pFile)) // read a line and check if it is read
			arr[i++] = countPipes(line, maxLen);
	arr = NULL;

	fclose(pFile);
	fcheck(pFile, CLOSE);
}
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{	// the elements of the students array (char**) are constant
	     
	// handle edge cases:
	//  null ptr     null ptr              null ptr       empty str      
	if (!students || !coursesPerStudent || !courseName || !*courseName)
		return;

	// chck if factor is valid
	if (factor < -20 || factor > 20)
	{
		puts("Invalid factor value\n");
		return;
	}
	
	int stringsPerStudent;
	
	// iterate over the students array
	// if numberOfStudents is nonpositive the loop is not executed	
	for (int i = 0; i < numberOfStudents; i++)
	{
		if (!students[i]) // edge case check
			continue;

		// calculate the number of strings in  array of strings
		stringsPerStudent = 1 + 2 * coursesPerStudent[i];
			 
		// iterate over the array of strings
		// skip the first string i.e. the student name
		for (int j = 1; j < stringsPerStudent - 1; j += 2)
			// check if the string pointer is valid and the string maches
			if (students[i][j] && !strcmp(students[i][j], courseName))			
				// convert the string to an integer, add the factor, constrain to be within the range of 0-100, convert back to a string and save in the array of strings
				_itoa(constrainGrade(atoi(students[i][j + 1]) + factor), students[i][j + 1], 10);
	}
}
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	// handle edge cases:
	if (!students || !coursesPerStudent)
		return;

	int stringsPerStudent, lineLen = 0;
	char* fileName = "studentList_m.txt";

	FILE* pFile = fopen(fileName, "w");
	fcheck(pFile, OPEN);

	// iterate over the students array
	// if numberOfStudents is nonpositive the loop is not executed	
	for (int i = 0, j; i < numberOfStudents; i++)
	{
		if (!students[i]) // edge case check
			continue;

		// calculate the number of strings in  array of strings
		stringsPerStudent = 1 + 2 * coursesPerStudent[i];

		j = 0;

		// check if the string pointer is valid and the string is not empty
		if (students[i][j] && *students[i][j])
			// check if there is enough space in the line for the string
			if (strlen(students[i][j]) < maxLen)
			{
				// print the first string i.e. the student name
				lineLen = fprintf(pFile, "%s", students[i][j]);

				// free the string
				free(*students[i]);
				students[i][j] = NULL;

				j++;
			}

		// iterate over the array of strings
		for (; j >= 1 && j < stringsPerStudent - 1; j += 2)
			// check if the strings pointer are valid and the strings are not empty
			if (students[i][j] && *students[i][j] && students[i][j + 1] && *students[i][j])
				// check if there is enough space left in the line for the course and the grade strings + delimiters
				if (lineLen + strlen(students[i][j]) + strlen(students[i][j + 1]) + 2 < maxLen)
				{
					// print a pipe char followed by the course and the grade strings separated by ',' to the stream
					// sum the number of characters printed
					lineLen += fprintf(pFile, "|%s,%s", students[i][j], students[i][j + 1]);

					// free the course string
					free(students[i][j]);
					students[i][j] = NULL;

					// free the grade string
					free(students[i][j]);
					students[i][j] = NULL;
				}
				else
					break;

		// iterate over the array of strings
		// this loop is executed if the previous loop breaks
		for (; j < stringsPerStudent; j++)
		{
			// free the string
			free(students[i][j]);
			students[i][j] = NULL;
		}

		// free the the array of strings
		free(students[i]);
		students[i] = NULL;

		if (j > 1) // line must contain at least 1 course
			fflush(pFile); // save the printed line to the file
		else
			fseek(pFile, lineLen, SEEK_CUR); // return the position pointer to the start of the line
	}

	// free students array
	free(students);
	students = NULL;

	// free coursesPerStudent array
	free(coursesPerStudent);
	coursesPerStudent = NULL;

	int size = ftell(pFile);

	fclose(pFile);
	fcheck(pFile, CLOSE);

	// check if the file is empty
	if (!size)
		if (remove(fileName))
		{
			perror("Failed to remove file");
			exit(1);
		}
}
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{	// assume that filename is not a null pointer nor empty string
	char line[maxLen];
	int stringsPerStudent;

	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	
	// create a dynamic array of arrays of strings 
	char*** students = (char***)xmalloc(*numberOfStudents * sizeof(char**));
	
	FILE* pFile = fopen(fileName, "r");
	fcheck(pFile, OPEN);
	
	// iterate over the students array
	for (int i = 0; i < *numberOfStudents && !feof(pFile);)
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
	fcheck(pFile, CLOSE);
	
	return students;
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

// Part B
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

// auxiliary functions
void fcheck(FILE* pFile, _Bool op)
{
	// check if pFile is a null pointer
	if (!pFile)
	{
		if (op == OPEN)
			perror("Failed to open file");
		else 
			perror("Failed to close file");
		
		exit(1);
	}
}
void rtoa(char* rec, char** arr)
{
	char* delim = "|,";

	// read the first token i.e. the student name
	char* token = strtok(rec, delim);

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
int constrainGrade(int grade)
{
	if (grade < 0)
		grade = 0;
	else if (grade > 100)
		grade = 100;

	return grade;
}
