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
    rc = pthread_mutex_lock(&order_threads_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    while (customer->cid != current_thread) {   // Η συνθήκη φροντίζει οι πελάτες να εξυπηρετούνται με την σειρά από 1 έως Ncust.
        rc = pthread_cond_wait(&order_threads_cond, &order_threads_mutex);
        if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
		pthread_exit(&(customer->cid));
	    }
    } 
    clock_gettime(CLOCK_REALTIME, &customer->arrival_time);
    if (customer->cid != 1) {                   // Η if είναι μέσα στο mutex καθώς ένας πελάτης έρχεται μετά από [ORDER_MIN_TIME, ORDER_MAX_TIME] από τον πρηγούμενο (επομένως μόνο όταν περάσει ο ένας πρέπει να ξεκινήσει ο χρόνος).         // Αλλιώς σπάσε το ένα mutex(order_threads_mutex) σε δυο mutex.
        random_number = rand_r(&seed);
        sleep(random_number % (ORDER_MAX_TIME - ORDER_MIN_TIME + 1) + ORDER_MIN_TIME);
    }
    current_thread++;
    rc = pthread_cond_broadcast(&order_threads_cond);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_cond_broadcast() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }	
    while (phone_calls >= NUM_TELEPHONES) {     // Η συνθήκη φροντίζει οι κλήσεις να μην είναι παραπάνω από τους τηλεφωνητές.
        rc = pthread_cond_wait(&call_available, &order_threads_mutex);
        if (rc != 0) {	
		printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
		pthread_exit(&(customer->cid));
	} 
    }
    phone_calls ++;
    rc = pthread_mutex_unlock(&order_threads_mutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
		pthread_exit(&(customer->cid));
	}
    telephone_thread(customer);
    pthread_exit(NULL);
}

