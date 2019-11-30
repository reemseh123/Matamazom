//
// Created by manar on 20/11/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "amount_set.h"

typedef struct AmountSetElement_t{
    ASElement element;
    double amount;
    struct AmountSetElement_t* next;
}ASElements;


struct AmountSet_t{
    CopyASElement copyElement;
    FreeASElement freeElement;
    CompareASElements compareElements;
    ASElements* head;
    ASElements* iterator;
};

AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){

    if ((!copyElement)||(!freeElement)||(!compareElements)){
        return NULL;
    }

    AmountSet newSet=malloc(sizeof(*newSet));
    if (newSet==NULL){
        return NULL;
    }
    newSet->copyElement=copyElement;
    newSet->freeElement=freeElement;
    newSet->compareElements=compareElements;
    newSet->head=NULL;
    newSet->iterator=newSet->head;
    return newSet;

}

void asDestroy(AmountSet set){
    if(set==NULL){
        return;
    }
    asClear(set);
    free(set);
}


AmountSet asCopy(AmountSet set){
    if(!set){
        return  NULL;
    }
    AmountSet newSet=asCreate(set->copyElement,set->freeElement,set->compareElements);
    if (newSet==NULL){
        return  NULL;
    }
    AS_FOREACH(ASElement, iterator, set){
        asRegister(newSet,iterator);
//        assert(result==AS_SUCCESS);
        double x=0;
        double* outcome=&x;
        asGetAmount(set,iterator,outcome);
        asRegister(newSet,iterator);
        asChangeAmount(newSet,iterator,*outcome);

    }
    return newSet;
}



ASElement asGetFirst(AmountSet set){
    if(set==NULL){
        return NULL;
    }
    if(set->head ==NULL){
        return NULL;
    }
    set->iterator=set->head;
    return set->iterator->element;
}


ASElement asGetNext(AmountSet set){
    if(set==NULL){
        return  NULL;
    }
    if (set->iterator==NULL){//the iterator is at an invalid state
        return  NULL;
    }
    if (set->iterator->next==NULL){//iterator reached the end
        set->iterator=set->iterator->next;
        return  NULL;
    }
    set->iterator=set->iterator->next;
    return set->iterator->element;
}


int asGetSize(AmountSet set) {
    if (set == NULL) {
        return -1;
    }
    if (set->head == NULL) {
        return 0;
    }
    int size = 0;
    ASElements *iter = set->head;
    while (iter != NULL) {
        size++;
        iter = iter->next;
    }
    return size;
}


bool asContains(AmountSet set, ASElement element){
    if ((set==NULL)||(element==NULL)){
        return false;
    }
    if (set->head==NULL){
        return false;
    }
    ASElements* iter=set->iterator;
    ASElement current_element=asGetFirst(set);
    while(current_element!=NULL){
        if(set->compareElements(element,current_element)==0){
            set->iterator=iter;
            return true;
        }
        current_element=asGetNext(set);
    }
    set->iterator=iter;
    return false;
}


AmountSetResult asGetAmount(AmountSet set, ASElement element, double *outAmount){
    if((set==NULL)||(element==NULL)||(outAmount==NULL)){
        return AS_NULL_ARGUMENT;
    }
    ASElements* iter=set->iterator;
//    ASElement current_element=asGetFirst(set);
    asGetFirst(set);
    assert(set->iterator=set->head);
    while (set->iterator!=NULL){
        if(set->compareElements(set->iterator->element,element)==0){
            double amount=set->iterator->amount;
            *outAmount=amount;
            set->iterator=iter;
            return AS_SUCCESS;
        }
        set->iterator=set->iterator->next;
    }
    set->iterator=iter;
    assert(asContains(set,element)==false);
    return AS_ITEM_DOES_NOT_EXIST;
}


AmountSetResult asRegister(AmountSet set, ASElement element){
    if(set==NULL || element==NULL){
        return AS_NULL_ARGUMENT;
    }
    if(asContains(set,element)==true){
        return  AS_ITEM_ALREADY_EXISTS;
    }
    ASElements* new_set_element=malloc(sizeof(*new_set_element));
    if (new_set_element==NULL){
        return AS_OUT_OF_MEMORY;
    }
    new_set_element->amount=0;
    new_set_element->element=set->copyElement(element);
    new_set_element->next=NULL;
    if (set->head==NULL){//adding the element at the beginning of the set(the set is empty)
        set->head=new_set_element;
        set->iterator=set->head;
        return AS_SUCCESS;
    }
    ASElement currentElement = asGetFirst(set);
    if (set->compareElements(element,currentElement) < 0){//adding the new_element at the beginning of the set
        new_set_element->next = set->iterator;
        set->head = new_set_element;
        return AS_SUCCESS;
    }
    while(set->iterator->next != NULL) {
        if (set->compareElements(element,set->iterator->next->element) < 0) {
            new_set_element->next = set->iterator->next;
            set->iterator->next = new_set_element;
            return AS_SUCCESS;
        }
        currentElement = asGetNext(set);
    }
    set->iterator->next = new_set_element;
    set->iterator = new_set_element->next;
    return AS_SUCCESS;
}


AmountSetResult asChangeAmount(AmountSet set, ASElement element, const double amount){
    if( !set || ! element){
        return  AS_NULL_ARGUMENT;
    }
    if(asContains(set,element)==false){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    ASElements* iter=set->iterator;
//    ASElement current_element=asGetFirst(set);
    set->iterator=set->head;
    while (set->iterator!=NULL){
        if(set->compareElements(set->iterator->element,element)==0){
            double new_total_amount=(set->iterator->amount)+amount;
            if(new_total_amount<0){
                return AS_INSUFFICIENT_AMOUNT;
            }
            set->iterator->amount=new_total_amount;
            set->iterator=iter;
            return AS_SUCCESS;
        }
        set->iterator=set->iterator->next;
    }
    return AS_SUCCESS;
}


AmountSetResult asDelete(AmountSet set, ASElement element){
    if(!set || !element){
        return  AS_NULL_ARGUMENT;
    }
    if (!asContains(set,element)){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    if(set->compareElements(set->head->element,element)==0){//deleting the head
        if(set->head->next==NULL){//there is just one ASElement in the set
            set->freeElement(set->head->element);
            free(set->head);
            set->head=NULL;
            set->iterator=set->head;
            return AS_SUCCESS;
        }
        ASElements* ptr=set->head;
        set->head=set->head->next;
        set->iterator=set->head;
        ptr->next=NULL;
        set->freeElement(ptr->element);
        free(ptr);
        return AS_SUCCESS;
    }
//    ASElement current_element=asGetFirst(set);
    set->iterator=set->head;
    while (set->iterator->next != NULL) {
        ASElements* ptr = set->iterator->next;
        if (set->compareElements(ptr->element, element) == 0){
            set->iterator->next = ptr->next;
            set->freeElement(ptr->element);
            ptr->next = NULL;
            free(ptr);
            return AS_SUCCESS;
        }
        set->iterator=set->iterator->next;
    }
    return AS_NULL_ARGUMENT;
}


AmountSetResult asClear(AmountSet set){
    if(set==NULL){
        return AS_NULL_ARGUMENT;
    }
    while(set->head!=NULL){
        asDelete(set,set->head->element);
    }
    return AS_SUCCESS;
}
