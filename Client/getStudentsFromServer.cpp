#include <iostream>
#include <windows.h>
#include "Student.h"

using namespace std;

Student getStudentFromServer(HANDLE hNamedPipe, int num, int command)
{
	Student student;
	WriteFile(hNamedPipe, &command, sizeof(command), NULL, NULL);
	WriteFile(hNamedPipe, &num, sizeof(num), NULL, NULL);
	ReadFile(hNamedPipe, &student, sizeof(student), NULL, NULL);
	return student;
}