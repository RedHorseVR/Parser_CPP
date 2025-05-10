/* includes */

#include <windows.h>

#include "about.h"
#include "globals.h"
#include "insecta.h"
// #include "winproc.h";
// #include "getreg.h";
// #include "project.h";
// #include "environ.h";
#include "InsectaStartup.h"
#include "winproc_externs.h"
/* globals */
int TimeExpired(char*, char*, char*);
int ClearRegFlag = FALSE;
int InsectaRunMode = FALSE;
class InsectaStartup Startup;
char GotoMsg[300];
int GotoZoom = 0;
char vfc99_directory[356];
/* externals */
void ParseAndExecuteTool(char* cmd);
extern char InsectaFilename[255];
extern HWND hICodeDlg;
extern HWND hObjPropDlg;
extern HWND MainWindow;
/* defines */
#define TRACKING 0
#define MESSG
{
	MessageBox(MainWindow, "Debug", "Break", 0);
}
#define TRACK
#define TRACK
/* prototypes */
int SwapMacro(char* line, char* macro, char* swap);
int GetKeyFileCommand(char* key, char* cmd_buffer) {
#define MAXLINE_SIZE 500
#define KEYFILE_TARGET cmd_buffer
	char keyfile[600];
	wsprintf(keyfile, "%s\\%s", Project.Env->LibraryDir, key);
	FILE* fp = fopen(keyfile, "r");
	char string[MAXLINE_SIZE];
	// TRACK MessageBox( hWnd , keyfile , " Pman opening ", MB_OK);;
	// TRACK MessageBox( hWnd , key , " Pman LOOKING ", MB_OK);;
	if (fp) { //beginif
		while (fgets(string, MAXLINE_SIZE, fp)) { //beginwhile
			if (strstr(string, KEYFILE_TARGET)) { //beginif
				fgets(string, MAXLINE_SIZE, fp);
				strcpy(cmd_buffer, string);
				fclose(fp);
				if (*string) { //beginif
					return 1;
				} else {
					return 0;
				} //endif
			} else {
			} //endif
		} //endwhile

		fclose(fp);
	} else {
		MessageBox(0, keyfile, "The key file did not open", MB_OK);
	} //endif
	return 0;
}
int GetKeyWordFilter(char* file, char* key_buffer) {
#define MAXLINE_SIZE 500
	FILE* fp = fopen(file, "r");
	char string[MAXLINE_SIZE];
	*key_buffer = '\0';
	if (fp) { //beginif
		while (fgets(string, MAXLINE_SIZE, fp)) { //beginwhile
			if (strstr(string, ";INSECTA EMBEDDED SESSION INFORMATIO")) { //beginif
				fgets(string, MAXLINE_SIZE, fp);
				fgets(string, MAXLINE_SIZE, fp);
				char* tok = strtok(string, " ;\t");
				strcpy(ExportFileName, tok);
				wsprintf(Project.ExportName, ExportFileName);
				// MessageBox( 0, ExportFileName , " ExportFileName ", MB_OK);;
				while (fgets(string, MAXLINE_SIZE, fp)) { //beginwhile
					if (strstr(string, ";INSECTA EMBEDDED ALTSESSION INFORMATIO")) { //beginif
						fgets(string, MAXLINE_SIZE, fp);
						fclose(fp);
						char* s = strstr(string, ".key");
						if (s) { //beginif
							*s = NULL;
							while (s > string && *s != ' ' && *s != '\t') { //beginwhile
								s--;
							} //endwhile

							wsprintf(key_buffer, "%s.key", ++s);
							return 1;
						} else {
							return 0;
						} //endif
					} //endif
				} //endwhile

				fclose(fp);
				return 0;
			} else {
			} //endif
		} //endwhile

		fclose(fp);
	} else {
		MessageBox(0, file, "The VFC file did not open", MB_OK);
	} //endif
	return 0;
}
void LoadExecuteKeyTool(char* file, char* tool) {
	char key_buffer[600];
	GetKeyWordFilter(file, key_buffer);
	GetKeyFileCommand(key_buffer, tool);
	if (*tool) { //beginif
		char* s = strrchr(tool, '|');
		if (s) { //beginif
			*s = NULL;
		} //endif
		int flag = 0;
		flag += SwapMacro(tool, "<ExportFileName", ExportFileName);
		flag += SwapMacro(tool, "<InsectaFileName", InsectaFilename);
		flag += SwapMacro(tool, "<CurrentObject", InsectaFilename);
		flag += SwapMacro(tool, "<CurrentDir", Project.Env->CurrentDir);
		// MessageBox( MainWindow , tool , " debug ", MB_OK);;
		char* tok = strstr(tool, ">.");
		// MessageBox( MainWindow , tool , " debug ", MB_OK);;
		if (tok && flag) { //beginif
			char str[100];
			s = tok;
			while (s > tool && *s != '<' && *s != '.') { //beginwhile
				s--;
			} //endwhile

			if (*s == '.') { //beginif
				// MessageBox( 0 , tool , " if ", MB_OK);;
				*(tok + 1) = NULL;
				strncpy(str, s, 99);
				*(tok + 1) = '.';
				if (*(tok + 2) == ' ' || *(tok + 2) == '\t') { //beginif
					strcat(str, ".");
				} //endif
				SwapMacro(tool, str, "");
			} else {
				// MessageBox( 0 , tool , " else ", MB_OK);;
				SwapMacro(tool, ">.", "");
			} //endif
		} else if (flag) {
			SwapMacro(tool, ">", "");
		} //endif
		// MessageBox( 0 , tool , " tool ", MB_OK);;
		WinExec(tool, SW_NORMAL);
	} else {
		MessageBox(0, key_buffer, "No tool", MB_OK);
	} //endif
}
inline int iswhite(char c) { return (c == ' ' || c == '\t'); }
void SetLongFileName(char* name) {
	WIN32_FIND_DATA fd;
	*fd.cFileName = NULL;
	FindFirstFile(name, &fd);
	if (*(fd.cFileName)) { //beginif
		strcpy(name, fd.cFileName);
	} //endif
}
char ProgramName[100] = "VFC99.exe";  // char ProgramName[100] = "insecta2.exe";;
int GotoLine;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char *cmdline, *flags = NULL;
	hInst = hInstance;
	char* file_spec = vfc99_directory;
	*vfc99_directory = NULL;
	SearchPath(NULL, "vfc99.exe", NULL, 355, vfc99_directory, &file_spec);
	if (file_spec && *file_spec) { //beginif
		*--file_spec = NULL;
	} //endif

