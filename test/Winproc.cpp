/*  Winproc.cpp  */
	
	#include <time.h>
	#include <sys\utime.h>
	#include <sys\types.h>
	#include <sys\stat.h>
	#include <windows.h>
	#include <windowsx.h>
	#include <stdio.h>
	#include <commdlg.h>
	#include <commctrl.h>
	
	#include "about.h"
	#include "environ.h"
	#include "project.h"
	#include "globals.h"
	#include "obj_parser.h"
	#include "Winproc.h"
	#include "res_extra.h"
	#include "insecta_objs.h"
	#include "variables.h"
	#include "interpreter.h"
	#include "print.h"
	#include "StdLIB.h"
	#include "generator.h"
	#include "editing.h"
	#include "editvars.h"
	#include"ProcessToolbarTips.h"
	
	#include "save.h"
	#include "FuncNames.h"
	#include "Simulator.h"
	#include "getreg.h"
	#include "undo_redo.h"
	#include "analysis.h"
	#include "pathview.h"
	#include "console.h"
	#include "BookMarkDlg.h"
/* defines */
#define  SECURITY_CODE  "secure_code=MY_DOG_PEES_IN_THE_HOUSE"
#define TRACKING   0
	#define TRACK
	#define VERSION_INFO "3.06 Version "
	#define ID_AUTO_PARSE_AND_LOAD_KEYLESS  235
	#define ID_GET_FILE_TIMES 234
	#define ID_AUTO_PARSE_AND_LOAD 233
	#define ID_GETKEYWORDFILTER 232
	#define ID_ASKINSECTA 231
	#define PAGE_UP 33
	#define PAGE_DOWN 34
	#define CURSOR_LEFT 37
	#define CURSOR_UP 38
	#define CURSOR_RIGHT 39
	#define CURSOR_DOWN 40
	#define DEL_KEY 46
	#define HOME_KEY 36
	#define END_KEY 35
	#define BACK_SPACE 8
	#define ESCAPE_KEY 127
	#define START_FONT_SIZE 16
/* ToolBar DLL  externals */
	void  AlignToolBars();
/* externals */
	extern  HWND h ;
	extern char GotoMsg[300];
	extern class InsectaObject *ObjectList;
	extern class Variables *Vlist;
	extern class StdLIB InsectaLibrary;
	extern class InsectaObject *ProgramCounterObject;
	extern int tracers;
	extern int new_type;
	extern HWND hICodeDlg;
	HWND I_code_hwnd;
	extern HANDLE comments_pen;
	extern HANDLE trace_pen;
	extern HANDLE trace_brush;
	extern HANDLE undef_pen;
	extern HANDLE break_pen;
	extern HANDLE break_brush;
	extern HANDLE current_pen;
	extern HANDLE current_brush;
	extern HANDLE error_pen;
	extern HANDLE interp_pen;
	extern HANDLE diagram_pen;
	extern HANDLE thick_diagram_pen;
	extern HANDLE background_pen;
	extern HANDLE background_brush;
	extern HANDLE select_pen;
	extern HANDLE select_brush;
	extern HANDLE TbarID;
	extern HANDLE VbarID;
	extern HANDLE DbarID;
	extern int DIAGRAM_THICKNESS;
	extern int BuffUndoPtr;
