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
void fcheck(FILE*, _Bool), rtoa(char*, char**), freeStrings(char** arr, int size), freeStudents(char***, int), * xmalloc(unsigned int);
int constrainGrade(int);

// Part A
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
void studentsToFile_s(char*** students, int* coursesPerStudent, int numberOfStudents);
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
		
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", 5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);	
	studentsToFile_s(students, coursesPerStudent, numberOfStudents); // this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
	
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
	fcheck(pFile, open);

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
	for (int i = 0; i < *numberOfStudents && !feof(pFile); i++)
		if (fgets(line, maxLen, pFile)) // read a line and check if it is read
			arr[i] = countPipes(line, maxLen);
	arr = NULL;

	fclose(pFile);
	fcheck(pFile, close);
}
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{	// the elements of the students array (char**) are constant
	     
	// handle edge cases:
	if (!students || !coursesPerStudent || !courseName )
		return;

	// chck if factor is valid
	if (factor < -20 || factor > 20)
	{
		puts("Invalid factor value\n");
		return;
	}
	
	int stringsPerStudent;
	
	// iterate over the students array
	for (int i = 0; i < numberOfStudents; i++)
	{
		if (!students[i]) // edge case check
			continue;

		// calculate the number of strings in  array of strings
		stringsPerStudent = 1 + 2 * coursesPerStudent[i];
			 
		// iterate over the array of strings from the second string
		for (int j = 1; j < stringsPerStudent - 1; j += 2)
			// check if the string pointer is valid and the string maches
			if (students[i][j] && !strcmp(students[i][j], courseName))			
				// convert the string to an integer, add the factor, constrain to be within the range, convert back to a string and save in the array of strings
				_itoa(constrainGrade(atoi(students[i][j + 1]) + factor), students[i][j + 1], 10);
	}
}
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{	// assume that students is a pointer to a proper array representation of the students records
	// handle edge cases:
	if (!students)
	{
		free(coursesPerStudent);
		coursesPerStudent = NULL;

		return;
	}
	if (!coursesPerStudent)
	{
		freeStudents(students, numberOfStudents);
		students = NULL;

		puts("Unable to deallocate memory");
		exit(1);
	}
	
	int stringsPerStudent;

	FILE* pFile = fopen("studentList_m.txt", "w");
	fcheck(pFile, open);

	// iterate over the students array
	for (int i = 0, j; i < numberOfStudents; i++)
	{
		// calculate the number of strings in the array of strings
		stringsPerStudent = 1 + 2 * coursesPerStudent[i];

		// print the first string i.e. the student name
		fprintf(pFile, "%s", students[i][0]);

		// free the name string
		freeStrings(students[i], 1);

		// iterate over the array of strings
		for (j = 1; j < stringsPerStudent - 1; j += 2)
		{
			// print a pipe char followed by the course and the grade strings separated by ','
			// sum the number of characters printed
			fprintf(pFile, "|%s,%s", students[i][j], students[i][j + 1]);

			// free the course and the grade strings
			freeStrings(students[i] + j, 2);
		}

		// free the array of strings
		free(students[i]);
		students[i] = NULL;

		// save the printed line and print a newline character
		fflush(pFile);
		fputc('\n', pFile);
	}

	// free the students and coursesPerStudent array
	free(students);
	students = NULL;
	
	free(coursesPerStudent);
	coursesPerStudent = NULL;

	fclose(pFile);
	fcheck(pFile, close);
}
void studentsToFile_s(char*** students, int* coursesPerStudent, int numberOfStudents)
{ // this strict version will ensure that the new file will meet all the requirements
	// handle edge cases:
	if (!students)
	{
		free(coursesPerStudent);
		coursesPerStudent = NULL;
	
		return;
	}

	if (!coursesPerStudent)
	{
		freeStudents(students, numberOfStudents);
		students = NULL;

		puts("Unable to deallocate memory");
		exit(1);
	}

	int stringsPerStudent, fileSize, nameLen = 0, lineLen = 0;
	char* fileName = "studentList_m.txt";

	FILE* pFile = fopen(fileName, "w");
	fcheck(pFile, open);

	// req 4: the file should not contain 2 students with the same name
	// delete duplicate students if any
	for (int i = 0; i < numberOfStudents - 1; i++)
		for (int j = i + 1; j < numberOfStudents; j++)
			if (!strcmp(*students[i], *students[j]))
			{
				freeStrings(students[j], 1 + 2 * coursesPerStudent[j]);
				freeStudents(students + j, 1);
			}

	// iterate over the students array
	for (int i = 0, j; i < numberOfStudents; i++)
	{
		if (!students[i]) // check if the array of strings pointer is valid
			continue;

		// calculate the number of strings in the array of strings
		stringsPerStudent = 1 + 2 * coursesPerStudent[i];

		// check if the string pointer is valid and the string is not empty
		if (students[i][0] && *students[i][0])
			// req 1: line length should not exceed 1023 characters
			// check if there is enough space in the line for the string
			if (strlen(students[i][0]) < maxLen)
			{
				// print the first string i.e. the student name
				// save the number of characters printed
				lineLen = nameLen = fprintf(pFile, "%s", students[i][0]);

				// free the name string
				freeStrings(students[i], 1);
			}

		// req 4: the record must contain the student name
		// check if the name wasn`t printed
		if (!nameLen)
		{
			// free all the strings and the array of strings
			freeStrings(students[i], stringsPerStudent);
			freeStudents(students + i, 1);

			continue; // continue to the next student
		}

		// iterate over the array of strings from the second string
		for (j = 1; j < stringsPerStudent - 1; j += 2)
			// req 5: the record cannot have a course without grade or a grade only
			// check if the strings pointer are valid and the strings are not empty
			if (students[i][j] && *students[i][j] && students[i][j + 1] && *students[i][j + 1])
				// req 1: line length should not exceed 1023 characters
				// check if there is enough space left in the line for the strings + delimiters
				if (lineLen + strlen(students[i][j]) + strlen(students[i][j + 1]) + 2 < maxLen)
				{
					// print a pipe char followed by the course and the grade strings separated by ','
					// sum the number of characters printed
					lineLen += fprintf(pFile, "|%s,%s", students[i][j], students[i][j + 1]);

					// free the course and the grade strings
					freeStrings(students[i] + j, 2);
				}
				else
				{
					// free the rest of the strings
					freeStrings(students[i] + j, stringsPerStudent);
					break;
				}

		// free the array of strings
		freeStudents(students + i, 1);
		
		// req 4: the record must contain at least 1 course
		// check if the line is longer then the name 
		if (lineLen > nameLen)
		{   // save the printed line and print a newline char
			fflush(pFile); 
			fputc('\n', pFile);
		}
		else // no courses were printed
			 // return the position pointer to the start of the line
			fseek(pFile, -lineLen, SEEK_CUR);
	}

	// free the students and coursesPerStudent array
	free(students);
	students = NULL;
	
	free(coursesPerStudent);
	coursesPerStudent = NULL;

	// obtain the file size
	fileSize = ftell(pFile); 

	fclose(pFile);
	fcheck(pFile, close);

	// req 2: the file should not be empty
	// check if the file is empty
	if (!fileSize)
		if (remove(fileName))
		{
			perror("Failed to remove file");
			exit(1);
		}
}
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{	
	// handle edge cases:
	if (!coursesPerStudent || !numberOfStudents)
		return NULL;
	
	char line[maxLen];
	int stringsPerStudent;

	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	
	// create a dynamic array of arrays of strings 
	char*** students = (char***)xmalloc(*numberOfStudents * sizeof(char**));
	
	FILE* pFile = fopen(fileName, "r");
	fcheck(pFile, open);
	
	// iterate over the students array
	for (int i = 0; i < *numberOfStudents && !feof(pFile); i++)
	{	
		// calculate the required number of strings
		stringsPerStudent = 1 + 2 * (*coursesPerStudent)[i];
		
		// allocate memory for the array of strings
		students[i] = (char**)xmalloc(stringsPerStudent * sizeof(char*));
			
		if (fgets(line, maxLen, pFile))	// read a line and check if it is read
		{
			line[strlen(line) - 1] = 0; // delete the end of line character

			// convert the line to an array of strings and save in the students array
			rtoa(line, students[i]);
		}
	}
	
	fclose(pFile);
	fcheck(pFile, close);
	
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
		if (op == open)
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
void freeStrings(char** arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		free(arr[i]);
		arr[i] = NULL;
	}

	arr = NULL;
}
void freeStudents(char*** arr, int size)
{
	for (int i = 0; i < size; i++)
	{
		free(arr[i]);
		arr[i] = NULL;
	}

	arr = NULL;
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
