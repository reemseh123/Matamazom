//
// Created by manar on 25/11/2019.
//

//#include <crtdbg.h>
#include "product.h"
#include <string.h>
#include <malloc.h>
#include "matamazom.h"

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



Product productCreate(unsigned int id,const char* name,MtmProductData data,MatamazomAmountType type
        ,MtmCopyData copyDat,MtmFreeData freeData,MtmGetProductPrice productPrice){
    if(!data || !copyDat || !freeData || !productPrice){
        return NULL;
    }
    Product newProduct= malloc(sizeof(*newProduct));
    if(!newProduct){
        return NULL;
    }
    newProduct->productName = malloc((sizeof(char))*(strlen(name)+1));
    if(newProduct->productName == NULL){
        productDestroy(newProduct);
        return NULL;
    }
    strcpy((newProduct->productName), name);
    newProduct->id=id;
    newProduct->data=copyDat(data);
    newProduct->type=type;
    newProduct->productIncome=0;
    newProduct->copyData=copyDat;
    newProduct->freeData=freeData;
    newProduct->productPrice=productPrice;
    return newProduct;
}


Product productCopy(Product p){
    if(!p)
        return NULL;
    Product copy = productCreate(p->id,p->productName,p->data,p->type,p->copyData,p->freeData,p->productPrice);
    if(!copy)
        return NULL;
    return copy;
}

void productDestroy(Product product){
    if(product==NULL){
        return;
    }
    product->freeData(product->data);
    free(product->productName);
    free(product);
}


//Product productCopy(Product product){
//    if(product==NULL){
//        return NULL;
//    }
//    Product newProduct=malloc(sizeof(*newProduct));
//    if(newProduct==NULL){
//        return NULL;
//    }
//    newProduct->productName = malloc((sizeof(char))*(strlen(((Product)product)->productName)+1));
//    if(newProduct->productName == NULL){
//        productDestroy(newProduct);
//        return NULL;
//    }
//    strcpy((newProduct->productName), (((Product)product)->productName));
//    newProduct->id=product->id;
//    newProduct->data=product->copyData(product->data);
//    newProduct->type=product->type;
//    newProduct->productIncome=product->productIncome;
//    newProduct->copyData=product->copyData;
//    newProduct->freeData=product->freeData;
//    newProduct->productPrice=product->productPrice;
//    return newProduct;
//}

int productCompare(Product product1,Product product2){
    if(!product1 || !product2)
        return PRODUCT_NULL_ARGUMENT;
    return product1->id - product2->id;
}

//double productPrice(Product p, double amount){
//    return p->productPrice(p->data,amount);
//}

//double productUnitPrice(Product p,double amount){
//    double unit = productPrice(p,amount)/amount;
//    unit = curve(unit);
//    return unit;
//}






