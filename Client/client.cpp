#include <iostream>
#include <windows.h>
#include "Student.h"

using namespace std;

Student getStudentFromServer(HANDLE hNamedPipe, int num, int command);

int main(int args, char *argv[]) {
	Student student;
	int command;
	int num;
	bool exit = false;
	int answer;

	HANDLE hNamedPipe;
	
	char pipeName[] = "\\\\.\\pipe\\students";
	hNamedPipe = CreateFile(pipeName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	while (!exit)
	{
		printf("%s (%s, %s, %s): ", "Enter command", "1 - change", "2 - read", "3 - exit");
		scanf("%d", &command);
		switch (command) {
			case 1:
				printf("Enter record-book number:\n");
				scanf("%d", &num);
				command = 2;
				student = getStudentFromServer(hNamedPipe, num, command);
				printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", student.num, "Name", student.name, "Grade", student.grade);
				printf("%s: ", "Enter new record-book number");
				scanf("%d", &student.num);
				printf("%s: ", "Enter new name");
				scanf("%s", &student.name);
				printf("%s: ", "Enter new grade");
				scanf("%lf", &student.grade);
				printf("%s (%s, %s): ", "Do you want to send new values to the server?", "1 - Yes", "0 - No");
				scanf("%d", &answer);

				if (answer == 1) 
				{
					command = 1;
					WriteFile(hNamedPipe, &command, sizeof(int), NULL, NULL);
					WriteFile(hNamedPipe, &student, sizeof(Student), NULL, NULL);
				}

				break;
			case 2:
				printf("%s: ", "Enter record-book number");
				scanf("%d", &num);

				student = getStudentFromServer(hNamedPipe, num, command);

				printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", student.num, "Name", student.name, "Grade", student.grade);

				break;
			default:
				WriteFile(hNamedPipe, &command, sizeof(command), NULL, NULL);
				exit = true;

				break;
		}
	}

	CloseHandle(hNamedPipe);

	return 0;
}