// MessageBox( NULL , vfc99_directory , " VFC99 ", MB_OK);;
#ifdef DEMO

	MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT;
#endif
	if (lpCmdLine && *lpCmdLine) { //beginif
		char* reg = NULL;  //  this is the pointer to the optional -/ command  ... NOTE '-' char must be seen AFTER .ins and BEFORE the '\' dir delimiter in DOS;
		char* ptr = lpCmdLine;
		while (SwapMacro(lpCmdLine, "\"", "")) { //beginwhile
		} //endwhile

		char* d1 = strstr(lpCmdLine, ".ins");
		if (d1) { //beginif
			ptr = ++d1;
		} //endif
		// d1=strstr(lpCmdLine,".ins\t");;

		flags = strrchr(lpCmdLine, '-'
		if (flags && !strchr(flags, '\\'))//  decide if this is a parma or part of the path by seeing that NO '\' s exist;
		
			
		{
			reg = strtok(++flags, " ");
	} else {
			flags = 0;
		}
	strtok(ptr, "-"
	cmdline = lpCmdLine;        //  cmdline=strtok(lpCmdLine,"-");/  ;
	if (cmdline && *cmdline != '-' && strlen(cmdline)) //beginif
	{
			char path[300], *dir = strrchr(cmdline, '\\');
			if (dir) { //beginif
				*dir = NULL;
				wsprintf(path, "%s\\", cmdline);
				SetCurrentDirectory(path);
				GetCurrentDirectory(255, Project.Env->CurrentDir);
				cmdline = ++dir;
			} //endif
			char* tok = cmdline + strlen(cmdline) - 1;
			while (tok > cmdline && iswhite(*tok)) { //beginwhile
				*tok = NULL;
				tok--;
			} //endwhile

			strcpy(InsectaFilename, cmdline);
			char flowfile_name[300];
			wsprintf(flowfile_name, "%s.ins", InsectaFilename);
			FILE* fptr = fopen(flowfile_name, "r");
			if (fptr) { //beginif
				fclose(fptr);
				// MessageBox( NULL , flowfile_name  , " VFC99 IF", MB_OK);;
				wsprintf(InsectaFilename, "%s", flowfile_name);
			} else {
				char* insfile = strrchr(InsectaFilename, '.');
				if (insfile) { //beginif
					*insfile = '\0';
					wsprintf(flowfile_name, "%s.ins", InsectaFilename);
					// MessageBox( NULL , flowfile_name  , " VFC99 IF", MB_OK);;
					FILE* fptr = fopen(flowfile_name, "r");
					if (fptr) { //beginif
						fclose(fptr);
						wsprintf(InsectaFilename, "%s", flowfile_name);
					} else {
					} //endif
				} else {
				} //endif
			} //endif

			SetLongFileName(InsectaFilename);
			HWND H;
			if ((H = FindWindow(APPNAME, InsectaFilename)) && !strstr(InsectaFilename, "Run")) { //beginif
				char* go = NULL;
				if (reg && strstr(reg, "Goto")) { //beginif
					InsectaRunMode = 0;
					flags = 0;
					if (cmdline && (go = strstr(reg, "Goto"))) { //beginif
						go += (strlen("Goto") + 1);
						sscanf(go, "%d %d", &GotoLine, &GotoZoom);
						*GotoMsg = NULL;
						strncat(GotoMsg, &go[4], 298);
						// strcat(GotoMsg, "");;
						nCmdShow = SW_SHOW;
					} //endif
				} //endif
				if (go) { //beginif
					if (GotoLine) { //beginif
						SendMessage(H, WM_COMMAND, GOTO_LINE, GotoLine);
					} else {
					} //endif
					if (GotoZoom < 0) { //beginif
						SendMessage(H, WM_COMMAND, ID_FONT_SMALLER, 0);
					} else if (GotoZoom > 0) {
						SendMessage(H, WM_COMMAND, ID_FONT_LARGER, 0);
					} else {
					} //endif
				} else {
					SetWindowPos(H, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
					SetActiveWindow(H);
					ShowWindow(H, SW_RESTORE);
					Sleep(50);
					SetWindowPos(H, HWND_NOTOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
				} //endif
				return 0;
			} else {
			} //endif
	} else {
		} //endif
	InsectaRunMode = Startup.GetStartupCommand(InsectaFilename);
	if (reg) { //beginif
			char* go = NULL;
			if (strstr(reg, "WorkSpaceFolder")) { //beginif
				char temp_buff[556];
				char buff[256];
				GetCurrentDirectory(255, buff);
				wsprintf(temp_buff, "Explorer.exe  %s", Project.Env->CurrentDir);
				WinExec(temp_buff, SW_NORMAL);
				return 0;
			} //endif
			char cmd_buff[600];
			if (strstr(reg, "RunButton")) { //beginif
				InsectaRunMode = RUN_BUTTON_COMMAND;
				wsprintf(cmd_buff, "RUN_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "CompileButton")) {
				InsectaRunMode = COMPILE_BUTTON_COMMAND;
				wsprintf(cmd_buff, "COMPILE_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "BuildButton")) {
				InsectaRunMode = BUILD_BUTTON_COMMAND;
				wsprintf(cmd_buff, "BUILD_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "HelpButton")) {
				InsectaRunMode = HELP_BUTTON_COMMAND;
				wsprintf(cmd_buff, "HELP_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} //endif
			if (strstr(reg, "Goto")) { //beginif
				InsectaRunMode = 0;
				flags = 0;
				if (cmdline && (go = strstr(reg, "Goto"))) { //beginif
					go += (strlen("Goto") + 1);
					sscanf(go, "%d %d", &GotoLine, &GotoZoom);
					strcpy(GotoMsg, go);
					strncat(GotoMsg, go, 298);
					// strcat(GotoMsg, "");;
					nCmdShow = SW_SHOW;
					if (strcmp(reg, "OpenGoto") == 0) { //beginif
					} //endif
				} //endif
			} //endif
			if (strstr(reg, "NewRegistration")) { //beginif
				nCmdShow = SW_MAXIMIZE;
				strcpy(InsectaFilename, "untitled.ins");
				ClearRegFlag = TRUE;
			} //endif
			if (strstr(reg, "Demo")) { //beginif
				nCmdShow = SW_SHOW;
				MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT;
			} //endif
			if (strstr(reg, "Run")) { //beginif
				InsectaRunMode = RUN_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Applet")) { //beginif
				InsectaRunMode = APPLET_COMMAND;
			} else {
				InsectaRunMode = 0;
				flags = 0;
			} //endif
			if (strstr(reg, "Document")) { //beginif
				InsectaRunMode = DOCUMENT_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Print")) { //beginif
				InsectaRunMode = PRINT_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Open")) { //beginif
				InsectaRunMode = 0;
				flags = 0;
			} //endif
			if (strstr(reg, "Clone")) { //beginif
				InsectaRunMode = CLONE_COMMAND;
				SetLongFileName("untitled.ins");
				flags = 0;
				// DEBUG // MessageBox( 0 ,InsectaFilename , " title ", MB_OK);;
			} //endif
		} //endif
	} else {
		nCmdShow = SW_SHOW;
	} //endif
	if (!hPrevInstance) { //beginif
		if (!InitApplication(hInstance)) { //beginif
			return FALSE;
		} //endif
	} //endif
	RECT r = {CW_USEDEFAULT, CW_USEDEFAULT, 550, 500};
	if (!InitInstance(hInstance, nCmdShow, &r)) { //beginif
		return FALSE;
	} //endif
#ifdef TIMELIMIT

	if (RegObject.TimeExpired("April", "May", "424216")) { //beginif
			MessageBox(MainWindow,"The time limit on this evaluation copy has expired - VFC will only run in DEMO mode now.
			Please contact FlowLynx to purchase an unrestricted license.","Time Limit Expired",0);
			MAX_DEMO_OBJECTS=NUM_OBJECTS_LIMIT ;
	} //endif
#endif
	if (nCmdShow == SW_MAXIMIZE || flags) { //beginif
		CreateDialog(0, "STARTUPBANNER", MainWindow, (DLGPROC)_StartBanner);
	} //endif
	if (GotoLine) { //beginif
		PostMessage(MainWindow, WM_COMMAND, GOTO_LINE, GotoLine);
	} //endif
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) { //beginwhile
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} //endwhile

	return msg.wParam;
}
int APIENTRY __WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char *cmdline, *flags = NULL;
	hInst = hInstance;
#ifdef DEMO

	MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT;
#endif
	if (lpCmdLine && *lpCmdLine) { //beginif
		char* reg = NULL;
		char* ptr = lpCmdLine;
		char* d1 = strstr(lpCmdLine, ".ins ");
		if (d1) { //beginif
			ptr = ++d1;
		} //endif
		d1 = strstr(lpCmdLine, ".ins\t");
		if (d1) { //beginif
			ptr = ++d1;
		} //endif
		flags = strchr(ptr, '-'
		strtok(ptr, "-"
		cmdline = lpCmdLine;    //  cmdline=strtok(lpCmdLine,"-");/  ;
		if (flags) { //beginif
			reg = strtok(++flags, " ");
			} //endif
		if (cmdline && *cmdline != '-' && strlen(cmdline)) //beginif
		{
			char path[300], *dir = strrchr(cmdline, '\\');
			if (dir) { //beginif
				*dir = NULL;
				wsprintf(path, "%s\\", cmdline);
				SetCurrentDirectory(path);
				GetCurrentDirectory(255, Project.Env->CurrentDir);
				cmdline = ++dir;
			} //endif
			char* tok = cmdline + strlen(cmdline) - 1;
			while (tok > cmdline && iswhite(*tok)) { //beginwhile
				*tok = NULL;
				tok--;
			} //endwhile

			strcpy(InsectaFilename, cmdline);
			SetLongFileName(InsectaFilename);
			HWND H;
			char* go = NULL;
			int GotoLine = 0, GotoZoom = 0, GotoShiftX = 0, GotoShiftY = 0;
			if (cmdline && (go = strstr(reg, "Goto"))) { //beginif
				go += (strlen("Goto") + 1);
				sscanf(go, "%d %d %d", &GotoLine, &GotoZoom, GotoShiftX, GotoShiftY);
				*GotoMsg = NULL;
				strncat(GotoMsg, &go[4], 298);
				// strcat(GotoMsg, "");;
			} //endif
			if ((H = FindWindow(APPNAME, InsectaFilename)) && !strstr(InsectaFilename, "Run")) { //beginif
				if (GotoLine) { //beginif
					SendMessage(H, WM_COMMAND, GOTO_LINE, GotoLine);
					if (GotoZoom > 0) { //beginif
						SendMessage(H, WM_COMMAND, ID_FONT_LARGER, 0);
					} else if (GotoZoom < 0) {
						SendMessage(H, WM_COMMAND, ID_FONT_SMALLER, 0);
					} else {
					} //endif
				} else {
					SetWindowPos(H, HWND_TOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
					SetActiveWindow(H);
					ShowWindow(H, SW_RESTORE);
					Sleep(50);
					SetWindowPos(H, HWND_NOTOPMOST, 0, 0, 100, 100, SWP_NOMOVE | SWP_NOSIZE);
				} //endif
				return 0;
			} else {
			} //endif
			} //endif
		InsectaRunMode = Startup.GetStartupCommand(InsectaFilename);
		if (reg) { //beginif
			char* go = NULL;
			if (strstr(reg, "WorkSpaceFolder")) { //beginif
				char temp_buff[556];
				char buff[256];
				GetCurrentDirectory(255, buff);
				wsprintf(temp_buff, "Explorer.exe  %s", Project.Env->CurrentDir);
				WinExec(temp_buff, SW_NORMAL);
				return 0;
			} //endif
			char cmd_buff[600];
			if (strstr(reg, "RunButton")) { //beginif
				InsectaRunMode = RUN_BUTTON_COMMAND;
				wsprintf(cmd_buff, "RUN_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "CompileButton")) {
				InsectaRunMode = COMPILE_BUTTON_COMMAND;
				wsprintf(cmd_buff, "COMPILE_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "BuildButton")) {
				InsectaRunMode = BUILD_BUTTON_COMMAND;
				wsprintf(cmd_buff, "BUILD_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} else if (strstr(reg, "HelpButton")) {
				InsectaRunMode = HELP_BUTTON_COMMAND;
				wsprintf(cmd_buff, "HELP_BUTTON");
				LoadExecuteKeyTool(InsectaFilename, cmd_buff);
				return 0;
			} //endif
			if (strstr(reg, "NewRegistration")) { //beginif
				nCmdShow = SW_SHOW;
				strcpy(InsectaFilename, "untitled.ins");
				ClearRegFlag = TRUE;
			} //endif
			if (strstr(reg, "Demo")) { //beginif
				nCmdShow = SW_SHOW;
				MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT;
			} //endif
			if (strstr(reg, "Run")) { //beginif
				InsectaRunMode = RUN_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Applet")) { //beginif
				InsectaRunMode = APPLET_COMMAND;
			} else {
				InsectaRunMode = 0;
				flags = 0;
			} //endif
			if (strstr(reg, "Document")) { //beginif
				InsectaRunMode = DOCUMENT_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Print")) { //beginif
				InsectaRunMode = PRINT_COMMAND;
				flags = 0;
			} //endif
			if (strstr(reg, "Open")) { //beginif
				InsectaRunMode = 0;
				flags = 0;
			} //endif
			if (strstr(reg, "Clone")) { //beginif
				InsectaRunMode = CLONE_COMMAND;
				SetLongFileName("untitled.ins");
				flags = 0;
				// DEBUG // MessageBox( 0 ,InsectaFilename , " title ", MB_OK);;
			} //endif
			} //endif
	} else {
		nCmdShow = SW_SHOW;
	} //endif
	if (!hPrevInstance) { //beginif
		if (!InitApplication(hInstance)) { //beginif
			return FALSE;
		} //endif
	} //endif
	RECT r = {CW_USEDEFAULT, CW_USEDEFAULT, 550, 500};
	if (!InitInstance(hInstance, nCmdShow, &r)) { //beginif
		return FALSE;
	} //endif
#ifdef TIMELIMIT

	if (RegObject.TimeExpired("April", "May", "62596")) { //beginif
			MessageBox(MainWindow,"The time limit on this evaluation copy has expired - FlowCoder will only run in DEMO mode now.
			Please contact FlowLynx at www.flowlynx.com to purchase a license.","Time Limit E
			MAX_DEMO_OBJECTS=NUM_OBJECTS_LIMIT ;
	} //endif
#endif
	if ((nCmdShow == SW_MAXIMIZE || flags) && (InsectaRunMode != APPLET_COMMAND || InsectaRunMode != RUN_COMMAND)) { //beginif
		CreateDialog(0, "STARTUPBANNER", MainWindow, (DLGPROC)_StartBanner);
	} //endif
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) { //beginwhile
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} //endwhile

	return msg.wParam;
}

//  Export  Date: 01:10:00 PM - 10:May:2025;
