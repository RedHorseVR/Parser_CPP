/* includes */
	
	

	#include <windows.h>
	#include "insecta.h"
	#include "globals.h"
	#include "about.h"
	#include "res_extra.h"
	#include "utils.h"
	
	
	
	#include "InsectaStartup.h"
	#include "winproc_externs.h"
/* globals */
	int TimeExpired(char*, char*,char*);
	int  ClearRegFlag   = FALSE;
	int  InsectaRunMode = FALSE;
	class InsectaStartup Startup;
	char GotoMsg[1024];
	int GotoZoom=0;
	extern char vfc99_directory[];
/* externals */
	void  ParseAndExecuteTool( char *cmd );
	extern char InsectaFilename[];
	extern HWND hICodeDlg;
	extern HWND hObjPropDlg;
	extern HWND MainWindow;
	extern int check_for_export(char *VFCfile, char *EXPfile);
/* defines */
	#define TRACKING    0
	#define MESSG       {MessageBox(MainWindow,"Debug","Break",0);}
	#define TRACK
	#define TRACK
/* prototypes */
	int SwapMacro( char * line , char * macro, char *swap );
	
int GetKeyFileCommand( char * key , char * cmd_buffer  ){
	#define MAXLINE_SIZE   2048
	#define  KEYFILE_TARGET   cmd_buffer
	char  keyfile[1024];
	wsprintf( keyfile,"%s\\%s", Project.Env->LibraryDir, key );
	FILE *fp = fopen( keyfile , "r"  );
	char string[MAXLINE_SIZE];
	
	
	if( fp )
	{
		while(  fgets( string, MAXLINE_SIZE , fp )  ) {
			if( strstr( string , KEYFILE_TARGET )  )
			{
				fgets( string, MAXLINE_SIZE , fp );
				strcpy( cmd_buffer , string );
				fclose( fp );
				if( *string  )
				{
					return 1;
				}else{
					return 0;
					}
			}else{
				}
			}
		
		fclose( fp );
	}else{
		MessageBox( 0, keyfile , "The key file did not open" , MB_OK);
		}
	return 0;   }
int   GetKeyWordFilter( char * file, char *key_buffer ){
	
	FILE *fp = fopen( file , "r"  );
	char string[MAXLINE_SIZE];
	*key_buffer = '\0';
	if( fp )
	{
		while(  fgets( string, MAXLINE_SIZE , fp )  ) {
			if( strstr( string , ";INSECTA EMBEDDED SESSION INFORMATIO" )  )
			{
				fgets( string, MAXLINE_SIZE , fp );
				fgets( string, MAXLINE_SIZE , fp );
				char  *tok = strtok( string , " ;\t" );
				strcpy( ExportFileName , tok  );
				wsprintf( Project.ExportName , ExportFileName );
				
				while(  fgets( string, MAXLINE_SIZE , fp )  ) {
					if( strstr( string , ";INSECTA EMBEDDED ALTSESSION INFORMATIO" )  )
					{
						fgets( string, MAXLINE_SIZE , fp );
						fclose( fp );
						char *s=strstr( string , ".key" );
						if( s )
						{
							*s = NULL;
							while( s>string && *s!=' ' && *s!='\t'  ) {
								s--; }
							
							wsprintf( key_buffer , "%s.key", ++s );
							return 1;
						}else{
							return 0;
							}
						}
					}  // end while
				
				fclose( fp );
				return 0;
			}else{
				}
			}
		
		fclose( fp );
	}else{
		MessageBox( 0, file , "The VFC file did not open" , MB_OK);
		}
	return 0;   }
