#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include "t8210126-t8210103-t8210128-pizza.h"

void *telephone_thread(void *arg);
void *cook_thread(void *arg);
void *oven_thread(void *arg);
void *deliverer_thread(void *arg);

typedef enum {
    SUCCESSFUL,
    FAILED
} OrderStatus;

typedef struct {
    int cid;
    int order_number;
    int pizza_quantity;
    struct timespec arrival_time;      
    struct timespec completion_time;
    struct timespec delivery_time;   
    struct timespec pack_time;        
    OrderStatus status;    
} OrderData;


void *customer_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    pthread_mutex_lock(&order_threads_mutex); 
    while (customer->cid != current_thread) {   // Η συνθήκη φροντίζει οι πελάτες να εξυπηρετούνται με την σειρά από 1 έως Ncust.
        pthread_cond_wait(&order_threads_cond, &order_threads_mutex);
    } 
    clock_gettime(CLOCK_REALTIME, &customer->arrival_time);
    if (customer->cid != 1) {                   // Η if είναι μέσα στο mutex καθώς ένας πελάτης έρχεται μετά από [ORDER_MIN_TIME, ORDER_MAX_TIME] από τον πρηγούμενο (επομένως μόνο όταν περάσει ο ένας πρέπει να ξεκινήσει ο χρόνος).
        random_number = rand_r(&seed);          // Αλλιώς σπάσε το ένα mutex(order_threads_mutex) σε δυο mutex.
        sleep(random_number % (ORDER_MAX_TIME - ORDER_MIN_TIME + 1) + ORDER_MIN_TIME);
    }
    current_thread++;
    pthread_cond_broadcast(&order_threads_cond);
    while (phone_calls >= NUM_TELEPHONES) {     // Η συνθήκη φροντίζει οι κλήσεις να μην είναι παραπάνω από τους τηλεφωνητές.
        pthread_cond_wait(&call_available, &order_threads_mutex);  
    }
    phone_calls ++;
    pthread_mutex_unlock(&order_threads_mutex);
    telephone_thread(customer);
    pthread_exit(NULL);
}

void *telephone_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    int pizza_type; // αρκεί να είναι τοπική μεταβλητή
    int payment; // αρκεί να είναι τοπική μεταβλητή
    random_number = rand_r(&seed);
    sleep(random_number % (PAYMENT_MAX_TIME - PAYMENT_MIN_TIME + 1) + PAYMENT_MIN_TIME);
    pthread_mutex_lock(&phone_mutex);
    payment = rand_r(&seed) % 100;
    if (payment < 100 - P_FAILURE) {
        pthread_mutex_lock(&print_mutex);
        printf("Η παραγγελία με αριθμό %d καταχωρήθηκε.\n", customer->order_number);
        pthread_mutex_unlock(&print_mutex);
        customer->status = SUCCESSFUL;
    	successful_orders ++;
        random_number = rand_r(&seed); 
    	customer->pizza_quantity = random_number % (PIZZA_MAX_QUANTITY - PIZZA_MIN_QUANTITY + 1) + PIZZA_MIN_QUANTITY; 
        for (int i = 0; i < customer->pizza_quantity; i++) {
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
	    cook_thread(customer);
    } else {
        customer->status = FAILED;
    	failed_orders ++;
        pthread_mutex_lock(&print_mutex);
        printf("Η παραγγελία με αριθμό %d απέτυχε.\n", customer->order_number);
        pthread_mutex_unlock(&print_mutex);
	    phone_calls --;
        pthread_cond_broadcast(&call_available);
        pthread_mutex_unlock(&phone_mutex);
    }
    pthread_exit(NULL);
}


void *cook_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    pthread_mutex_lock(&cook_mutex);  
    while (cooks_occupied >= NUM_COOKS) {        // Η συνθήκη φροντίζει οι παραγγελείες που ετοιμάζονται να μην είναι παραπάνω από τους μάγειρες.
		pthread_cond_wait(&cook_available, &cook_mutex);  
    }
    cooks_occupied ++;
    pthread_mutex_unlock(&cook_mutex);
    for(int i = 0; i < customer->pizza_quantity ; i++){
    	sleep(PREP_TIME);    
    }
    oven_thread(customer);
    pthread_mutex_lock(&cook_mutex);
    cooks_occupied --;
    pthread_mutex_unlock(&cook_mutex);
    pthread_cond_signal(&cook_available);
    sleep(BAKE_TIME);
    clock_gettime(CLOCK_REALTIME, &customer->completion_time);
    deliverer_thread(customer); 
    pthread_exit(NULL);
}

