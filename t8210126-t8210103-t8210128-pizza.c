#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "t8210126-t8210103-t8210128-pizza.h"

typedef struct {
// εδώ τα πεδία που χρειάζονται για την κάθε παραγγελία
    int cid;
    int order_number;
    int pizza_quantity;
} OrderData;

typedef struct {
    int oid;
    double serve_time; 
    double cold_time;
} SuccessfulOrder;


void *customer_thread(void *arg) {
    OrderData customer = *((OrderData *) arg);

    pthread_mutex_lock(&order_threads_mutex);
    
    while (customer.cid != current_thread) {   // Η συνθήκη φροντίζει οι πελάτες να εξυπηρετούνται με την σειρά από 1 έως Ncust.
        pthread_cond_wait(&order_threads_cond, &order_threads_mutex);
    }

    if (customer.cid != 1) {                   // Η if είναι μέσα στο mutex καθώς ένας πελάτης έρχεται μετά από [ORDER_MIN_TIME, ORDER_MAX_TIME] από τον πρηγούμενο (επομένως μόνο όταν περάσει ο ένας πρέπει να ξεκινήσει ο χρόνος).
        random_number = rand_r(&seed);          // Αλλιώς σπάσε το ένα mutex(order_threads_mutex) σε δυο mutex.
    	sleep(random_number % (ORDER_MAX_TIME - ORDER_MIN_TIME + 1) + ORDER_MIN_TIME);
    }

    current_thread++;
    pthread_cond_broadcast(&order_threads_cond);

    // printf("Customer: %d \n is calling/waiting available phones", customer.cid);

    while (phone_calls >= NUM_TELEPHONES) {     // Η συνθήκη φροντίζει οι κλήσεις να μην είναι παραπάνω από τους τηλεφωνητές.
		pthread_cond_wait(&call_available, &order_threads_mutex);  
    }
    
    phone_calls ++;

    pthread_mutex_unlock(&order_threads_mutex);

    telephone_thread(&customer);
    pthread_exit(NULL);
    // εδώ περιμένουν οι πελάτες για ελεύθερο τηλεφωνητή
}

void *telephone_thread(void *arg) {
    OrderData customer = *((OrderData *) arg);
    int pizza_type; // αρκεί να είναι τοπική μεταβλητή
    int payment; // αρκεί να είναι τοπική μεταβλητή
    
    // printf("Customer: %d \n is going to order", customer.cid);

    random_number = rand_r(&seed);
    sleep(random_number % (PAYMENT_MAX_TIME - PAYMENT_MIN_TIME + 1) + PAYMENT_MIN_TIME);
    
    pthread_mutex_lock(&phone_mutex);
    payment = rand_r(&seed) % 100;
    if (payment < 100 - P_FAILURE) {
    	successful_orders ++;
        random_number = rand_r(&seed); // ιδιο ονομα χμμ
    	customer.pizza_quantity = random_number % (PIZZA_MAX_QUANTITY - PIZZA_MIN_QUANTITY + 1) + PIZZA_MIN_QUANTITY; //Fix
    	
    	for (int i; i < pizza_quantity; i++) {
    		pizza_type = rand_r(&seed) % (100); // P(m) + P(p) + P(s) = 100%
    		if (pizza_type < P_MARGARITA) {
    			margherita_sold ++;
    			total_revenue += C_MARGARITA;
    		} else if (pizza_type < P_MARGARITA + P_PEPPERONI) {
    			pepperoni_sold ++;
    			total_revenue += C_PEPPERONI;
    		} else {
    			special_sold++;
    			total_revenue += C_SPECIAL;
            	}
		
    	}
    	
    } else {
    	failed_orders ++;
    }
    
    phone_calls --;
    pthread_cond_broadcast(&call_available);

    pthread_mutex_unlock(&phone_mutex);
    
    pthread_exit(NULL);
    // εδώ εκτελούνται οι παραγγελείες
}


void *cook_thread(void *arg) {

    OrderData customer = *((OrderData *) arg);
    pthread_mutex_lock(&cook_mutex); // ΕΡΩΤΗΣΗ: μαλλον κατω απο το while?
    
    while (cooks_occupied >= NUM_COOKS) {        // Η συνθήκη φροντίζει οι παραγγελείες που ετοιμάζονται να μην είναι παραπάνω από τους μάγειρες.
		pthread_cond_wait(&cook_available, &cook_mutex);  
    }
    
    cooks_occupied ++;
    sleep(PREP_TIME) * customer->pizza_quantity; //Fix
    pthread_mutex_unlock(&cook_mutex);
    
    pthread_cond_signal(&cook_available);
    
    // 
    pthread_exit(NULL);
}


void *deliverer_thread(void *arg) {

 OrderData customer = *((OrderData *) arg);
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

    int Ncust = atoi(argv[1]);
    seed = atoi(argv[2]);
    
    pthread_t customer_threads[Ncust];
    OrderData customer[Ncust];
   
    for (int i = 0; i < Ncust; i++) {
        customer[i] = (OrderData *)malloc(sizeof(OrderData));
        customer[i]->cid = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer[i]);
    }
   
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customer_threads[i], NULL);
    }
    // Τυπώνουμε τα τελικά αποτελέσματα

    
    return 0;
}
