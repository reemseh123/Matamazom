//
// Created by manar on 25/11/2019.
//
#include "product.h"
#include <stdlib.h>
#include "order.h"
#include <string.h>
#include "product.h"
#include "amount_set.h"
#include <stdio.h>
#include <malloc.h>

struct order_t{
    unsigned int id;
    AmountSet orderedProducts;
};


Order orderCreate(unsigned int id){
    Order order = malloc(sizeof(*order));
    if(!order){
        return NULL;
    }
    order->orderedProducts=asCreate((CopyASElement)productCopy,(FreeASElement)productDestroy,
                                    (CompareASElements)productCompare);
    if(order->orderedProducts==NULL){
        orderDestroy(order);
        return NULL;
    }
    order->id=id;
    return order;
}


void orderDestroy(Order order){
    if(order==NULL){
        return;
    }
    asDestroy(order->orderedProducts);
    free(order);

}


Order orderCopy(Order order){
    if(order==NULL){
        return NULL;
    }
    Order newOrder=orderCreate(order->id);
    if(newOrder==NULL){
        return NULL;
    }
    if(order->orderedProducts==NULL){
        return newOrder;
    }
    newOrder->orderedProducts= asCopy(order->orderedProducts);
    if(!(newOrder->orderedProducts)){
        orderDestroy(newOrder);
        return NULL;
    }
    return newOrder;
}





int orderCompare(Order or1, Order or2){
    if(!or1 || !or2){
        return -1;
    }
    return(or1->id - or2->id);
}