void   LoadExecuteKeyTool( char *file, char* tool ){
	char key_buffer[1024];
	GetKeyWordFilter( file, key_buffer );
	GetKeyFileCommand( key_buffer , tool );
	if( *tool )
	{
		char *s=strrchr( tool , '|' );
		if( s )
		{
			*s = NULL;
			}
		int flag = 0;
		flag += SwapMacro( tool , "<ExportFileName" , ExportFileName );
		flag +=SwapMacro( tool , "<InsectaFileName" , InsectaFilename );
		flag +=SwapMacro( tool , "<CurrentObject" , InsectaFilename );
		flag +=SwapMacro( tool , "<CurrentDir" , Project.Env->CurrentDir );
		
		char *tok=strstr( tool , ">." );
		
		if( tok && flag )
		{
			char str[1024];
			s= tok;
			while( s>tool && *s!='<' && *s!='.'  ) {
				s--; }
			
			if( *s=='.'  )
			{
				
				*(tok+1) = NULL;
				strncpy( str , s , 99 );
				*(tok+1) = '.' ;
				if( *(tok+2) == ' ' || *(tok+2) == '\t'  )
				{
					strcat( str , "."  );
					}
				SwapMacro( tool , str , "" );
			}else{
				
				SwapMacro( tool , ">." , "" );
				}
		}else if(flag) {
			SwapMacro( tool , ">" , "" );
			}
		
		WinExec(tool,SW_NORMAL);
	}else{
		MessageBox( 0 , key_buffer , "No tool" , MB_OK);
		}
	}
inline  int iswhite( char c){
	return (c==' ' || c=='\t') ;  }
void   SetLongFileName( char * name ){
	WIN32_FIND_DATA fd;
	*fd.cFileName = NULL;
	FindFirstFile( name , &fd );
	if(*(fd.cFileName) )
	{
		strcpy( name , fd.cFileName );
		}
	}
