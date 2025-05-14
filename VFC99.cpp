/* includes */   // FILE:winPROC.h;
   // Main Windows Entry Point for Visual Flow Coder Tool :::;
   // Main Windows Entry Point for Visual Flow Coder Tool :::;
 ///::;
  #include <windows.h> 
  #include "insecta.h" 
  #include "globals.h" 
  #include "about.h" 
   // #include "winproc.h";
   // #include "getreg.h";
   // #include "project.h";
   // #include "environ.h";
  #include "InsectaStartup.h" 
  #include "winproc_externs.h"
/* globals */  
  int TimeExpired(char*, char*,char*); 
  int  ClearRegFlag   = FALSE; 
  int  InsectaRunMode = FALSE; 
  class InsectaStartup Startup;
  char GotoMsg[300];
  int GotoZoom=0;
  char  vfc99_directory[356];
/* externals */  
  void  ParseAndExecuteTool( char *cmd );
  extern char InsectaFilename[255]; 
  extern HWND hICodeDlg; 
  extern HWND hObjPropDlg; 
  extern HWND MainWindow; 
/* defines */
  #define TRACKING    0 // #define TRACKING    1 ;
  #define MESSG       {MessageBox(MainWindow,"Debug","Break",0);}
  #define TRACK // 
  #define TRACK // 
/* prototypes */ 
  int SwapMacro( char * line , char * macro, char *swap ); // defined in FILE: saveexport.ins ;