/* globals */
	int FileAutoParsed =0;
	int MAX_DEMO_OBJECTS=-1;
	int showtools=TRUE;
	extern int ClearRegFlag;
	class ProjectClass Project;
	class WindowProcess WinprocObject;
	class FunctionNames FuncNames;
	COLORREF colors[16];
	class InsectaObject *CurrentObject;
	HFONT winfont=NULL;
	int nobjs=0;
	int fontwidth=START_FONT_SIZE;
	char InsectaFilename[255]="";
	int sounds=FALSE;
	int TraceMode=FALSE;
	HWND IMC232Window=NULL;
	int show_comments=FALSE;
	int export_indented=TRUE;
	int ExportAuto=FALSE;
	int Backup=TRUE;
	char ExportFileName[257]="";
	char PostExportCommand[512]="";
	int FileSaved=TRUE;
	HWND TreeView=NULL;
	int createtree=TRUE;
	int RunHidden=FALSE;
	HWND hObjPropDlg=NULL;
	HWND hStatusBar;
	BOOL AreaSelectFlag=FALSE;
	RECT SelectRect;
	int SelectAnchorX=0;
	int SelectAnchorY=0;
	int xPos;
	int yPos;
	char tmp_buff[600];
	char old_name[256] ="";
	HDC hDC;
	PAINTSTRUCT ps;
	int LastX=0,LastY=0;
	int tracktimes=0;
	int TbarPosX=0,TbarPosY=0;
	int mouse_button_down =FALSE;
	extern int ExportTime;
	double scale_factor =1.0;
	class RegisterInsecta RegObject;
	
/* defines for VFC shared mem modules */
	#define VFC_SHARED_MEMORY_DATA "{06B0F857-50F0-11d2-9213-00207814C083}"
	
int ReadSharedMemory( char * str ){
	int rtn = 0;
	HANDLE  hFile = OpenFileMapping( FILE_MAP_READ , 0 , VFC_SHARED_MEMORY_DATA );
	char *mfile  = (char*) MapViewOfFile( hFile , FILE_MAP_READ , 0 ,  0 , 0 );
	if( mfile )
	{
		strcpy( str , mfile );
		rtn = 1;
	}else{
		rtn = 0;
		}
	CloseHandle( hFile );
	return rtn;  }
int  WriteSharedMemory( LPSTR string )  {
	int rtn = 0;
	HANDLE hFile  = CreateFileMapping( (HANDLE)0xFFFFFFFF , NULL , PAGE_READWRITE , 0 , 355  ,  VFC_SHARED_MEMORY_DATA  );
	char * mfile  = (char*)MapViewOfFile( hFile , FILE_MAP_WRITE , 0 ,  0 , 0 );
	if( mfile )
	{
		strncpy( mfile, string , 354 );
		rtn = 1;
	}else{
		rtn = 0;
		}
	return rtn;  }
int GetTimeDifference( char * InsectaFileName , char * ExportFileName )
	{
	double realtime;
	int Dtime =-1 , r1 , r2;
	struct _stat InsFile, ExpFile;
	r1 =_stat( InsectaFileName , &InsFile );
	r2 =_stat( ExportFileName , &ExpFile );
	if( r1 != 0 || r2 !=0 )
	{
		return 0;
		
		
	} realtime =difftime( InsFile.st_mtime , ExpFile.st_mtime ) ;
	if( realtime >= 0 )
	{
		return 0;
		
		}
	else
		{
		return -1;
		
		
	} Dtime =(int)(10*realtime) ;
	return Dtime ;
}
void LoadWindowsWordAnswer( HWND hWnd, char * str )
	{
	int index=0;
	while( index < 30 ) {
		SetWindowWord( hWnd , index , (WORD)*str );
		index++;
		str ++;
		}
	
	}
#define  DX  10
#define  DY  10
void JoyMoveFlowCode(DWORD lParam)
	{
	WORD x, y;
	POINT pt = { 0, 0 };
	
	
	
	
	
	
	
	x =LOWORD(lParam) >> 11;
	y =HIWORD(lParam) >> 11;
	
	
	
	
	
	if(x <= 12)
	
		pt.x = - DX;
	else
		if(x >= 20)
		
			pt.x =  DX;
			
		
	if(y <= 12)
	
		pt.y =  - DY;
	else
		if(y >= 20)
		
			pt.y =  DY;
			
		
	
	if(ObjectList)
	{
		ObjectList->shift( (x-15)*DX , (y-15)*DY );
		}
	InvalidateRect(MainWindow,NULL,TRUE);
	}
