#include <iostream>
#include <fstream>
#include <windows.h>
#include "student.h"

using namespace std;

ifstream getIFStream(char* fileName);
ofstream getOFStream(char* fileName);

int main() 
{
	char lpszAppName[] = "Client.exe";
	char* lpszCommandLine = new char[1000];
	char* fileName = new char[260];
	int studentsNum;
	int num;
	int command;
	bool exit = false;
	int lastReadIndex;
	int answer;
	Student* students;
	Student student;
	ofstream fout;
	ifstream fin;

	HANDLE hNamedPipe;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	printf("%s: ", "Enter file name");
	scanf("%s", fileName);
	fout = getOFStream(fileName);

	printf("%s: ", "Enter number of students");
	scanf("%d", &studentsNum);
	fout << studentsNum;
	students = new Student[studentsNum];
	for (int i = 0; i < studentsNum; i++)
	{
		printf("%s: ", "Enter record-book number");
		scanf("%d", &(students[i].num));
		printf("%s: ", "Enter name");
		scanf("%s", students[i].name);
		printf("%s: ", "Enter grade");
		scanf("%lf", &(students[i].grade));
		fout.write((char*)&students[i], sizeof(Student));
	}
	fout.close();

	printf("%s:\n", "Original file");
	fin = getIFStream(fileName);
	fin >> studentsNum;
	for (int i = 0; i < studentsNum; i++) {
		printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", students[i].num, "Name", students[i].name, "Grade", students[i].grade);
	}
	fin.close();

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	sprintf(lpszCommandLine, "%s", lpszAppName);
	CreateProcess(NULL, lpszCommandLine, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	char pipeName[] = "\\\\.\\pipe\\students";
	hNamedPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, 0, 0, INFINITE, NULL);
	ConnectNamedPipe(hNamedPipe, NULL);

	while (!exit) {
		ReadFile(hNamedPipe, &command, sizeof(command), NULL, NULL);
		switch (command) {
			case 1:
				ReadFile(hNamedPipe, &student, sizeof(Student), NULL, NULL);

				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fin.read((char*)&students[i], sizeof(Student));
				}
				fin.close();

				fout = getOFStream(fileName);
				students[lastReadIndex] = student;
				fout << studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fout.write((char*)&students[i], sizeof(Student));
				}
				fout.close();

				break;
			case 2:
				ReadFile(hNamedPipe, &num, sizeof(int), NULL, NULL);

				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					fin.read((char*)&student, sizeof(Student));
					if (student.num == num) {
						WriteFile(hNamedPipe, &student, sizeof(Student), NULL, NULL);
						lastReadIndex = i;
						break;
					}
				}
				fin.close();

				break;
			default:
				printf("%s:\n", "Changed file");
				fin = getIFStream(fileName);
				fin >> studentsNum;
				for (int i = 0; i < studentsNum; i++) {
					printf("%s: %d\n%s: %s\n%s: %lf\n", "Record-book number", students[i].num, "Name", students[i].name, "Grade", students[i].grade);
				}
				fin.close();
				printf("%s (%s, %s): ", "Do you want to exit?", "1 - Yes", "0 - No");
				scanf("%d", &answer);
				if (answer == 1)
					exit = true;

				break;
		}
	}

	DisconnectNamedPipe(hNamedPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hNamedPipe);

	return 0;
}