int GotoLine;
char ProgramName[1024]  = "VFC2000.exe";

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
	{
	FILE *fptr ;
	char *cmdline, *flags=NULL;
	hInst=hInstance;
	char *file_spec=vfc99_directory;
	*vfc99_directory = NULL;
	SearchPath( NULL, (LPCSTR)ProgramName , NULL, 355 ,  vfc99_directory , &file_spec );
	if( file_spec && *file_spec )
	{
		*--file_spec = NULL;
		}
	
	
	if(lpCmdLine && *lpCmdLine)
	{
		char *reg =NULL;
		char *ptr  = lpCmdLine;
		char  cmd[MAX_PATH];
		strcpy( cmd , lpCmdLine );
		while( SwapMacro( cmd, "\"" , "")  ) {
			}
		
		char * filen=strrchr( cmd , '\\' );
		if( filen )
		{
			*filen='\0';
			strcpy( Project.Env->CurrentDir , cmd );
			SetCurrentDirectory( cmd );
			filen++;
		}else{
			filen = cmd;
			}
		strcpy( InsectaFilename, filen );
		char * load_options=strrchr( InsectaFilename, '-' );
		if( load_options )
		{
			*load_options =  NULL;
		}else{
			}
		char * d1=strrchr( filen , '.' );
		if(  d1 )
		{
			ptr = ++d1;
		}else{
			ptr  = filen ;
			}
		flags=strrchr( ptr ,'-');
		if( flags  && flags > ptr)
		{
			reg=strtok(++flags," ");
		}else{
			flags=0;
			}
		strtok( ptr ,"-");
		cmdline=lpCmdLine;
		if( flags && cmdline && *cmdline!='-' && strlen(cmdline)   )
		{
			char path[1024], *dir = strrchr(cmdline, '\\');
			if ( dir )
			{
				*dir=NULL;
				wsprintf( path, "%s\\",cmdline );
				SetCurrentDirectory(path);
				GetCurrentDirectory(255, Project.Env->CurrentDir);
				cmdline = ++dir;
				}
			char *tok = cmdline +strlen(cmdline ) - 1;
			while( tok > cmdline && iswhite(*tok) ){
				*tok = NULL;
				tok--; }
			
			//  nsectaFileName is the main target file name
			char flowfile_name[1512];
			char export_name[1512];
			wsprintf( export_name ,"%s" ,  InsectaFilename );
			char *insfile = strstr( export_name, EXTENSION  );
			if( insfile )
			{
				*insfile = '\0';
				}
			wsprintf( flowfile_name ,"%s\\%s" , Project.Env->CurrentDir,  InsectaFilename );
			fptr = fopen( flowfile_name ,"r");
			if( fptr &&  ( strstr( InsectaFilename, EXTENSION )  ||  strstr( InsectaFilename, LEGACY_EXTENSION )   )  )
			{
				fclose( fptr );
			}else{
				wsprintf( flowfile_name ,"%s\\%s%s" , Project.Env->CurrentDir,  InsectaFilename , EXTENSION );
				fptr = fopen( flowfile_name ,"r");
				if(  fptr  )
				{
				}else{
					wsprintf( flowfile_name ,"%s\\%s%s" , Project.Env->CurrentDir,  InsectaFilename , LEGACY_EXTENSION );
					fptr = fopen( flowfile_name ,"r");
					}
				if(  fptr  && check_for_export( flowfile_name,  export_name  )  )
				{
					fclose( fptr );
					
					wsprintf( InsectaFilename,"%s" , flowfile_name );
				}else{
					if( fptr )
					{
						fclose( fptr );
					}else{
						}
					char *insfile = strrchr( InsectaFilename, '.' );
					if( insfile  )
					{
						*insfile = '\0';
						wsprintf( flowfile_name ,"%s\\%s%s" , Project.Env->CurrentDir , InsectaFilename , EXTENSION );
						
						fptr = fopen(flowfile_name,"r");
						if(  fptr  )
						{
						}else{
							wsprintf( flowfile_name ,"%s\\%s%s" , Project.Env->CurrentDir,  InsectaFilename , LEGACY_EXTENSION );
							fptr = fopen( flowfile_name ,"r");
							}
						if(  fptr && check_for_export( flowfile_name,  export_name  ) )
						{
							fclose( fptr );
							wsprintf( InsectaFilename,"%s" , flowfile_name );
						}else{
							if( fptr )
							{
								fclose( fptr );
							}else{
								}
							MessageBox( NULL , "Visual Flow Code file for specified code not found.", export_name  , MB_OK);
							return 0;
							}
					}else{
						}
					}
				// END OF NEW AUTO OPEN FLOWFILE FIX
				}
			
			
			SetLongFileName(InsectaFilename);
			HWND H;
			if(  (H = FindWindow( APPNAME , InsectaFilename )  ) && ! strstr( InsectaFilename,"Run")  )
			{
				char *go=NULL;
				if( cmdline && (go=strstr(cmdline, "Reload"))   )
				{
					
					SendMessage(H ,WM_COMMAND, ID_FILE_RELOAD ,  0 );
					
					}
				if( reg &&  strstr(reg,"Goto") )
				{
					InsectaRunMode=0;
					flags=0;
					if( cmdline && (go=strstr(reg, "Goto"))   )
					{
						go+=(strlen("Goto")+1);
						sscanf(go, "%d %d",&GotoLine , &GotoZoom );
						*GotoMsg=NULL;
						strncat(GotoMsg, &go[4], 298 );
						
						nCmdShow=SW_SHOW;
						}
					}
				if( go  )
				{
					if( GotoLine )
					{
						SendMessage( H , WM_COMMAND , GOTO_LINE , GotoLine );
					}else{
						}
					if( GotoZoom < 0  )
					{
						SendMessage( H , WM_COMMAND , ID_FONT_SMALLER , 0 );
					}else if( GotoZoom > 0 ) {
						SendMessage( H , WM_COMMAND , ID_FONT_LARGER , 0 );
					}else{
						}
				}else{
					SetWindowPos( H ,HWND_TOPMOST,0,0,100,100,SWP_NOMOVE|SWP_NOSIZE);
					SetActiveWindow( H );
					ShowWindow( H , SW_RESTORE );
					Sleep(50);
					SetWindowPos( H ,HWND_NOTOPMOST,0,0,100,100,SWP_NOMOVE|SWP_NOSIZE);
					}
				return 0;
			}else{
				}
		}else{
			HWND H;
			if( (H = FindWindow( APPNAME , InsectaFilename ) )  )
			{
				SetWindowPos( H ,HWND_TOPMOST,0,0,100,100,SWP_NOMOVE|SWP_NOSIZE);
				SetActiveWindow( H );
				ShowWindow( H , SW_RESTORE );
				Sleep(50);
				SetWindowPos( H ,HWND_NOTOPMOST,0,0,100,100,SWP_NOMOVE|SWP_NOSIZE);
				return 0;
			}else{
				}
			}
		InsectaRunMode = Startup.GetStartupCommand(InsectaFilename);
		if(reg)
		{
			char *go=NULL;
			if( strstr(reg,"WorkSpaceFolder") )
			{
				char  temp_buff[556];
				char   buff[256];
				GetCurrentDirectory(  255 , buff );
				wsprintf( temp_buff,"Explorer.exe  %s" , Project.Env->CurrentDir  );
				WinExec( temp_buff , SW_NORMAL );
				return 0;
				}
			char  cmd_buff[1600];
			if( strstr(reg,"RunButton") )
			{
				InsectaRunMode= RUN_BUTTON_COMMAND;
				wsprintf( cmd_buff, "RUN_BUTTON" );
				LoadExecuteKeyTool( InsectaFilename, cmd_buff );
				return 0;
			}else   if( strstr(reg,"CompileButton")  ) {
				InsectaRunMode= COMPILE_BUTTON_COMMAND;
				wsprintf( cmd_buff, "COMPILE_BUTTON" );
				LoadExecuteKeyTool( InsectaFilename, cmd_buff );
				return 0;
			}else   if( strstr(reg,"BuildButton")  ) {
				InsectaRunMode= BUILD_BUTTON_COMMAND;
				wsprintf( cmd_buff, "BUILD_BUTTON" );
				LoadExecuteKeyTool( InsectaFilename, cmd_buff );
				return 0;
			}else   if( strstr(reg,"HelpButton")  ) {
				InsectaRunMode= HELP_BUTTON_COMMAND;
				wsprintf( cmd_buff, "HELP_BUTTON" );
				LoadExecuteKeyTool( InsectaFilename, cmd_buff );
				return 0;
				}
			if( strstr(reg,"Goto") )
			{
				InsectaRunMode=0;
				flags=0;
				if( cmdline && (go=strstr(reg, "Goto"))   )
				{
					go+=(strlen("Goto")+1);
					sscanf(go, "%d %d",&GotoLine , &GotoZoom );
					strcpy(GotoMsg,go);
					strncat(GotoMsg, go, 298 );
					
					nCmdShow=SW_SHOW;
					if( strcmp(reg,"OpenGoto") == 0  )
					{
						}
					}
				}
			if( strstr(reg,"NewRegistration") )
			{
				nCmdShow=SW_MAXIMIZE;
				strcpy(InsectaFilename,UNTITLED);
				ClearRegFlag=TRUE;
				}
			if( strstr(reg,"Demo") )
			{
				nCmdShow=SW_SHOW;
				MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT;
				}
			if( strstr(reg,"Run") )
			{
				InsectaRunMode=RUN_COMMAND;
				flags=0;
				}
			if( strstr(reg,"Applet") )
			{
				InsectaRunMode= APPLET_COMMAND;
			}else{
				InsectaRunMode=0;
				flags=0;
				}
			if( strstr(reg,"Document") )
			{
				InsectaRunMode=DOCUMENT_COMMAND;
				flags=0;
				}
			if( strstr(reg,"Print") )
			{
				InsectaRunMode=PRINT_COMMAND;
				flags=0;
				}
			if( strstr(reg,"Open") )
			{
				InsectaRunMode=0;
				flags=0;
				}
			if( strstr(reg,"Clone") )
			{
				InsectaRunMode=CLONE_COMMAND;
				SetLongFileName( UNTITLED );
				flags=0;
				//DEBUG
				}
			}
		}
	else
		{
		nCmdShow=SW_SHOW;
		}
	if (!hPrevInstance)
	{
		if (!InitApplication(hInstance))
		{
			return FALSE;
			}
		}
	RECT r ={ CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT};
	if (!InitInstance(hInstance, nCmdShow, &r ))
	{
		return FALSE;
		}
	if( nCmdShow==SW_MAXIMIZE  || flags   )
	{
		CreateDialog(0,"STARTUPBANNER",MainWindow,(DLGPROC)_StartBanner);
		}
	if( GotoLine )
	{
		PostMessage( MainWindow ,WM_COMMAND, GOTO_LINE ,  GotoLine );
	}else{
		}
	MSG msg;
	while (  GetMessage(&msg, NULL, 0, 0)  )
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	
	return msg.wParam;
	}
//  Export  Date: 12:09:45 PM - 05:Dec:2020;

