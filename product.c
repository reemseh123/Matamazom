//
// Created by manar on 25/11/2019.
//

#include <crtdbg.h>
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

static double curve(double unit);

Product productCreate(unsigned int id,const char* name,MtmProductData data,MatamazomAmountType type
        ,MtmCopyData copyDat,MtmFreeData freeData,MtmGetProductPrice productPrice){
    if(!data || !copyDat || !freeData || !productPrice){
        return NULL;
    }
    Product newProduct=malloc(sizeof(*newProduct));
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
    newProduct->data=data;
    newProduct->type=type;
    newProduct->productIncome=0;
    newProduct->copyData=copyDat;
    newProduct->freeData=freeData;
    newProduct->productPrice=productPrice;
    return newProduct;
}


void productDestroy(Product product){
    if(product==NULL){
        return;
    }
    free(product->productName);
    free(product);
}


Product productCopy(Product product){
    if(product==NULL){
        return NULL;
    }
    Product newProduct=malloc(sizeof(*newProduct));
    if(newProduct==NULL){
        return NULL;
    }
    newProduct->productName = malloc((sizeof(char))*(strlen(((Product)product)->productName)+1));
    if(newProduct->productName == NULL){
        productDestroy(newProduct);
        return NULL;
    }
    strcpy((newProduct->productName), (((Product)product)->productName));
    newProduct->id=product->id;
    newProduct->data=product->data;
    newProduct->type=product->type;
    newProduct->productIncome=product->productIncome;
    newProduct->copyData=product->copyData;
    newProduct->freeData=product->freeData;
    newProduct->productPrice=product->productPrice;
    return newProduct;
}

int productCompare(Product product1,Product product2){
    if(!product1 || !product2)
        return PRODUCT_NULL_ARGUMENT;
    return product1->id - product2->id;
}

double productPrice(Product p, double amount){
    return p->productPrice(p->data,amount);
}

double productUnitPrice(Product p,double amount){
    double unit = productPrice(p,amount)/amount;
    unit = curve(unit);
    return unit;
}

static double curve(double unit){
    int test = unit*1000;
    test %=1000;
    if(test == 0){
        return unit;
    }
    if((test/10)%10>=5) {
        test = 100*(1+test/100);
        return ((int) unit + (double)test / 1000);
    }
    if(test%10>=5){
        if(test%10==9){
            test = 100*(1+test/100);
            return ((int) unit + (double)test / 1000);
        }
        test = 100*(test/100)+(1+(test/10)%10)*10;
        return ((int) unit + (double)test / 1000);
    }
    unit = (int)unit + (double)(test/100)/10;
    return unit;
}