/* defines for VFC shared mem modules */
	#define VFC_SHARED_MEMORY_DATA "{06B0F857-50F0-11d2-9213-00207814C083}"
	#define  VFCREMOTECALL     11223334
	#define  NAME_OF_VS_COM_TOOL  "VFC Add-in Command Tool"
	
RECT LastWindowR ;
LRESULT CALLBACK _WndProc(HWND hWnd,UINT uMessage,WPARAM wParam,LPARAM lParam)
	{
	static int draw_tools =TRUE;
	MainWindow=hWnd;
	if(AreaSelectFlag)
	{
		SetCursor(LoadCursor(NULL,IDC_CROSS));
		
	} WinprocObject.CatchSimEvents(hWnd,uMessage,wParam,lParam);
	switch (uMessage){
	case WM_MOVE: {
		RECT r , tr;
		if( EditObject.hObjectBrowser  )
		{
			GetWindowRect( EditObject.hObjectBrowser , &tr );
			GetWindowRect( hWnd , &r );
			SetWindowPos( EditObject.hObjectBrowser , HWND_NOTOPMOST , r.left - LastWindowR.left + tr.left , r.top- LastWindowR.top + tr.top , 10 , 10 , SWP_NOSIZE | SWP_NOACTIVATE  );
			}
		if( hToolBar )
		{
			GetWindowRect( hToolBar , &tr );
			GetWindowRect( hWnd , &r );
			SetWindowPos( hToolBar  , HWND_NOTOPMOST , r.left - LastWindowR.left + tr.left , r.top- LastWindowR.top + tr.top , 10 , 10 , SWP_NOSIZE | SWP_NOACTIVATE  );
			
			}
		if( hObjPropDlg )
		{
			GetWindowRect( hObjPropDlg, &tr );
			GetWindowRect( hWnd , &r );
			SetWindowPos( hObjPropDlg , HWND_NOTOPMOST , r.left - LastWindowR.left + tr.left , r.top- LastWindowR.top + tr.top , 10 , 10 , SWP_NOSIZE | SWP_NOACTIVATE  );
			
			}
		BookMarks[0].FollowMainWindow(  LastWindowR   );
		GetWindowRect( hWnd , &LastWindowR );
		return 0;
		}
	case WM_COMMAND :
		{
		int    keyless =0;
		switch(wParam) {
		case ID_AUTO_PARSE_AND_LOAD_KEYLESS  :
			TRACK  keyless = TRUE;
		case ID_AUTO_PARSE_AND_LOAD :
			{
			TRACK    char buff[512];
			char parser[100] ="";
			_utime( InsectaFilename , NULL );
			
			if( lParam )
			{
				GetWindowText( (HWND)lParam, parser, 100 );
				char * s =strrchr( parser, '-' );
				if( s )
				{
					*s =0;
					
				}
				
			}
			TRACK if( !*parser )
			{
				TRACK MessageBox( hWnd , "Parser Not Loaded. " , "VFC", MB_OK);
				}
			else
				{
				if( keyless )
				{
					TRACK   wsprintf( buff ,"%s\\parse.exe %s %s %d  %d  %s %s", Project.Env->GeneratorDir, ExportFileName, parser, (LPARAM)lParam , hWnd , "Keyless" ,  SECURITY_CODE  );
				}else{
					TRACK   wsprintf( buff ,"%s\\parse.exe %s %s %d  %d  UseKeys   %s", Project.Env->GeneratorDir, ExportFileName, parser, (LPARAM)lParam , hWnd , SECURITY_CODE  );
					}
				FileAutoParsed =TRUE ;
				WinExec( buff , SW_HIDE );
				
				
			} break;
			
			}
		case ID_GETKEYWORDFILTER :
			{
			char str[100];
			int FileDeltaT =GetTimeDifference( InsectaFilename , ExportFileName );
			wsprintf( str , "%s:%d", FuncNames.KeyWordFilter , FileDeltaT );
			str[30]=0;
			LoadWindowsWordAnswer( hWnd , str );
			break;
			
			}
		case ID_ASKINSECTA :
			{
			LoadWindowsWordAnswer( hWnd , VERSION_INFO );
			break;
			
			}
		case ID_GETEXPORTFILE :
			{
			LoadWindowsWordAnswer( hWnd , ExportFileName );
			break;
			
			}
		case ID_GETCURRENTDIR  :
			{
			LoadWindowsWordAnswer( hWnd , Project.Env->CurrentDir );
			WriteSharedMemory( Project.Env->CurrentDir  )  ;
			break;
			
			}
		case ONTOP :
			SetWindowPos( (HWND)lParam , HWND_TOPMOST , 0,0,100,100,SWP_NOMOVE|SWP_NOSIZE);
			break;
			
		case IDC_RELOADKEYS :
			FuncNames.Reload( hWnd );
			break;
		case IDC_EDITKEYFILE : {
			class Environment Env(NULL);
			char   buff[556];
			wsprintf(buff,"NotePad.exe \"%s\\%s\"", Env.LibraryDir, FuncNames.KeyWordFilter );
			WinExec( buff , SW_NORMAL );
			break; }
			break;
		case UPDATE :
			WinprocObject.UpdateStatusBar();
			break;
			
		case UPDATE_KEYNAME :
			/*
			if( wParam  )
			{
				HANDLE  hFile = OpenFileMapping( FILE_MAP_READ , 0 , VFC_SHARED_MEMORY_DATA );
				char *mfile  = (char*) MapViewOfFile( hFile , FILE_MAP_READ , 0 ,  0 , 0 );
				strcpy( FuncNames.KeyWordFilter, mfile  );
				FuncNames.Reload( hWnd );
				CloseHandle( hFile );
				WinprocObject.UpdateStatusBar();
				}
			*/
			break;
			
		case GOTO_LINE :
			if( IsIconic( MainWindow ) )
			{
				ShowWindow( MainWindow,SW_RESTORE );
				
			} ShowWindow( MainWindow,SW_SHOW );
			{
			
			
			
			}
			SetWindowPos( hWnd, HWND_TOPMOST , 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
			EditObject.ShowStatement( (int) lParam );
			SetWindowPos( hWnd, HWND_NOTOPMOST , 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
			if( *GotoMsg )
			{
				WinprocObject.StatusBarMessage( 2 , GotoMsg );
				*GotoMsg =NULL;
				UpdateWindow(hStatusBar);
				
			} break;
			
		default:
			switch( HIWORD(wParam) ){
			case LBN_DBLCLK:
				PostMessage( EditObject.hObjectBrowser, WM_COMMAND, IDC_OBJ_VIEWER_JUMP , 0 );
				break;
				
			default:
				int rtnvalue =WinprocObject.ProcessCommand(hWnd,uMessage,wParam,lParam);
				if(rtnvalue)
				{
					return rtnvalue;
					
					}
				}
			
			break;
			
			}
		
		} break;
		
	case WM_CLOSE:
		if(!FileSaved || FileAutoParsed )
		{
			int choice =MessageBox(hWnd,"Would you like to save the current diagram?","VFC",MB_YESNOCANCEL);
			switch( choice ) {
			case IDYES:
				SendMessage(hWnd,WM_COMMAND,ID_FILE_SAVE,NULL);
				break;
				
			case IDCANCEL:
				return 0;
				
				}
			
			
		} DestroyWindow(hWnd);
		return 0;
		
	case WM_CREATE:
		WinprocObject.Create(hWnd,uMessage,wParam,lParam);
		GetWindowRect( hWnd , &LastWindowR );
		FuncNames.Reload( MainWindow );
		
		if(!FuncNames.num_tools )
		{
			EnableMenuItem( GetMenu(MainWindow) , ID_TOOLS_CUSTOMIZE, MF_GRAYED );
			break;
		}else{
			EnableMenuItem( GetMenu(MainWindow) , ID_TOOLS_CUSTOMIZE, MF_ENABLED );
			}
		if( strcmp( InsectaFilename ,  UNTITLED  ) == 0  )
		{
			
			CreateDialog(0,"STARTUPBANNER",MainWindow,(DLGPROC)_StartBanner);
			
			
			}
		AlignToolBars() ;
		break;
		
	case WM_KEYDOWN:
		WinprocObject.KeyDown(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_DROPFILES:
		WinprocObject.DragDrop(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_NOTIFY:
		ToolBars.ProcessCustomToolTips(lParam);
		ProcessToolTips(lParam);
		ProcessDebugToolTips(lParam);
		ProcessViewToolTips(lParam);
		EditVarsObject.ProcessMessage((NMHDR*)lParam);
		break;
		
	case WM_SIZE:
		{
		static  int    MIN_FLAG=0;
		switch(wParam) {
		case SIZE_RESTORED :
			if( h )
			{
				ShowWindow( h , SW_SHOW  );
				if( IsIconic( h ) && MIN_FLAG )
				{
					MIN_FLAG=0;
					ShowWindow( h , SW_RESTORE );
					
					}
				}
			break;
		case SIZE_MINIMIZED :
			MIN_FLAG=1;
			if( h )
			{
				ShowWindow( h , SW_MINIMIZE );
				ShowWindow( h , SW_HIDE );
				}
			PostMessage( MainWindow , WM_COMMAND ,  ID_VIEW_TOOLBARS_ALLIGN , 0 );
			break;
			}
		RECT r,sb;
		GetClientRect(MainWindow,&r);
		if(hStatusBar)GetClientRect(hStatusBar,&sb);
		if(hStatusBar)SetWindowPos(hStatusBar,HWND_TOP,0,r.bottom-sb.bottom,r.left,sb.bottom,SWP_NOZORDER);
		break;
		}
	case WM_KILLFOCUS :
		{
		HWND hwndGetFocus = (HWND) wParam;
		joyReleaseCapture(JOYSTICKID1);
		break;
		}
	case WM_SETFOCUS :
		{
		HWND hwndLoseFocus = (HWND) wParam;
		if( hwndLoseFocus  )
		{
			SendMessage( hwndLoseFocus  ,WM_COMMAND, WM_KILLFOCUS , 0 );
			}
		joySetCapture(hWnd, JOYSTICKID1, 0, FALSE) ;
		break;
		}
	case WM_ACTIVATE :
		{
		RECT r,sb;
		joySetCapture(hWnd, JOYSTICKID1, 0, FALSE) ;
		GetClientRect(MainWindow , &r );
		GetClientRect(hStatusBar,&sb);
		SetWindowPos(hStatusBar,HWND_TOP,0,r.bottom-sb.bottom,r.left,r.bottom-sb.bottom+sb.bottom,SWP_NOZORDER);
		if( hStatusBar)
		
			WinprocObject.UpdateStatusBar();
			
		}
		
		InvalidateRect( MainWindow , NULL , FALSE );
		UpdateWindow ( MainWindow );
		if( ViewToolBar )
		{
			InvalidateRect( ViewToolBar , NULL , FALSE );
			UpdateWindow ( ViewToolBar );
			}
		if( DebugToolBar )
		{
			InvalidateRect( DebugToolBar , NULL , FALSE );
			UpdateWindow ( DebugToolBar );
			}
		if( hExportToolBar )
		{
			InvalidateRect( hExportToolBar , NULL , FALSE );
			UpdateWindow ( hCustomToolBar );
			}
		if( hCustomToolBar )
		{
			InvalidateRect( hCustomToolBar , NULL , FALSE );
			UpdateWindow ( hCustomToolBar );
			}
		if( hToolBar)
		{
			InvalidateRect( hToolBar , NULL , FALSE );
			UpdateWindow ( hToolBar);
			}
		break;
		
	case WM_PAINT:
		WinprocObject.Paint(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_LBUTTONDBLCLK:
		WinprocObject.LeftButtonDblClk(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_LBUTTONDOWN:
		WinprocObject.LeftMouseButtonDown(hWnd,uMessage,wParam,lParam);
		break;
		
	case MM_JOY1MOVE :
		if(wParam & (JOY_BUTTON1 | JOY_BUTTON2))
		{
			JoyMoveFlowCode( lParam );
			}
		if(wParam & (JOY_BUTTON3 ) )
		{
			SendMessage( MainWindow ,WM_COMMAND, ID_FONT_SMALLER ,  0 );
			}
		if(wParam & (JOY_BUTTON4 ) )
		{
			SendMessage( MainWindow ,WM_COMMAND, ID_FONT_LARGER ,  0 );
			}
		break;
		
	case WM_MOUSEWHEEL:
		{
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if( zDelta < 0 )
		{
			SendMessage( MainWindow ,WM_COMMAND, ID_FONT_SMALLER ,  0 );
		}else{
			if( zDelta > 0 )
			{
				SendMessage( MainWindow ,WM_COMMAND, ID_FONT_LARGER ,  0 );
				}
			}
		return 0;
		}
	case WM_MOUSEMOVE:
		WinprocObject.MouseMove(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_LBUTTONUP:
		WinprocObject.LeftMouseButtonUp(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_RBUTTONDOWN:
		WinprocObject.RightMouseButtonDown(hWnd,uMessage,wParam,lParam);
		break;
		
	case WM_TIMER:
		switch(wParam){
		case ANIMATE_TIMER:
			Break();
			if(!ExecuteStep())
			{
				Stop();
				
			}
			if(RunMode==ANIMATION && !InsectaLibrary.WaitingForEvent() )
			{
				Continue();
				
			} break;
			
		case SIMULATION_SYSTEM_TIMER:
			break;
			
			}
		
		break;
		
		break;
		
	case WM_DESTROY:
		
		if(TreeView)
		{
			DestroyWindow(TreeView);
			TreeView=NULL;
			
		}
		if(Vlist)
		{
			delete Vlist;
			Vlist=NULL;
			
		}
		if(ObjectList)
		{
			delete ObjectList;
			ObjectList=NULL;
			
		} InsectaFilename[0]=NULL;
		if(winfont)
		{
			DeleteObject(winfont);
			
		} DeleteObject( trace_pen );
		DeleteObject( trace_brush );
		DeleteObject( undef_pen );
		DeleteObject( break_pen );
		DeleteObject( break_brush );
		DeleteObject( current_pen );
		DeleteObject( current_brush );
		DeleteObject( error_pen );
		DeleteObject( interp_pen );
		DeleteObject( diagram_pen );
		DeleteObject( thick_diagram_pen );
		DeleteObject( background_brush );
		DeleteObject( background_pen );
		DeleteObject( select_pen );
		DeleteObject( select_brush );
		DeleteObject( comments_pen );
		hICodeDlg=NULL;
		I_code_hwnd=NULL;
		if(TbarID)
		{
			DeleteObject(TbarID);
			
		}
		if(VbarID)
		{
			DeleteObject(VbarID);
			
		}
		if(DbarID)
		{
			DeleteObject(DbarID);
			
		} TbarID=NULL;
		VbarID=NULL;
		DbarID=NULL;
		SaveObject.get_time(tmp_buff);
		strcat(RegObject.RegSession," ~ ");
		strcat(RegObject.RegSession,tmp_buff);
		RegObject.LogSessionTime(RegObject.RegSession);
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
		
		}
	return 0;
}
//  Export  Date: 12:00:29 AM - 20:Jun:2023;

