/* LEIDOS PROPRIETARY INFORMATION
 * The information in this document is proprietary to LEIDOS. It may not be used,
 * reproduced, disclosed, or exported without the written approval of LEIDOS
 */
 
/* TqmpMaster_main.cpp
 *
 * Entry Point for Master Processor
 *
*/
/*******************************************************************************
*                                 Includes                                     *
*******************************************************************************/
#include "Alg_integrate.h"
#include "SysAPI.hpp"
#include "IntraVasComms.hpp"
#include "ErrHandler.hpp"
#include <INTEGRITY.h>
#include <stdlib.h>
#include <iostream>

#ifdef FSW_GTEST_GHS_BUILD // Conditionally include and start unit test task
   #include "TaskGTest.hpp"
#endif
/*******************************************************************************
*                              Local Constants                                 *
*******************************************************************************/
const uint16_t MAX_ATTEMPTS = 1000;  //1000x500usec=0.5 sec
/*******************************************************************************
*                                  Globals                                     *
*******************************************************************************/

/*******************************************************************************
*                                  Statics                                     *
*******************************************************************************/
static SysAPI& oApi = SysAPI::GetInstance();
static IntraVasComms& oComms = IntraVasComms::GetInstance();
#ifdef FSW_GTEST_GHS_BUILD      
   static Task TaskGTest;
#endif
/*******************************************************************************
*                            Private Definitions                               *
*******************************************************************************/

/*******************************************************************************
 *                          Constructors/Destructors                           *
 ******************************************************************************/

/*******************************************************************************
 *                            Public Definitions                               *
 ******************************************************************************/

// entry point for Exec_VAS
int main()
{
   // wait for kernel to be ready before starting Exec_VAS tasks
   WaitForSemaphore(ExecStartupLink);
   printf("Main VAS starting...\n");

   while(1)
   {
      ////////////////////////////////////////////////////
      //           System Initialization               //
      //////////////////////////////////////////////////

      // clear all system errors - ErrHandler initializes to no errors

      // default parameters are set up in constructor: SysAPI::SysAPI(), which runs before main()
      
      // let Task PayloadC2 start-up & run until Pending
      Value taskStatus;
      int16_t ctr=0, errCnt = 0;
      Error intErrVal = RunTask(PayloadC2);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)PayloadC2, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));

      // let Task IrstapAlg start-up & run until Pending
      ctr=0;
      intErrVal = RunTask(IrstapAlg);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)IrstapAlg, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));

      // let Task PntCtrl start-up & run until Pending
      ctr=0;
      intErrVal = RunTask(PntCtrl);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)PntCtrl, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));

      // Mem control task for ALG
      ctr=0;
      intErrVal = RunTask(TaskMemCtrlAlg);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)TaskMemCtrlAlg, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));

#ifndef NO_PCIO
      // let TaskPcioMemComms start-up & run until Pending
      ctr=0;
      intErrVal = RunTask(PcioMemComms);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)PcioMemComms, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));
#endif
     
      //Run the BIT task
      ctr=0;
      intErrVal = RunTask(TaskBitCtrl);
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)TaskBitCtrl, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));

#ifdef FSW_GTEST_GHS_BUILD      
      //Run the Testing task
      // Use a delay to allow ethernet IO to complete before running tests
      // Any less that 5s will cut into the other io msg for sockets
      const uint32_t IOSTARTDELAY = 5000000;
      usleep(IOSTARTDELAY);
      Error errVal = CommonCreateTask(1, (Address)TaskGTest::MainTaskEntrance, 0x40000,
                             (char *)"TaskGTest", &TaskGTest);
      if(errVal){
	 errCnt++;
      }
      errVal = RunTask(TaskGTest);
      ctr=0;
      if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      do {
         Yield();
         usleep(500);
         intErrVal = GetTaskStatus((Task)TaskGTest, &taskStatus, NULL, NULL);
         if(intErrVal != ErrHandler::NOT_AN_ERROR) {errCnt+=errCnt;}
      } while((taskStatus != StatPending) && (ctr++<MAX_ATTEMPTS));
#endif // FSW_GTEST_GHS_BUILD

      WaitForSemaphore(ReStartExecSem); 
   }
}