void *telephone_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    int pizza_type; // αρκεί να είναι τοπική μεταβλητή
    int payment; // αρκεί να είναι τοπική μεταβλητή
    random_number = rand_r(&seed);
    sleep(random_number % (PAYMENT_MAX_TIME - PAYMENT_MIN_TIME + 1) + PAYMENT_MIN_TIME);
    rc = pthread_mutex_lock(&phone_mutex);
	if (rc != 0) {	
		printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
		pthread_exit(&(customer->cid));
	}
    random_number = rand_r(&seed);
    payment = random_number % 100;
    if (payment < 100 - P_FAILURE) {
        rc = pthread_mutex_lock(&print_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
        printf("Η παραγγελία με αριθμό %d καταχωρήθηκε.\n", customer->order_number);
        rc = pthread_mutex_unlock(&print_mutex);
    	if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&(customer->cid));
	}
        customer->status = SUCCESSFUL;
    	successful_orders ++;
        random_number = rand_r(&seed);
    	customer->pizza_quantity = random_number % (PIZZA_MAX_QUANTITY - PIZZA_MIN_QUANTITY + 1) + PIZZA_MIN_QUANTITY; 
        for (int i = 0; i < customer->pizza_quantity; i++) {
            random_number = rand_r(&seed);
    		pizza_type = random_number % (100); 
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
        rc = pthread_cond_broadcast(&call_available);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_cond_broadcast() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
        rc = pthread_mutex_unlock(&phone_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
	cook_thread(customer);
    } else {
        customer->status = FAILED;
    	failed_orders ++;
        rc = pthread_mutex_lock(&print_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
        printf("Η παραγγελία με αριθμό %d απέτυχε.\n", customer->order_number);
        rc = pthread_mutex_unlock(&print_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&(customer->cid));
	}
	phone_calls --;
        rc = pthread_cond_broadcast(&call_available);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_cond_broadcast() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
        rc = pthread_mutex_unlock(&phone_mutex);
    	if (rc != 0) {	
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
    }
    pthread_exit(NULL);
}


void *cook_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    rc = pthread_mutex_lock(&cook_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }  
    while (cooks_occupied >= NUM_COOKS) {        // Η συνθήκη φροντίζει οι παραγγελείες που ετοιμάζονται να μην είναι παραπάνω από τους μάγειρες.
    	rc = pthread_cond_wait(&cook_available, &cook_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
    }
    cooks_occupied ++;
    rc = pthread_mutex_unlock(&cook_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    for(int i = 0; i < customer->pizza_quantity ; i++){
    	sleep(PREP_TIME);    
    }
    oven_thread(customer);
    rc = pthread_mutex_lock(&cook_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    cooks_occupied --;
    rc = pthread_mutex_unlock(&cook_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    rc = pthread_cond_signal(&cook_available);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }	
    sleep(BAKE_TIME);
    clock_gettime(CLOCK_REALTIME, &customer->completion_time);
    deliverer_thread(customer); 
    pthread_exit(NULL);
}

void *oven_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    rc = pthread_mutex_lock(&oven_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    while (ovens_occupied + customer->pizza_quantity > NUM_OVENS) {        
	rc = pthread_cond_wait(&oven_available, &oven_mutex);
        if (rc != 0) {	
                printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
                pthread_exit(&(customer->cid));
        }  
    }	
    ovens_occupied = ovens_occupied + customer->pizza_quantity;
    rc = pthread_mutex_unlock(&oven_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
}


void *deliverer_thread(void *arg) {
    OrderData *customer = (OrderData *) arg;
    int delivery_time;
    rc = pthread_mutex_lock(&deliverer_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    while (deliverers_occupied >= NUM_DELIVERERS) {        
	rc = pthread_cond_wait(&deliverer_available, &deliverer_mutex);
        if (rc != 0) {	
            printf("ERROR: return code from pthread_cond_wait() is %d\n", rc);
            pthread_exit(&(customer->cid));
        }
     }
    deliverers_occupied ++;
    ovens_occupied = ovens_occupied - customer->pizza_quantity;
    rc = pthread_mutex_unlock(&deliverer_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    sleep(PACK_TIME);
    clock_gettime(CLOCK_REALTIME, &customer->pack_time);
    long X_seconds = customer->pack_time.tv_sec - customer->arrival_time.tv_sec;
    rc = pthread_mutex_lock(&print_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    printf("Η παραγγελία με αριθμό %d ετοιμάστηκε σε %ld λεπτά.\n", customer->order_number, X_seconds); 
    rc = pthread_mutex_unlock(&print_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    random_number = rand_r(&seed);
    delivery_time = (random_number % (DELIVERY_MAX_TIME - DELIVERY_MIN_TIME + 1)) + DELIVERY_MIN_TIME;
    sleep(delivery_time);
    clock_gettime(CLOCK_REALTIME, &customer->delivery_time);
    long Y_seconds = customer->delivery_time.tv_sec - customer->arrival_time.tv_sec;
    rc = pthread_mutex_lock(&print_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    printf("Η παραγγελία με αριθμό %d παραδώθηκε σε %ld λεπτά.\n", customer->order_number, Y_seconds);
    rc = pthread_mutex_unlock(&print_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    sleep(delivery_time);
    rc = pthread_mutex_lock(&deliverer_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
    deliverers_occupied --;
    rc = pthread_cond_signal(&deliverer_available);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_cond_signal() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }	
    rc = pthread_mutex_unlock(&deliverer_mutex);
    if (rc != 0) {	
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", rc);
        pthread_exit(&(customer->cid));
    }
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
        rc = pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer[i]);
        if (rc != 0) {
    		printf("ERROR: return code from pthread_create() is %d\n", rc);
       		exit(-1);
	    }
    }
    for (int i = 0; i < Ncust; i++) {
        rc = pthread_join(customer_threads[i], NULL);
        if (rc != 0) {
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);	
		}

	free(customer[i]);
    }
    print_statistics(Ncust, customer);
    // Καταστροφή mutex και condition
	rc = pthread_mutex_destroy(&order_threads_mutex);
if (rc != 0) {
	printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
	exit(-1);		
	}
    rc = pthread_mutex_destroy(&calling_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_mutex_destroy(&oven_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_mutex_destroy(&phone_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_mutex_destroy(&cook_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_mutex_destroy(&deliverer_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_mutex_destroy(&print_mutex);
	if (rc != 0) {
		printf("ERROR: return code from pthread_mutex_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_cond_destroy(&order_threads_cond);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_cond_destroy(&call_available);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_cond_destroy(&oven_available);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_cond_destroy(&cook_available);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}
    rc = pthread_cond_destroy(&deliverer_available);
	if (rc != 0) {
		printf("ERROR: return code from pthread_cond_destroy() is %d\n", rc);
		exit(-1);		
	}

    return 0;
}
