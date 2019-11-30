
#ifndef MATAMAZOM_PRODUCT_H
#define MATAMAZOM_PRODUCT_H
#include <stdbool.h>
#include "matamazom.h"
#include "amount_set.h"

/**
* product Container
*
* Implements a product container type.
*
* The following functions are available:
*  productCreate		          - Creates a new empty product
*  productDestroy              - Deletes an existing product and frees all resources
*  productCopy		          - Copies an existing product
*  productGetIncome           -gets the product income
*  bool productCompare
*/

/** Type for defining the product */
typedef struct product_t *Product;

/** Type used for returning error codes from order functions */
typedef enum ProductResult_t {
    PRODUCT_SUCCESS,
    PRODUCT_OUT_OF_MEMORY,
    PRODUCT_NULL_ARGUMENT,
} ProductResult;

/**
* productCreate: Allocates a new product.
*
* @param id - the new product id
* @param productName - the new product name
* @param data        -the new product data
 * @param type       -the new product type
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new product in case of success.
*/
Product productCreate(unsigned int id,const char* name,MtmProductData data,MatamazomAmountType type
                      ,MtmCopyData copyDat,MtmFreeData freeData,MtmGetProductPrice productPrice);


/**
* productDestroy: Deallocates an existing product. Clears all elements by using the
* stored free functions.
*
* @param product - Target product to be deallocated. If order is NULL nothing will be
* 		done
*/
void productDestroy(Product product);

/**
* productCopy: Creates a copy of target product.
* @param product - Target product.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A product containing the same elements as product otherwise.
*/
Product productCopy(Product product);
/**
 * pCompare : compare two products and checks if they are the same or not
 * @param p1 : the first product
 * @param p2 : the second product
 * @return :
 * -1 : in case NULL was sent
 * the difference between the first product id and the second product id
 */

int productCompare(Product product1,Product product2);




double productUnitPrice(Product p,double amount);
double productPrice(Product p, double amount);
















#endif //MATAMAZOM_PRODUCT_H
