#ifndef __USER_EVENT_H__
#define __USER_EVENT_H__

extern EmberEventControl customEventControl; 
extern EmberEventControl customEventControl1; 
extern EmberEventControl customEventControl2; 
extern EmberEventControl customEventControl3;
void customEventFunction(void);
void customEventFunction1(void);
void customEventFunction2(void);
void customEventFunction3(void); 
void LossPacketNotice(void * pcontent);
#endif 

