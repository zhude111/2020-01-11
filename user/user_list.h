#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

typedef struct _ListElement {
  struct _ListElement* next;
  struct _ListElement* previous;
  int    repeat_cnt;
  int    timeout;
  void*  content;
} ListElement;

typedef struct {
  ListElement* head;
  ListElement* tail;
  int  count;
} LinkedList;

extern LinkedList* uartRxList; 
extern LinkedList* uartSendList;
extern LinkedList* deviceJoinList;
typedef void timer_msg_proc(void * content);

int createList(void);
LinkedList* ListInit(void);
char ListRemoveElement(LinkedList* list,ListElement* element);
ListElement* ListNextElement(LinkedList* list,ListElement* elementPosition);
ListElement* ListPopFront(LinkedList* list);
ListElement* FindListElementAndDelete(LinkedList* list,void* content,int len,int startIndex);
ListElement* FindListElement(LinkedList* list,void* content,int len,int startIndex);
void Period_Process_List(LinkedList* list,timer_msg_proc fun,timer_msg_proc leave_fun);
ListElement* ListPushBack(LinkedList* list,void* content,int repeat_cnt,int timeout);
#endif 

