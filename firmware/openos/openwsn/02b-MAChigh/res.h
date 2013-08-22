#ifndef __RES_H
#define __RES_H

/**
\addtogroup MAChigh
\{
\addtogroup RES
\{
*/

#include "opentimers.h"
#include "IEEE802154E.h"
#include "schedule.h"

//=========================== define ==========================================

#define MULTISUPERFRAME_LENGTH  11 // distance, in number of superframe, beetween ADV sending
#define MSSLOTDURATION          15 // given by TsSlotDuration*US_PER_TICKS/1000
#define AVGNUMSLOTSYNCH         16 // should be setted to 16 in accord to probability frequency match
#define SYNCHPHASEDURATION AVGNUMSLOTSYNCH*MSSLOTDURATION*SUPERFRAME_LENGTH

//=========================== typedef =========================================

//=========================== module variables ================================

typedef struct {
   bool            MacMgtTask;           // switch beetween MAC synch and normal operation phases
   uint8_t         MacMgtTaskCounter;    // counter to determine what management task to do
   uint16_t        periodMaintenance;
   bool            busySendingKa;        // TRUE when busy sending a keep-alive
   bool            busySendingAdv;       // TRUE when busy sending an advertisement
   uint8_t         dsn;                  // current data sequence number
   opentimer_id_t  timerId;
   opentimer_id_t  taskTimerId;
} res_vars_t;

//=========================== prototypes ======================================

void      res_init();
bool      debugPrint_myDAGrank();
void      res_scheduleSwitchTask();
void      res_changeMacMgtTask(bool newTask);
void      res_checkMacMgtTask();
void      res_retrieveMacMgtTaskCounter(uint8_t* counter);
void      res_incrementMacMgtTaskCounter();
// from upper layer
owerror_t res_send(OpenQueueEntry_t *msg);
// from lower layer
void      task_resNotifSendDone();
void      task_resNotifReceive();

/**
\}
\}
*/

#endif
