

#ifndef MATAMAZOM_ORDER_H
#define MATAMAZOM_ORDER_H
#include <stdbool.h>
#include "amount_set.h"
#include "product.h"

/**
* order Container
*
* Implements a order container type.
*
* The following functions are available:
*  orderCreate		          - Creates a new empty order
*  orderDestroy              - Deletes an existing order and frees all resources
*  orderCopy		          - Copies an existing order
 *  orderCompare
*/

/** Type for defining the order */
typedef struct order_t *Order;

/** Type used for returning error codes from order functions */
typedef enum OrderResult_t {
    ORDER_SUCCESS,
    ORDER_OUT_OF_MEMORY,
    ORDER_NULL_ARGUMENT,
    ORDER_ITEM_ALREADY_EXISTS
} OrderResult;

/**
* orderCreate: Allocates a new order.
*
* @param id - the new order id
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new order in case of success.
*/
Order orderCreate(unsigned int id);

/**
* orderDestroy: Deallocates an existing order. Clears all elements by using the
* stored free functions.
*
* @param order - Target order to be deallocated. If order is NULL nothing will be
* 		done
*/
void orderDestroy(Order order);

/**
* orderCopy: Creates a copy of target order.
* @param order - Target order.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A order containing the same elements as order otherwise.
*/
Order orderCopy(Order order);
/**
 * Type of function used  to identify equal orders.
 * This function should return:
 *     A positive integer if the first order is greater;
 *     0 if they're equal;
 *     A negative integer if the second order is greater.
 */
int orderCompare(Order order1,Order order2);























#endif //MATAMAZOM_ORDER_H