int GetKeyFileCommand( char * key , char * cmd_buffer  ){ // this replaces cmd_buffer with the tool refernce found in the key.  return 0  if no tool or entry;
  #define  MAXLINE_SIZE   500
  #define  KEYFILE_TARGET   cmd_buffer
  char  keyfile[600];
  wsprintf( keyfile,"%s\\%s", Project.Env->LibraryDir, key );
  FILE *fp = fopen( keyfile , "r"  );
  char string[MAXLINE_SIZE];
   // TRACK MessageBox( hWnd , keyfile , " Pman opening ", MB_OK);;
   // TRACK MessageBox( hWnd , key , " Pman LOOKING ", MB_OK);;
  if( fp )
  {  
    while(  fgets( string, MAXLINE_SIZE , fp )  ) {
      if( strstr( string , KEYFILE_TARGET )  )
      { //  yes this keyfile is listed ;
        fgets( string, MAXLINE_SIZE , fp );
        strcpy( cmd_buffer , string );
        fclose( fp );
        if( *string  )
        { //  yes this keyfile is listed ;
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
int   GetKeyWordFilter( char * file, char *key_buffer ){ // scans the file for the key file definition and loads it in key_buffer -;
  #define  MAXLINE_SIZE   500
  FILE *fp = fopen( file , "r"  );
  char string[MAXLINE_SIZE];
  *key_buffer = '\0';
  if( fp )
  {  
    while(  fgets( string, MAXLINE_SIZE , fp )  ) {
      if( strstr( string , ";INSECTA EMBEDDED SESSION INFORMATIO" )  )
      { //  yes this keyfile is listed ;
        fgets( string, MAXLINE_SIZE , fp );
        fgets( string, MAXLINE_SIZE , fp );
        char  *tok = strtok( string , " ;\t" );
        strcpy( ExportFileName , tok  );
        wsprintf( Project.ExportName , ExportFileName );
         // MessageBox( 0, ExportFileName , " ExportFileName ", MB_OK);;
        while(  fgets( string, MAXLINE_SIZE , fp )  ) {
          if( strstr( string , ";INSECTA EMBEDDED ALTSESSION INFORMATIO" )  )
          { //  yes this keyfile is listed ;
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
  char key_buffer[600];
  GetKeyWordFilter( file, key_buffer ); // scans the file for the key file definition and loads it in key_buffer -;
  GetKeyFileCommand( key_buffer , tool ); // reads the tool sting and then overwrites its the tool sting with what it finds in the key_buffer;
  if( *tool ) // a tool was loaded;
  { 
    char *s=strrchr( tool , '|' );
    if( s )
    { 
      *s = NULL;
    } 
    int flag = 0;
    flag += SwapMacro( tool , "<ExportFileName" , ExportFileName ); // defined in FILE: saveexport.ins ;
    flag +=SwapMacro( tool , "<InsectaFileName" , InsectaFilename ); // defined in FILE: saveexport.ins ;
    flag +=SwapMacro( tool , "<CurrentObject" , InsectaFilename ); // defined in FILE: saveexport.ins ;
    flag +=SwapMacro( tool , "<CurrentDir" , Project.Env->CurrentDir ); // defined in FILE: saveexport.ins ;
     // MessageBox( MainWindow , tool , " debug ", MB_OK);;
    char *tok=strstr( tool , ">." );
     // MessageBox( MainWindow , tool , " debug ", MB_OK);;
    if( tok && flag )
    { 
      char str[100]; 
      s= tok;
      while( s>tool && *s!='<' && *s!='.'  ) {
      s--; }
      if( *s=='.'  )
      { 
         // MessageBox( 0 , tool , " if ", MB_OK);;
        *(tok+1) = NULL;
        strncpy( str , s , 99 );
        *(tok+1) = '.' ;
        if( *(tok+2) == ' ' || *(tok+2) == '\t'  )
        { 
          strcat( str , "."  ); // set up to null out a >. with no extension;
        } 
        SwapMacro( tool , str , "" ); // defined in FILE: saveexport.ins ;
      }else{ 
         // MessageBox( 0 , tool , " else ", MB_OK);;
        SwapMacro( tool , ">." , "" ); // defined in FILE: saveexport.ins ;
      } 
    }else if(flag) {
      SwapMacro( tool , ">" , "" ); // defined in FILE: saveexport.ins ;
    } 
     // MessageBox( 0 , tool , " tool ", MB_OK);;
    WinExec(tool,SW_NORMAL); // ParseAndExecuteTool( tool );;
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
char ProgramName[100] = "VFC99.exe"; // char ProgramName[100] = "insecta2.exe";;
int GotoLine;
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) //  ... main entry point for the windows program;
  { 
  char *cmdline, *flags=NULL; 
  hInst=hInstance; 
  char *file_spec=vfc99_directory;
  *vfc99_directory = NULL;
  SearchPath( NULL, "vfc99.exe" , NULL, 355 ,  vfc99_directory , &file_spec );  
  if( file_spec && *file_spec )
  { 
    *--file_spec = NULL; // CUT OFF THE NAME TO GET A FULL DIR PATH;
  } 
   // Project.Env->SetDefaults();;
   // MessageBox( NULL , vfc99_directory , " VFC99 ", MB_OK);;
  #ifdef DEMO
  
    MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT ; 
  #endif 
  if(lpCmdLine && *lpCmdLine) 
  { 
    char *reg =NULL; //  this is the pointer to the optional -/ command  ... NOTE '-' char must be seen AFTER .ins and BEFORE the '\' dir delimiter in DOS;
    char *ptr  = lpCmdLine; 
    while( SwapMacro( lpCmdLine , "\"" , "")  ) { // trim any double quotes;
    } 
    char * d1=strstr(lpCmdLine,".ins"); //  POINT TO EXTENSIONS AND POSSIBLE - COMMAND;
    if(  d1 )
    { 
      ptr = ++d1;
    } 
     // d1=strstr(lpCmdLine,".ins\t");;
     // if(  d1 );
     //  { ;
       // ptr = ++d1;;
     //  } ;
    flags=strrchr(lpCmdLine,'-'); // flags=strchr( ptr ,'-');;
    if( flags  && !strchr( flags, '\\')  ) //  decide if this is a parma or part of the path by seeing that NO '\' s exist;
    { 
      reg=strtok(++flags," "); 
    }else{ 
      flags=0;
    } 
    strtok( ptr ,"-"); //  cmdline=strtok(lpCmdLine,"-");/  ;
    cmdline=lpCmdLine; //  cmdline=strtok(lpCmdLine,"-");/  ;
    if(cmdline && *cmdline!='-' && strlen(cmdline)   ) //  allow a -Run to fire a file with exisiting window name;
    { 
      char path[300], *dir = strrchr(cmdline, '\\');
      if ( dir )
      {
        *dir=NULL;
        wsprintf( path, "%s\\",cmdline );
        SetCurrentDirectory(path);
        GetCurrentDirectory(255, Project.Env->CurrentDir);
        cmdline = ++dir;
      }
      char *tok = cmdline +strlen(cmdline ) - 1; // TAKE OFF TRAILING SPACES;
      while( tok > cmdline && iswhite(*tok) ){
        *tok = NULL; 
      tok--; } 
      strcpy(InsectaFilename,cmdline); 
      char flowfile_name[300];
      wsprintf( flowfile_name ,"%s.ins" , InsectaFilename ); /// Auto Open a flowfile ..... 10/01;
      FILE *fptr = fopen(flowfile_name,"r");
      if(  fptr ) // see if a flowfile exisits ... ;
      { // reset to the flowfile if it is found;
        fclose( fptr );
         // MessageBox( NULL , flowfile_name  , " VFC99 IF", MB_OK);;
        wsprintf( InsectaFilename,"%s" , flowfile_name );
      }else{ // .. look for a flat .ins name;
        char *insfile = strrchr( InsectaFilename, '.' );
        if( insfile )
        { 
          *insfile = '\0';
          wsprintf( flowfile_name ,"%s.ins" , InsectaFilename ); /// Auto Open a flowfile ..... 10/01;
           // MessageBox( NULL , flowfile_name  , " VFC99 IF", MB_OK);;
          FILE *fptr = fopen(flowfile_name,"r");
          if(  fptr ) // see if a flowfile exisits ... ;
          { // reset to the flowfile if it is found;
            fclose( fptr );
            wsprintf( InsectaFilename,"%s" , flowfile_name );
          }else{ // .. look for a flat .ins name;
          } 
        }else{ 
        } 
      } 
      // END OF NEW AUTO OPEN FLOWFILE FIX
      SetLongFileName(InsectaFilename);
      HWND H; 
      if(  (H = FindWindow( APPNAME , InsectaFilename )  ) && ! strstr( InsectaFilename,"Run")  )
      {  
        char *go=NULL;
        if( reg &&  strstr(reg,"Goto") ) 
        { 
          InsectaRunMode=0; 
          flags=0; // Keeps the window from opening big;
          if( cmdline && (go=strstr(reg, "Goto"))   )
          {  
            go+=(strlen("Goto")+1);
            sscanf(go, "%d %d",&GotoLine , &GotoZoom ); // restore insecta cmd line;
            *GotoMsg=NULL;
            strncat(GotoMsg, &go[4], 298 );
             // strcat(GotoMsg, "");;
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
      char  cmd_buff[600];
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
        flags=0; // Keeps the window from opening big;
        if( cmdline && (go=strstr(reg, "Goto"))   )
        {  
          go+=(strlen("Goto")+1);
          sscanf(go, "%d %d",&GotoLine , &GotoZoom ); // restore insecta cmd line;
          strcpy(GotoMsg,go);
          strncat(GotoMsg, go, 298 ); // strncat(GotoMsg, &go[4], 298 );;
           // strcat(GotoMsg, "");;
          nCmdShow=SW_SHOW; 
          if( strcmp(reg,"OpenGoto") == 0  )
          {  
          }  
        }  
      } 
      if( strstr(reg,"NewRegistration") ) 
      { 
        nCmdShow=SW_MAXIMIZE; 
        strcpy(InsectaFilename,"untitled.ins"); 
        ClearRegFlag=TRUE; 
      } 
      if( strstr(reg,"Demo") ) 
      { 
        nCmdShow=SW_SHOW; // nCmdShow=SW_MAXIMIZE;;
        MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT; 
      } 
      if( strstr(reg,"Run") ) 
      { 
        InsectaRunMode=RUN_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Applet") ) 
      { 
        InsectaRunMode= APPLET_COMMAND; 
      }else{ 
        InsectaRunMode=0; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Document") ) 
      { 
        InsectaRunMode=DOCUMENT_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Print") ) 
      { 
        InsectaRunMode=PRINT_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Open") ) 
      { 
        InsectaRunMode=0; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Clone") ) 
      { 
        InsectaRunMode=CLONE_COMMAND; 
        SetLongFileName( "untitled.ins" );
        flags=0; // Keeps the window from opening big;
        //DEBUG // MessageBox( 0 ,InsectaFilename , " title ", MB_OK);;
      } 
    }  // if(reg) ;
    }  
  else 
    { 
    nCmdShow=SW_SHOW; // nCmdShow=SW_MAXIMIZE;;
  }  // if lpCmdLine ;
  if (!hPrevInstance) 
  { 
    if (!InitApplication(hInstance)) 
    { 
      return FALSE; 
    }  // endif ;
  }   // endif ;
  RECT r ={ CW_USEDEFAULT,CW_USEDEFAULT, 550, 500 };
  if (!InitInstance(hInstance, nCmdShow, &r )) 
  { 
    return FALSE;
  } 
  #ifdef TIMELIMIT
  
    if( RegObject.TimeExpired("April","May","424216") ) 
    { 
      MessageBox(MainWindow,"The time limit on this evaluation copy has expired - VFC will only run in DEMO mode now.  
	  Please contact FlowLynx to purchase an unrestricted license.","Time Limit Expired",0); 
      MAX_DEMO_OBJECTS=NUM_OBJECTS_LIMIT ; 
    } 
  #endif
  if( nCmdShow==SW_MAXIMIZE  || flags )   
  { 
    CreateDialog(0,"STARTUPBANNER",MainWindow,(DLGPROC)_StartBanner); 
  } 
  if( GotoLine )
  {  
    PostMessage( MainWindow ,WM_COMMAND, GOTO_LINE ,  GotoLine );
  }  
  MSG msg; 
  while (  GetMessage(&msg, NULL, 0, 0)  ) 
    { 
    TranslateMessage(&msg); 
    DispatchMessage(&msg); 
  } 
  return msg.wParam; 
} 
int APIENTRY __WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) //  ... main entry point for the windows program;
  { 
  char *cmdline, *flags=NULL; 
  hInst=hInstance; 
  #ifdef DEMO
  
    MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT ; 
  #endif 
  if(lpCmdLine && *lpCmdLine) 
  { 
    char *reg =NULL; 
    char *ptr  = lpCmdLine; 
    char * d1=strstr(lpCmdLine,".ins "); 
    if(  d1 )
    { 
      ptr = ++d1;
    } 
    d1=strstr(lpCmdLine,".ins\t"); 
    if(  d1 )
    { 
      ptr = ++d1;
    } 
    flags=strchr( ptr ,'-'); //  flags=strrchr(lpCmdLine,'-');;
    strtok( ptr ,"-"); //  cmdline=strtok(lpCmdLine,"-");/  ;
    cmdline=lpCmdLine; //  cmdline=strtok(lpCmdLine,"-");/  ;
    if( flags ) 
    { 
      reg=strtok(++flags," "); 
    } 
    if(cmdline && *cmdline!='-' && strlen(cmdline)   ) //  allow a -Run to fire a file with exisiting window name;
    { 
      char path[300], *dir = strrchr(cmdline, '\\');
      if ( dir )
      {
        *dir=NULL;
        wsprintf( path, "%s\\",cmdline );
        SetCurrentDirectory(path);
        GetCurrentDirectory(255, Project.Env->CurrentDir);
        cmdline = ++dir;
      }
      char *tok = cmdline +strlen(cmdline ) - 1; // TAKE OFF TRAILING SPACES;
      while( tok > cmdline && iswhite(*tok) ){
        *tok = NULL; 
      tok--; } 
      strcpy(InsectaFilename,cmdline); 
      SetLongFileName(InsectaFilename);
      HWND H; 
      char *go=NULL;
      int GotoLine  = 0 , GotoZoom = 0,  GotoShiftX = 0,  GotoShiftY = 0 ;
      if( cmdline && (go=strstr(reg, "Goto"))   )
      {  
        go+=(strlen("Goto")+1);
        sscanf(go, "%d %d %d",&GotoLine , &GotoZoom, GotoShiftX, GotoShiftY ); // restore insecta cmd line;
        *GotoMsg=NULL;
        strncat(GotoMsg, &go[4], 298 );
         // strcat(GotoMsg, "");;
      }  
      if(  (H = FindWindow( APPNAME , InsectaFilename )  ) && ! strstr( InsectaFilename,"Run")  )
      {  
        if( GotoLine ) 
        { 
          SendMessage( H , WM_COMMAND , GOTO_LINE , GotoLine );  
          if( GotoZoom > 0  )
          { 
            SendMessage( H , WM_COMMAND , ID_FONT_LARGER, 0 );  
          }else if(  GotoZoom < 0 ) {
            SendMessage( H , WM_COMMAND , ID_FONT_SMALLER , 0 );  
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
      char  cmd_buff[600];
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
      if( strstr(reg,"NewRegistration") ) 
      { 
        nCmdShow=SW_SHOW; //  nCmdShow=SW_MAXIMIZE;;
        strcpy(InsectaFilename,"untitled.ins"); 
        ClearRegFlag=TRUE; 
      } 
      if( strstr(reg,"Demo") ) 
      { 
        nCmdShow= SW_SHOW ; //  nCmdShow=SW_MAXIMIZE;;
        MAX_DEMO_OBJECTS = NUM_OBJECTS_LIMIT; 
      } 
      if( strstr(reg,"Run") ) 
      { 
        InsectaRunMode=RUN_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Applet") ) 
      { 
        InsectaRunMode= APPLET_COMMAND; 
      }else{ 
        InsectaRunMode=0; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Document") ) 
      { 
        InsectaRunMode=DOCUMENT_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Print") ) 
      { 
        InsectaRunMode=PRINT_COMMAND; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Open") ) 
      { 
        InsectaRunMode=0; 
        flags=0; // Keeps the window from opening big;
      } 
      if( strstr(reg,"Clone") ) 
      { 
        InsectaRunMode=CLONE_COMMAND; 
        SetLongFileName( "untitled.ins" );
        flags=0; // Keeps the window from opening big;
        //DEBUG // MessageBox( 0 ,InsectaFilename , " title ", MB_OK);;
      } 
    }  // if(reg) ;
    }  
  else 
    { 
    nCmdShow=SW_SHOW; //  nCmdShow=SW_MAXIMIZE;;
  }  // if lpCmdLine ;
  if (!hPrevInstance) 
  { 
    if (!InitApplication(hInstance)) 
    { 
      return FALSE; 
    }  // endif ;
  }   // endif ;
  RECT r ={ CW_USEDEFAULT,CW_USEDEFAULT, 550, 500 };
  if (!InitInstance(hInstance, nCmdShow, &r )) 
  { 
    return FALSE;
  } 
  #ifdef TIMELIMIT
  
    if( RegObject.TimeExpired("April","May","62596") ) 
    { 
      MessageBox(MainWindow,"The time limit on this evaluation copy has expired - FlowCoder will only run in DEMO mode now.  
	  Please contact FlowLynx at www.flowlynx.com to purchase a license.","Time Limit E
      MAX_DEMO_OBJECTS=NUM_OBJECTS_LIMIT ; 
    } 
  #endif
  if(  ( nCmdShow==SW_MAXIMIZE  || flags )  && (InsectaRunMode!= APPLET_COMMAND || InsectaRunMode!= RUN_COMMAND)  ) 
  { 
    CreateDialog(0,"STARTUPBANNER",MainWindow,(DLGPROC)_StartBanner); 
  } 
  MSG msg; 
  while (  GetMessage(&msg, NULL, 0, 0)  ) 
    { 
    TranslateMessage(&msg); 
    DispatchMessage(&msg); 
  } 
  return msg.wParam; 
} 
//  FlowCode File: VFC99.ins;
//  Export  File: VFC99.cpp;
//  Export  Date: 12:35:28 AM - 07:Jun:2009;