void *oven_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    pthread_mutex_lock(&oven_mutex);
    while (ovens_occupied + customer->pizza_quantity > NUM_OVENS) {        
		pthread_cond_wait(&oven_available, &oven_mutex);  
    }	
    ovens_occupied = ovens_occupied + customer->pizza_quantity;
    pthread_mutex_unlock(&oven_mutex);
}


void *deliverer_thread(void *arg) {
     OrderData *customer = (OrderData *) arg;
     int delivery_time;
     pthread_mutex_lock(&deliverer_mutex);
     while (deliverers_occupied >= NUM_DELIVERERS) {        
		pthread_cond_wait(&deliverer_available, &deliverer_mutex);  
     }
     deliverers_occupied ++;
     ovens_occupied = ovens_occupied - customer->pizza_quantity;
     pthread_mutex_unlock(&deliverer_mutex);
     sleep(PACK_TIME);
     clock_gettime(CLOCK_REALTIME, &customer->pack_time);
     long X_seconds = customer->pack_time.tv_sec - customer->arrival_time.tv_sec;
     pthread_mutex_lock(&print_mutex);
     printf("Η παραγγελία με αριθμό %d ετοιμάστηκε σε %ld λεπτά.\n", customer->order_number, X_seconds); 
     pthread_mutex_unlock(&print_mutex);
     random_number = rand_r(&seed); 
     delivery_time = (random_number % (DELIVERY_MAX_TIME - DELIVERY_MIN_TIME + 1)) + DELIVERY_MIN_TIME;
     sleep(delivery_time);
     clock_gettime(CLOCK_REALTIME, &customer->delivery_time);
     long Y_seconds = customer->delivery_time.tv_sec - customer->arrival_time.tv_sec;
     pthread_mutex_lock(&print_mutex);
	 printf("Η παραγγελία με αριθμό %d παραδώθηκε σε %ld λεπτά.\n", customer->order_number, Y_seconds);
	 pthread_mutex_unlock(&print_mutex);
     sleep(delivery_time);
     pthread_mutex_lock(&deliverer_mutex);
     deliverers_occupied --;
     pthread_cond_signal(&deliverer_available);
     pthread_mutex_unlock(&deliverer_mutex);
     pthread_exit(NULL);
}


void print_statistics(int Ncust, OrderData **orders) {
    for (int i = 0; i < Ncust; i++) {
            if (orders[i]->status == SUCCESSFUL) {
                long service_time = (orders[i]->delivery_time.tv_sec - orders[i]->arrival_time.tv_sec); 
                long cooling_time = (orders[i]->delivery_time.tv_sec - orders[i]->completion_time.tv_sec);
                total_service_time += service_time;
                total_cooling_time += cooling_time;
                if (service_time > max_service_time) {
                    max_service_time = service_time;
                }
                if (cooling_time > max_cooling_time) {
                    max_cooling_time = cooling_time;
                }
            }
        }

    double avg_service_time = (double)total_service_time / (Ncust - failed_orders);
    double avg_cooling_time = (double)total_cooling_time / (Ncust - failed_orders);

    printf("\nΤα συνολικά έσοδα από τις πωλήσεις είναι: %d\u20AC\n", total_revenue);
    printf("\nΠίτσες που πουλήθηκαν από κάθε τύπο\n");
    printf("Μαργαρίτα: %d\n", margherita_sold);
    printf("Πεπερόνι: %d\n", pepperoni_sold);
    printf("Σπέσιαλ: %d\n\n", special_sold);
    printf("Πλήθος επιτυχημένων παραγγελιών: %d\n", Ncust - failed_orders);
    printf("Πλήθος αποτυχημένων παραγγελιών: %d\n", failed_orders);
    printf("Μέσος χρόνος εξυπηρέτησης: %.2f λεπτά\n", avg_service_time);
    printf("Μέγιστος χρόνος εξυπηρέτησης: %d λεπτά\n", max_service_time);
    printf("Μέσος χρόνος κρυώματος: %.2f λεπτά\n", avg_cooling_time);
    printf("Μέγιστος χρόνος κρυώματος: %d λεπτά\n", max_cooling_time);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Ncust> <Seed>\n", argv[0]);
        return 1;
    }

    int Ncust = atoi(argv[1]);
    seed = atoi(argv[2]);
    
    pthread_t customer_threads[Ncust];
    OrderData *customer[Ncust];

    for (int i = 0; i < Ncust; i++) {
        customer[i] = (OrderData *)malloc(sizeof(OrderData));
        customer[i]->cid = i + 1;
        customer[i]->order_number = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer[i]);
    }
   
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customer_threads[i], NULL);
	    free(customer[i]);
    }
	
    print_statistics(Ncust, customer);

    
    return 0;
}
