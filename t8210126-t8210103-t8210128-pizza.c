#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "t8210126-t8210103-t8210128-pizza.h"

typedef struct {
    int order_number;
    // εδώ τα πεδία που χρειάζονται για την κάθε παραγγελία
} OrderData;

void *customer_thread(void *arg) {

    int customer_id = *((int *)arg);
    pthread_mutex_lock(&calling_mutex);
    
    if (*customer_id != 1) {
    	sleep(rand() % (ORDER_MAX_TIME - ORDER_MIN_TIME + 1) + ORDER_MIN_TIME);
    }
    
    while (phone_calls > NUM_TELEPHONES) {
		pthread_cond_wait(&call_available, &calling_mutex);
    }
    
    phone_calls ++;
    pthread_mutex_unlock(&calling_mutex);
    
    pthread_exit(NULL);
    // εδώ περιμένουν οι πελάτες για ελεύθερο τηλεφωνητή
}


void *telephone_thread(void *arg) {
    int telephone_id = *((int *)arg);
    
    pthread_mutex_lock(&phone_mutex);
    
    sleep(rand() % (PAYMENT_MAX_TIME - PAYMENT_MIN_TIME + 1) + PAYMENT_MIN_TIME);
    
    payment = rand() % 105; //P(fail) + P(m) + P(p) + P(s) = 105%
    if (payment < 100) {
    	successful_orders ++;
    	pizza_quantity = rand() % (PIZZA_MAX_QUANTITY - PIZZA_MIN_QUANTITY + 1) + PIZZA_MIN_QUANTITY;
    	
    	for (int i; i < pizza_quantity; i++) {
    		pizza_type = rand() % (100) //P(m) + P(p) + P(s) = 100%
    		if (pizza_type < P_MARGARITA) {
    			margherita_sold ++;
    			total_revenue += C_MARGARITA;
    		} else if (pizza_type < P_MARGARITA + P_PEPERONI) {
    			pepperoni_sold ++;
    			total_revenue += C_PEPERONI;
    		} else {
    			special_sold++;
    			total_revenue += C_SPECIAL;
            }
    	}
    	
    } else {
    	failed_orders ++;
    }
    
    phone_calls --;
    pthread_cond_signal(&call_available);

    pthread_mutex_unlock(&phone_mutex);
    
    pthread_exit(NULL);
    // εδώ εκτελούνται οι παραγγελείες
}


void *cook_thread(void *arg) {
    int cook_id = *((int *)arg);
    // 
    pthread_exit(NULL);
}


void *deliverer_thread(void *arg) {
 while (1) { // εβαλα 1 γιατι δεν ξερω αν το vm του εργαστηριου εχει c99 η μεταγενεστερη εκδοση της c
        
        pthread_mutex_lock(&order_mutex);
        int order_number = next_order_number++;
        pthread_mutex_unlock(&order_mutex);
     
        printf("Deliverer %d delivers order number %d\n", deliverer_id, order_number);
     
        sleep(1);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Ncust> <Seed>\n", argv[0]);
        return 1;
    }

    Ncust = atoi(argv[1]);
    int seed = atoi(argv[2]);
    srand(seed);
    
    pthread_t customer_threads[Ncust];
    pthread_t telephone_threads[NUM_TELEPHONES];
    pthread_t cook_threads[NUM_COOKS];
    pthread_t deliverer_threads[NUM_DELIVERERS];

   
    for (int i = 0; i < Ncust; i++) {
        int *customer_id = malloc(sizeof(int));
        *customer_id = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer_id);
    }

   
    for (int i = 0; i < NUM_TELEPHONES; i++) {
        int *telephone_id = malloc(sizeof(int));
        *telephone_id = i + 1;
        pthread_create(&telephone_threads[i], NULL, telephone_thread, (void *)telephone_id);
    }

   
    for (int i = 0; i < NUM_COOKS; i++) {
        int *cook_id = malloc(sizeof(int));
        *cook_id = i + 1;
        pthread_create(&cook_threads[i], NULL, cook_thread, (void *)cook_id);
    }


    for (int i = 0; i < NUM_DELIVERERS; i++) {
        int *deliverer_id = malloc(sizeof(int));
        *deliverer_id = i + 1;
        pthread_create(&deliverer_threads[i], NULL, deliverer_thread, (void *)deliverer_id);
    }

   
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customer_threads[i], NULL);
    }

 
    for (int i = 0; i < NUM_TELEPHONES; i++) {
        pthread_join(telephone_threads[i], NULL);
    }

  
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cook_threads[i], NULL);
    }

    
    for (int i = 0; i < NUM_DELIVERERS; i++) {
        pthread_join(deliverer_threads[i], NULL);
    }

    // Τυπώνουμε τα τελικά αποτελέσματα

    
    return 0;
}
