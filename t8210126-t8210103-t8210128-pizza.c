#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "t8210126-t8210103-t8210128-pizza.h"

typedef struct {
// εδώ τα πεδία που χρειάζονται για την κάθε παραγγελία
    int cid;
    int order_number;
    int pizza_quantity;
} OrderData;


struct timespec start_time, end_time;
// Χρήση της clock_gettime για να καταγράψετε την αρχή της πράξης
clock_gettime(CLOCK_REALTIME, &start_time);
// Χρήση της clock_gettime για να καταγράψετε το τέλος της πράξης
clock_gettime(CLOCK_REALTIME, &end_time);
// Υπολογισμός του χρόνου που πέρασε σε δευτερόλεπτα
long X_seconds = end_time.tv_sec - start_time.tv_sec;
long Y_seconds = end_time.tv_sec - start_time.tv_sec;



void *customer_thread(void *arg) {
    OrderData customer = *((OrderData *) arg);

    pthread_mutex_lock(&order_threads_mutex); // το λοκ γινεται εδω ? μαλλον κατω απο το while?
    
    while (customer.cid != current_thread) {   // Η συνθήκη φροντίζει οι πελάτες να εξυπηρετούνται με την σειρά από 1 έως Ncust.
        pthread_cond_wait(&order_threads_cond, &order_threads_mutex);
    } // δεν ειναι σωστο, αφου επιτρεπεται πχ ο πελατης 2 να παρει τηλ πιο μετα απο τον 3ο.

    if (customer.cid != 1) {                   // Η if είναι μέσα στο mutex καθώς ένας πελάτης έρχεται μετά από [ORDER_MIN_TIME, ORDER_MAX_TIME] από τον πρηγούμενο (επομένως μόνο όταν περάσει ο ένας πρέπει να ξεκινήσει ο χρόνος).
        random_number = rand_r(&seed);          // Αλλιώς σπάσε το ένα mutex(order_threads_mutex) σε δυο mutex.
    	sleep(random_number % (ORDER_MAX_TIME - ORDER_MIN_TIME + 1) + ORDER_MIN_TIME);
    }

    current_thread++;
    pthread_cond_broadcast(&order_threads_cond);

    while (phone_calls >= NUM_TELEPHONES) {     // Η συνθήκη φροντίζει οι κλήσεις να μην είναι παραπάνω από τους τηλεφωνητές.
		pthread_cond_wait(&call_available, &order_threads_mutex);  
    }// εδω θα ειναι λοκ τοση ωρα
    
    phone_calls ++;
	
    pthread_mutex_unlock(&order_threads_mutex);
    telephone_thread(&customer);
    pthread_exit(NULL);
}

void *telephone_thread(void *arg) {
    OrderData customer = *((OrderData *) arg);
    int pizza_type; // αρκεί να είναι τοπική μεταβλητή
    int payment; // αρκεί να είναι τοπική μεταβλητή

    random_number = rand_r(&seed);
    sleep(random_number % (PAYMENT_MAX_TIME - PAYMENT_MIN_TIME + 1) + PAYMENT_MIN_TIME);
    
    pthread_mutex_lock(&phone_mutex);
    payment = rand_r(&seed) % 100;
    if (payment < 100 - P_FAILURE) {
	pthread_mutex_lock(&print_mutex);
	printf("Η παραγγελία με αριθμό %d καταχωρήθηκε.\n", customer.order_number);
	pthread_mutex_unlock(&print_mutex);
    	successful_orders ++;
        random_number = rand_r(&seed); 
    	customer.pizza_quantity = random_number % (PIZZA_MAX_QUANTITY - PIZZA_MIN_QUANTITY + 1) + PIZZA_MIN_QUANTITY; //Fix
    	
    	for (int i; i < pizza_quantity; i++) {
    		pizza_type = rand_r(&seed) % (100); 
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
        phone_calls --;
        pthread_cond_broadcast(&call_available);
        pthread_mutex_unlock(&phone_mutex);
	cook_thread(&customer);
	 
    } else {
    	failed_orders ++;
	pthread_mutex_lock(&print_mutex);
	printf("Η παραγγελία με αριθμό %d απέτυχε.\n", customer.order_number);
	pthread_mutex_unlock(&print_mutex);
	    
	phone_calls --;
        pthread_cond_broadcast(&call_available);
        pthread_mutex_unlock(&phone_mutex);
    }
    pthread_exit(NULL);
}


void *cook_thread(void *arg) {

    OrderData customer = *((OrderData *) arg);
    pthread_mutex_lock(&cook_mutex); // ΕΡΩΤΗΣΗ: μαλλον κατω απο το while?
    
    while (cooks_occupied >= NUM_COOKS) {        // Η συνθήκη φροντίζει οι παραγγελείες που ετοιμάζονται να μην είναι παραπάνω από τους μάγειρες.
		pthread_cond_wait(&cook_available, &cook_mutex);  
    }
    
    cooks_occupied ++;
    for(int i = 0; i < customer.pizza_quantity ; i++){
    	sleep(PREP_TIME);    
    }
    
    pthread_mutex_unlock(&cook_mutex);
    pthread_cond_signal(&cook_available);
    oven_thread(&customer);
    
    pthread_mutex_lock(&cook_mutex);
    cooks_occupied --;
    pthread_mutex_unlock(&cook_mutex);
    pthread_cond_signal(&cook_available);
    sleep(BAKE_TIME)
    deliverer_thread(&customer);

    
    pthread_exit(NULL);
    
}

void *oven_thread(void *arg) {
    OrderData customer = *((OrderData *) arg);
    pthread_mutex_lock(&oven_mutex);
	
    while (ovens_occupied >= NUM_OVENS) {        
		pthread_cond_wait(&oven_available, &oven_mutex);  
    }	
    ovens_occupied = ovens_occupied + customer.pizza_quantity;
    pthread_mutex_unlock(&oven_mutex);
    pthread_cond_signal(&oven_available);
    pthread_exit(NULL);
}
}

void *deliverer_thread(void *arg) {
     OrderData customer = *((OrderData *) arg);
     pthread_mutex_lock(&deliverer_mutex);
     while (deliverers_occupied >= NUM_DELIVERERS) {        
		pthread_cond_wait(&deliverer_available, &deliverer_mutex);  
     }
     ovens_occupied --;
     deliverers_occupied ++;
     sleep(PACK_TIME);
     random_number = rand_r(&seed); 
     delivery_time = random_number % (DELIVERY_MAX_TIME - DELIVERY_MIN_TIME + 1) + DELIVERY_MIN_TIME;
     sleep(delivery_time);
     sleep(delivery_time);
     deliverers_occupied --;
	
     pthread_mutex_unlock(&deliverer_mutex);
     pthread_cond_signal(&deliverer_available);
     pthread_exit(NULL);
}


void print_statistics() {
   
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
	customer[i]->order_number = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer[i]);
    }
   
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customer_threads[i], NULL);
    }
	
    print_statistics();

    
    return 0;
}
