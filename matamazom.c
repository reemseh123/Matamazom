//
// Created by manar on 20/11/2019.
//



//
// Created by manar on 25/11/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "amount_set.h"
#include "matamazom.h"
#include "order.h"
#include "product.h"
#include "matamazom_print.h"
#include "set.h"
#define FAILURE 0




struct Matamazom_t{
    AmountSet products;
    Set orders;
    unsigned int orderId;
};

struct product_t{
    char* productName;
    unsigned int id;
    MtmProductData data;
    MatamazomAmountType type;
    double productIncome;
    MtmCopyData copyData;
    MtmFreeData freeData;
    MtmGetProductPrice productPrice;
};

struct order_t{
    unsigned int id;
    AmountSet orderedProducts;
};

static bool isValidName(const char* name){
    if (strlen(name)==0 || !name){//if name is empty
        return  false;
    }
    char firstLet = *name;
    if ((firstLet>='A' && firstLet<='Z')||(firstLet>='0' && firstLet<='9')||(firstLet>='a' &&firstLet<='z')){
        return true;
    }
    return false;
}

static bool validAmountAccordingToType(MatamazomAmountType type,const double amount){
    double copy = amount - 0.5;
    if(amount<0)
        copy += 1;
    switch (type) {
        case MATAMAZOM_INTEGER_AMOUNT:
            return (((amount - (int) amount <= 0.001) && (amount - (int) amount >= 0)) ||
                    (((int) amount - amount >= 0.999) && (amount < 0)) ||
                    ((int) amount + 1 - amount <= 0.001)||
                    ((int)amount - amount<=0.001 && amount<0));
        case MATAMAZOM_HALF_INTEGER_AMOUNT:
            return ((copy - (int) amount <= 0.001 && copy - (int) amount >= 0) ||
                    ((amount - (int) amount <= 0.001) && (amount - (int) amount >= 0)) ||
                    ((int) amount - amount <= 0.001 && amount < 0) ||
                    ((int) amount + 1 - amount <= 0.001) ||
                    ((int) amount - copy <= 0.001 && (int) amount > (int) copy)||
                    ((((int)amount-amount>=0.5 &&(int)amount - amount<=0.501)))||
                    ((int) amount - amount >= 0.999) );
        case MATAMAZOM_ANY_AMOUNT:
            return true;
    }
    return false;
}


static Product mtmGetProduct(AmountSet set, unsigned int productId){
    AS_FOREACH(Product,currentProduct,set){
        if(currentProduct->id==productId){
            return currentProduct;
        }
    }
    return NULL;
}


static Order mtmGetOrder(Set set, unsigned int orderId){
    SET_FOREACH(Order,currentOrder,set){
        if(currentOrder->id==orderId){
            return currentOrder;
        }
    }
    return NULL;
}


static bool validOrderProductsAmounts(Matamazom matamazom,AmountSet orderProducts){
    AS_FOREACH(Product,currentProduct,orderProducts) {
        double productAmountInOrder=0;
        double productAmountInMtm=0;
        asGetAmount(orderProducts, currentProduct,&productAmountInOrder);
        asGetAmount(matamazom->products, currentProduct, &productAmountInMtm);//matamazom contains the currentProduct
        if ((productAmountInMtm) < (productAmountInOrder)) {
            return false;
        }
    }
    return true;
}


static Product bestSelling(AmountSet products,double* outcome){
    if(!products)
        return NULL;
    Product best=NULL;
    double max=0;
    AS_FOREACH(Product,currentProduct,products){
        double productIncome= currentProduct->productIncome;
        if(productIncome>max) {
            best = currentProduct;
            max= productIncome;
        }
    }
    *outcome = max;
    return best;
}


    Matamazom matamazomCreate() {
        Matamazom newMatamzom = malloc(sizeof(*newMatamzom));
        if (newMatamzom == NULL) {
            return NULL;
        }
        newMatamzom->products =
                asCreate((CopyASElement)productCopy, (FreeASElement)productDestroy,
                         (CompareASElements)productCompare);
        if (newMatamzom->products == NULL) {
            matamazomDestroy(newMatamzom);
            return NULL;
        }
        newMatamzom->orders =
                setCreate((copySetElements)orderCopy,(freeSetElements)orderDestroy, (compareSetElements)orderCompare);
        if (newMatamzom->orders == NULL) {
            matamazomDestroy(newMatamzom);
            return NULL;
        }
        newMatamzom->orderId = 1;
        return newMatamzom;
    }


    void matamazomDestroy(Matamazom matamazom) {
        if(!matamazom){
            return;
        }
        setDestroy(matamazom->orders);
//        assert(matamazom->orders==NULL);
        asDestroy(matamazom->products);
        free(matamazom);

    }


MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id, const char *name,
                              const double amount, const MatamazomAmountType amountType,
                              const MtmProductData customData, MtmCopyData copyData,
                              MtmFreeData freeData, MtmGetProductPrice prodPrice) {
    if ((!matamazom) || (!name) || (!customData) || (!copyData) || (!freeData) || (!prodPrice)) {
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if (isValidName(name) == false) {
        return MATAMAZOM_INVALID_NAME;
    }
    if((amount<0)||(validAmountAccordingToType(amountType,amount)==false)) {
        return MATAMAZOM_INVALID_AMOUNT;
    }
    Product new_product = productCreate( id,name,customData,amountType,copyData,freeData,prodPrice);
    if (!new_product){
        matamazomDestroy(matamazom);
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    AmountSetResult result = asRegister(matamazom->products,new_product);
    if (result== AS_ITEM_ALREADY_EXISTS){
        productDestroy(new_product);
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }
    if (result==AS_SUCCESS){
        asChangeAmount(matamazom->products,new_product,amount);
        productDestroy(new_product);
        return MATAMAZOM_SUCCESS;
    }
    productDestroy(new_product);
    matamazomDestroy(matamazom);
     return MATAMAZOM_OUT_OF_MEMORY;
}


MatamazomResult mtmChangeProductAmount(Matamazom matamazom, const unsigned int id, const double amount){
    if(!matamazom){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    AS_FOREACH(Product,iterator,matamazom->products){
        if (iterator->id==id) {
            if (validAmountAccordingToType(iterator->type, amount) == false) {
                return MATAMAZOM_INVALID_AMOUNT;
            }
            AmountSetResult result = asChangeAmount(matamazom->products, iterator, amount);
            if (result == AS_INSUFFICIENT_AMOUNT) {
                return MATAMAZOM_INSUFFICIENT_AMOUNT;
            }
            assert(result == AS_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    }
    return MATAMAZOM_PRODUCT_NOT_EXIST;
}


MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){//we did not erase the product from the orders
    if(!matamazom){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Product productToDelete=mtmGetProduct(matamazom->products,id);
    if(productToDelete==NULL){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    asDelete(matamazom->products,productToDelete);
    SET_FOREACH(Order,currentOrder,matamazom->orders){
       Product product=mtmGetProduct(currentOrder->orderedProducts,id);
       if(product!=NULL){
           asDelete(currentOrder->orderedProducts,product);
       }
    }
    return MATAMAZOM_SUCCESS;
}


unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if(!matamazom){
        return FAILURE;
    }
    Order newOrder=orderCreate(matamazom->orderId);
    if(newOrder==NULL){
        return FAILURE;
    }
    if(setAdd(matamazom->orders,(SetElement)newOrder)!=SET_SUCCESS){
        orderDestroy(newOrder);
        return FAILURE;
    }
    orderDestroy(newOrder);
    matamazom->orderId=matamazom->orderId+1;
    return(matamazom->orderId-1);
}


MatamazomResult mtmChangeProductAmountInOrder(Matamazom mtmazom , const unsigned int orderId,
                                              const unsigned int productId, const double amount){
    if(!mtmazom){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Order order=mtmGetOrder(mtmazom->orders,orderId);
    if(order==NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    Product product=mtmGetProduct(mtmazom->products,productId);
    if(product==NULL){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }
    if(validAmountAccordingToType(product->type,amount)==false){
        return MATAMAZOM_INVALID_AMOUNT;
    }
    if(amount==0){
        return MATAMAZOM_SUCCESS;
    }
    AmountSetResult result = asChangeAmount(order->orderedProducts,product,amount);
    if(result == AS_INSUFFICIENT_AMOUNT){
      asDelete(order->orderedProducts,product);
    }
    if(result == AS_ITEM_DOES_NOT_EXIST && amount>0){
       asRegister(order->orderedProducts,product);
       asChangeAmount(order->orderedProducts,product,amount);
    }
    return MATAMAZOM_SUCCESS;

}


static void change_amount_outcome(Matamazom matamazom,Order order){
    double amount=0;
    AS_FOREACH(Product,currentProduct,order->orderedProducts){
        asGetAmount(order->orderedProducts,(ASElement)currentProduct,&amount);
        mtmChangeProductAmount(matamazom,currentProduct->id,-1*amount);
        double outcome =  currentProduct->productPrice(currentProduct->data,amount);
        Product product=mtmGetProduct(matamazom->products,currentProduct->id);
        product->productIncome=( product->productIncome)+outcome;
        currentProduct->productIncome=currentProduct->productIncome+outcome;
    }
}


MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId){
    if(!matamazom)
        return MATAMAZOM_NULL_ARGUMENT;
    Order orderToShip=mtmGetOrder(matamazom->orders,orderId);
    if(!orderToShip)
        return MATAMAZOM_ORDER_NOT_EXIST;
    if(validOrderProductsAmounts(matamazom,orderToShip->orderedProducts)){
        change_amount_outcome(matamazom,orderToShip);
        setRemove(matamazom->orders,(SetElement)orderToShip);
        return MATAMAZOM_SUCCESS;
    }
    return MATAMAZOM_INSUFFICIENT_AMOUNT;
}


MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId){
    if(!matamazom){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Order orderToCancel=mtmGetOrder(matamazom->orders,orderId);
    if(orderToCancel==NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    setRemove(matamazom->orders,(SetElement)orderToCancel);
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output){
    if(!matamazom || !output)
        return MATAMAZOM_NULL_ARGUMENT;
    double amount = 0;
    fprintf(output,"Inventory Status:\n");
    AS_FOREACH(Product,iterator,matamazom->products){
        asGetAmount(matamazom->products,(ASElement )iterator,&amount);
//        MtmProductData data=iterator->copyData(iterator->data);
        double unit = 1;
        double pricePerUnit=iterator->productPrice(iterator->data,unit);
        mtmPrintProductDetails(iterator->productName,iterator->id,amount,pricePerUnit,output);
    }
    return MATAMAZOM_SUCCESS;
}


MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId, FILE *output){
        if(!matamazom ||!output){
            return MATAMAZOM_NULL_ARGUMENT;
        }
        Order orderToPrint=mtmGetOrder(matamazom->orders,orderId);
        if(orderToPrint==NULL){
            return MATAMAZOM_ORDER_NOT_EXIST;
        }
        double totalPrice=0;
        mtmPrintOrderHeading(orderId,output);
        AS_FOREACH(Product,currentProduct,orderToPrint->orderedProducts){
            double amount=0;
            asGetAmount(orderToPrint->orderedProducts,currentProduct,&amount);
            double productPrice=currentProduct->productPrice(currentProduct->data,amount);
            mtmPrintProductDetails(currentProduct->productName,currentProduct->id,amount,productPrice,output);
            totalPrice=totalPrice+productPrice;
        }
        mtmPrintOrderSummary(totalPrice, output);
        return  MATAMAZOM_SUCCESS;
}


MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output){
    if(!matamazom || !output)
        return MATAMAZOM_NULL_ARGUMENT;
    double outcome=0;
    Product best = bestSelling(matamazom->products,&outcome);
    fprintf(output,"Best Selling Product:\n");
    if(!best){
        fprintf(output,"none\n");
        return MATAMAZOM_SUCCESS;
    }
    mtmPrintIncomeLine(best->productName,best->id,outcome,output);
    return MATAMAZOM_SUCCESS;
    }


MatamazomResult mtmPrintFiltered(Matamazom matamazom, MtmFilterProduct customFilter, FILE *output){
    if(!matamazom || !customFilter || !output)
        return MATAMAZOM_NULL_ARGUMENT;
    double amount = 0;
    AS_FOREACH(Product,currentProduct,matamazom->products){
        asGetAmount(matamazom->products,(ASElement)currentProduct,&amount);
        if(customFilter(currentProduct->id,currentProduct->productName,amount,currentProduct->data)){
            double unit=1;
            double pricePerUnit=currentProduct->productPrice(currentProduct->data,unit);
            mtmPrintProductDetails(currentProduct->productName,currentProduct->id,amount
                    ,pricePerUnit,output);
        }
    }
    return MATAMAZOM_SUCCESS;
}




