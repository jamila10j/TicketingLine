#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define NAMELEN 12
#define EMPTY -1

typedef struct customer
{
    char* name;
    int numTickets;
    int lineNumber;
    int timeInLine;
}customer;

typedef struct node 
{
    struct customer* data;
    struct node* next;
}node;

typedef struct queue
{
    struct node* front;
    struct node* back;
}queue;

void Enqueue(queue* qPtr, customer* customer);
void Dequeue(queue* qPtr, FILE* output, customer* customer);
int currentTime = 0; 

customer* initCustomers(FILE* customerData, int numOfCustomers, queue* qPtr)
{
    char tempName[NAMELEN];
    customer* customerArray = (customer*) malloc(numOfCustomers * sizeof(customer));

    for (int i = 0; i < numOfCustomers; i++)
    {
        fscanf(customerData, "%d", &customerArray[i].timeInLine);
        fscanf(customerData, "%d", &customerArray[i].lineNumber);

        customerArray[i].name = (char *) malloc(NAMELEN * sizeof(char));
        fscanf(customerData, "%s", tempName);
        strcpy(customerArray[i].name, tempName);

        fscanf(customerData, "%d", &customerArray[i].numTickets);
      
        Enqueue(qPtr, &customerArray[i]);
    }

    return customerArray;
}

queue* initQueues()
{
    queue* queuePtr = (queue*) malloc(12 * sizeof(queue));

    for (int i = 0; i < 12; i++)
    {
      queuePtr[i].front = NULL;
      queuePtr[i].back = NULL;
    }
    
    return queuePtr;
}

void Enqueue(queue* qPtr, customer* customer)
{
    node* temp = (node*) malloc(sizeof(node));

    temp -> data = customer;
    temp -> next = NULL;

    if (qPtr[customer -> lineNumber - 1].back != NULL)
    {
        qPtr[customer -> lineNumber - 1].back -> next = temp;
    }

    qPtr[customer -> lineNumber - 1].back = temp;

    if (qPtr[customer -> lineNumber - 1].front == NULL)
    {
        qPtr[customer -> lineNumber - 1].front = temp;
    }
}

void Dequeue(queue* qPtr, FILE* output, customer* customer)
{
    node* temp;

    if (qPtr[customer -> lineNumber - 1].front == NULL)
    {
        return;
    }

    temp = qPtr[customer -> lineNumber - 1].front;

    printf("%s from line %d checks out at time %d.\n", customer -> name, customer -> lineNumber, currentTime);
    fprintf(output, "%s from line %d checks out at time %d.\n", customer -> name, customer -> lineNumber, currentTime);

    qPtr[customer -> lineNumber - 1].front = qPtr[customer -> lineNumber - 1].front -> next;

    if (qPtr[customer -> lineNumber - 1].front == NULL)
    {
        qPtr[customer -> lineNumber - 1].back = NULL;
    }

    free(temp -> data -> name);
    free(temp);
}

node* Peek(queue* qPtr, int queueNum)
{
  return qPtr[queueNum].front;
}

int main(void)
{
    atexit(report_mem_leak);

    FILE* inFile = fopen("in.txt", "r");
    FILE* outFile = fopen("out.txt", "w");
    int numTestCases;
    int numCustomers;
    customer* customers = NULL;

    queue* qPtr = NULL;

    fscanf(inFile, "%d\n", &numTestCases);
    for (int i = 0; i < numTestCases; i++)
    {
        fscanf(inFile, "%d\n", &numCustomers);
        qPtr = initQueues();
        customers = initCustomers(inFile, numCustomers, qPtr);

        node* firstInLine = NULL;
        for (int k = 0; k < 12; k++)
        {
          if (firstInLine == NULL)
          {
            firstInLine = Peek(qPtr, k);
          }
          else if (Peek(qPtr, k) != NULL && firstInLine -> data -> timeInLine > Peek(qPtr, k) -> data -> timeInLine)
          {
             firstInLine = Peek(qPtr, k);
          }
        }
        currentTime += firstInLine -> data -> timeInLine + 30 + (5 * firstInLine -> data -> numTickets);
        Dequeue(qPtr, outFile, firstInLine -> data);  

        for (int j = 0; j < numCustomers - 1; j++)
        {
          node* nextInLine = NULL;
          for (int k = 0; k < 12; k++)
          {
            if (nextInLine == NULL)
            {
              nextInLine = Peek(qPtr, k);
            }
            else if (Peek(qPtr, k) != NULL && nextInLine -> data -> numTickets > Peek(qPtr, k) -> data -> numTickets)
            {
               nextInLine = Peek(qPtr, k);
            }
          }
          if (nextInLine -> data -> numTickets != 53)
          {
            currentTime += 30 + (5 * nextInLine -> data -> numTickets);
          }
          else
          {
            currentTime = nextInLine -> data -> timeInLine + 30 + (5 * nextInLine -> data -> numTickets);
          }
          
          Dequeue(qPtr, outFile, nextInLine -> data);
        }
      
        free(customers);
        free(qPtr);
        currentTime = 0;
    }
    
    
    fclose(inFile);
    fclose(outFile);
    return 0;
}