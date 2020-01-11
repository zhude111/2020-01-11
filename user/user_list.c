#include "user_include.h"

//#define  RESEND_SIZE_MAX  80

LinkedList* uartRxList=NULL;
LinkedList* uartSendList=NULL;
LinkedList* deviceJoinList=NULL;
LinkedList* radioList=NULL;

int createList(void)
{
    uartSendList=ListInit();
    if(uartSendList==NULL)
    {
      return 1;
    }
    
    deviceJoinList=ListInit();
     if(deviceJoinList==NULL)
    {
      return 1;
    }
    
    uartRxList=ListInit();
     if(uartRxList==NULL)
    {
      return 1;
    }
    
     radioList=ListInit();
     if(radioList==NULL)
    {
      return 1;
    }
    
    return 0;
}
/*************************************************************************
//初始化链表
*************************************************************************/
LinkedList* ListInit(void)
{
  LinkedList* list =(LinkedList*)osal_mem_alloc(sizeof(LinkedList));
  if (list != NULL) {
    memset(list, 0, sizeof(LinkedList));
  }
  return list;
}
/*************************************************************************
//把数据放入链表尾部
*************************************************************************/
ListElement* ListPushBack(LinkedList* list,void* content,int repeat_cnt,int timeout)
{
  ListElement* element =(ListElement*)osal_mem_alloc(sizeof(ListElement));
  if (element != NULL) 
  	{
         element->content = content;
         element->repeat_cnt=repeat_cnt;
         element->timeout=timeout;
         element->next = NULL;
         element->previous = list->tail;
         if (list->head == NULL) 
          {
           list->head = element;
          } 
          else 
          {
           list->tail->next = element;
          }
        list->tail = element;
        ++(list->count);
     }
  return element;
}
/*************************************************************************
//从链表中移除一个指定的元素
*************************************************************************/
char ListRemoveElement(LinkedList* list,ListElement* element)
{
  if ((element != NULL) && (list->head != NULL)) {
    if (element == list->head) {
      if (list->head == list->tail) {//只有一个元素
        list->head = NULL;
        list->tail = NULL;
      } else {
        list->head = element->next;
        element->next->previous = NULL;
      }
    } else if (element == list->tail) {
      list->tail = element->previous;
      element->previous->next = NULL;
    } else {
      element->previous->next = element->next;
      element->next->previous = element->previous;
    }
    --(list->count);
    return 0;
  }
  return 1;
}

/*************************************************************************
//====得到链表中特定位置的节点的下一个=============
*************************************************************************/
ListElement* ListNextElement(LinkedList* list,ListElement* elementPosition)
{
  if (elementPosition == NULL) {
    return list->head;
  } else {
    return elementPosition->next;
  }
}

/***********************************************************
从开始部位拿出一个元素，链表就小了一个元素
*************************************************************/
ListElement* ListPopFront(LinkedList* list)
{
  ListElement* head=NULL;
  
  if (list->count > 0) 
    {
        head = list->head;
        if (list->tail == head) 
        {
            list->tail = NULL;
        }
        list->head = list->head->next;
        --(list->count);
  }
  return head;
}
/***********************************************************
从链表中拿出一个条件匹配的元素,并把他从链表中移除
*************************************************************/
ListElement* FindListElementAndDelete(LinkedList* list,void* content,int len,int startIndex)
{
  ListElement* currentElement = NULL;
  uint8 * pbuf=NULL;
    if(content==NULL)
    {
        return currentElement;
    }
    do{
          currentElement= ListNextElement(list,currentElement);
                if (currentElement != NULL)
                {  
                        pbuf=currentElement->content;   //跳过包头和包长
                        if(memcmp(content,&pbuf[startIndex],len)==0)
                            {
                               ListRemoveElement(list,currentElement);
                               break;
                            }
                       else
                            {
                              halResetWatchdog();   // Periodically reset the watchdog.
                            }
                }
    } while (currentElement != NULL); 
    
    return currentElement;
}

/***********************************************************
从链表中拿出一个条件匹配的元素
*************************************************************/
ListElement* FindListElement(LinkedList* list,void* content,int len,int startIndex)
{
  ListElement* currentElement = NULL;
  uint8 * pbuf=NULL;
    if(content==NULL)
    {
        return content;
    }
    do{
          currentElement= ListNextElement(list,currentElement);
                if (currentElement != NULL)
                {         pbuf=currentElement->content; 
                        if(memcmp(content,&pbuf[startIndex],len)==0)
                            {
                               return currentElement;
                            }
                       else
                            {
                              halResetWatchdog();   // Periodically reset the watchdog.
                            }
                }
    } while (currentElement != NULL);

     currentElement=NULL;
     return currentElement;  
}

//周期查询重发链表，重发命令
void Period_Process_List(LinkedList* list,timer_msg_proc fun,timer_msg_proc leave_fun)
{
    ListElement* currentElement = NULL;
    ListElement* nextElement= NULL;
    
    currentElement= list->head;
    
    while (currentElement != NULL)
    {
        nextElement=currentElement->next;
            if(currentElement->repeat_cnt>0)
		{
                      if((currentElement->timeout--)>0)
                       {
                             if(currentElement->timeout ==0)
                                {
                                  currentElement->timeout = Resend_TIMEOUT;
                                  currentElement->repeat_cnt--;
                                  if(currentElement->repeat_cnt!=0)
                                  {
                                   fun(currentElement->content);
                                  }
                                }
                                if(currentElement->repeat_cnt ==0)
                                {
                                 ListRemoveElement(list,currentElement);
                                 if(leave_fun!=NULL)
                                 {
                                  leave_fun(currentElement->content); 
                                 }
                                 osal_mem_free(currentElement->content); 
                                 osal_mem_free(currentElement);
                                 
                                }
                      } 
	       }
       halResetWatchdog(); 
       currentElement=nextElement;
  };  
}





  

