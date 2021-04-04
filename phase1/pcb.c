#include <pcb.h>

/********************************************
*
* Implementation of Queue Manager functions
*
*********************************************/
HIDDEN pcb_t pcbFree_table[MAXPROC];
HIDDEN pcb_t *pcbFree_h;

HIDDEN pcb_t* resetPcb(pcb_t* p){
    // Clean PCB 
    p->p_next = NULL;
    p->p_prev = NULL;
    p->p_prnt = NULL;
    p->p_child = NULL;
    p->p_next_sib = NULL;
    p->p_prev_sib = NULL;
    p->p_semAdd = NULL;
    p->p_time = 0;
    
    // Clean p_s
    p->p_s.cause = 0;
    p->p_s.entry_hi = 0;
    for (int i = 0; i < STATE_GPR_LEN; ++i) {
        p->p_s.gpr[i] = 0;
    }
    p->p_s.hi = 0;
    p->p_s.lo = 0;
    p->p_s.pc_epc = 0;
    p->p_s.status = 0;

    // Clean p_supportStruct
    p->p_supportStruct = NULL;
    return p;
}

void freePcb(pcb_t* p){
    p->p_next=pcbFree_h;
    pcbFree_h=p;
}

pcb_t* allocPcb(){
    if(pcbFree_h == NULL) return NULL;
    //Takes and returns a pcb_t from the pcbFree
    pcb_t* toAlloc = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    return resetPcb(toAlloc);
}

void initPcbs(){
    //pcbFree points to the first element of the table
    pcbFree_h = &pcbFree_table[0];
    //Use tmp to cycle to don't lose the head
    pcb_t* tmp = pcbFree_h;
    //Adds all the other elements
    for(int i = 1; i < MAXPROC-1; i++){

        tmp -> p_next = &pcbFree_table[i];
        tmp = tmp-> p_next;
    }
    //Ending with NULL
    tmp -> p_next = NULL;
}


pcb_t *mkEmptyProcQ(){
    return NULL;
}

int emptyProcQ(pcb_t *tp){
    return tp == NULL;
}

void insertProcQ(pcb_t **tp, pcb_t *p){
    if(p==NULL) return;
    //If tp == NULL, the list does not exist therefore there will be only p.
    if(*tp == NULL){
        *tp = p;
        p->p_prev = p;
        p->p_next = p;
        return;
    }

    p->p_prev=*tp;                  //The prev of p becomes tp.
    p->p_next = (*tp)->p_next;      //The next of p will be the one which was the next of tp.
    p->p_prev->p_next = p;          //p->p_prev is tp. The next of tp becomes p.
    p->p_next->p_prev = p;          //The next of p is the first. The prev of the first is p.
    *tp = p;                        //Moves tp in p.
}

pcb_t* removeProcQ(pcb_t **tp){
    if(*tp==NULL) return NULL; //If the queue is empty returns NULL.
    //Otherwise deletes the first element and returns the pointer at him.
    else{
        //Takes the first element of the queue.
        pcb_t *head = (*tp)->p_next;
        //If the list has a single element.
        if(head->p_next==head){
            *tp=NULL;
        }else{
            //Makes the second element of the list the new first.
            (*tp)->p_next = head->p_next;
            head->p_next->p_prev = (*tp);

        }
        //Makes prev and next NULL and returns the deleted element.
        head->p_next=NULL;
        head->p_prev=NULL;
        return head;
    }
}

pcb_t *outProcQ(pcb_t **tp, pcb_t *p){
    if(*tp == NULL || p == NULL) return NULL;
    //If p is the tail element of the queue.
    if(*tp == p){
        //If the queue has a single element.
        if((*tp)->p_next == *tp){
            *tp = NULL;
            return p;
        }
        //If the element is the one at the end of the queue.
        (*tp)->p_prev->p_next = (*tp)->p_next;
        (*tp)->p_next->p_prev = (*tp)->p_prev;
        (*tp)=(*tp)->p_prev;
        return p;
    }
    //If the element is inside the list.
    else{
        //Takes the head and slide until i reach the end of the list.
        pcb_t* head = (*tp)->p_next;
        while(head != (*tp)){
            //If it is found.
            if(head == p){
                head->p_prev->p_next = head->p_next;
                head->p_next->p_prev = head->p_prev;
                return p;
            }
            head = head->p_next;
        }
        return NULL;
    }
}

pcb_t *headProcQ(pcb_t *tp){
    if(tp == NULL) return NULL;
    //Returns the head of the queue
    return tp->p_next;
}

/********************************************
*
* Implementation of Process Tree functions
*
*********************************************/

HIDDEN pcb_t* trim(pcb_t *p){
    p->p_child = NULL;
    p->p_next_sib=NULL;
    p->p_prev_sib=NULL;
    p->p_prnt=NULL;
    return p;
}

int emptyChild(pcb_t *p){
    return p->p_child == NULL;
}

void insertChild(pcb_t *prnt, pcb_t *p){
    //Inserts p as a child of prnt if is not NULL.
    if(p!=NULL){
        //If prnt has no child just inserts.
        if(prnt->p_child==NULL){
            prnt->p_child=p;
            p->p_prnt=prnt;
        }
        //Otherwise handles also the sibilings
        else{
            p->p_next_sib = prnt->p_child;
            prnt->p_child->p_prev_sib=p;
            prnt->p_child = p;
            p->p_prnt=prnt;
        }
    }
}
/*
pcb_t* removeChild(pcb_t *p){
    if(p==NULL || p->p_child==NULL) return NULL;

    //Identify the first child of p.
    pcb_t* son=p->p_child;
    if(son->p_next_sib==NULL){  //It's a only child.
        p->p_child=NULL;
    }else{                      //Has at least one brother.
        p->p_child=son->p_next_sib;
        p->p_child->p_prev_sib=NULL;
    }
    return trim(son);
}

pcb_t *outChild(pcb_t *p){
    if (p->p_prnt==NULL)
        return NULL;

    //If p has no left brother than it's the first of his parent, uses removechild.
    if (p->p_prev_sib == NULL)
        return removeChild(p->p_prnt);

    //Delete p from the brothers' list.
    p->p_prev_sib->p_next_sib=p->p_next_sib;
    //If p is not the last brother of the list,
    //the left brother of p becomes the left brother of the right brother of p.
    if(p->p_next_sib != NULL )  p->p_next_sib->p_prev_sib=p->p_prev_sib;

    return trim(p);
}


*/
pcb_t* removeChild(pcb_t *p) {
    // Perform a head-remove to the
    // children list

    if (p->p_child == NULL) {
        return NULL;
    }
    else {
        pcb_t *child = p->p_child;
        p->p_child = child->p_next_sib;

        if (child->p_next_sib != NULL){
            child->p_next_sib->p_prev_sib = NULL;
        }

        child->p_prnt = NULL;
        child->p_next_sib = NULL;

        return child;
    }
}

pcb_t* outChild(pcb_t *p) {
    if (p->p_prnt == NULL) {
        // p has no parent
        return NULL;
    }

    if (p->p_prev_sib != NULL) {
        p->p_prev_sib->p_next_sib = p->p_next_sib;
    }
    
    if (p->p_next_sib != NULL) {
        p->p_next_sib->p_prev_sib = p->p_prev_sib;
    }

    // If p was the first child, p->p_next_sib
    // becomes the new first child
    if (p->p_prnt->p_child == p) {
        p->p_prnt->p_child = p->p_next_sib;
    }

    p->p_prev_sib = NULL;
    p->p_next_sib = NULL;
    p->p_prnt = NULL;

    return p;
}
