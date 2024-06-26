#include <pthread.h>

#define NUM_TELEPHONES 2
#define NUM_COOKS 2
#define NUM_OVENS 10
#define NUM_DELIVERERS 10
#define ORDER_MIN_TIME 1
#define ORDER_MAX_TIME 5
#define PIZZA_MIN_QUANTITY 1
#define PIZZA_MAX_QUANTITY 5
#define P_MARGARITA 35
#define P_PEPPERONI 25
#define P_SPECIAL 40
#define PAYMENT_MIN_TIME 1
#define PAYMENT_MAX_TIME 3
#define P_FAILURE 5
#define C_MARGARITA 10
#define C_PEPPERONI 11
#define C_SPECIAL 12
#define PREP_TIME 1
#define BAKE_TIME 10
#define PACK_TIME 1
#define DELIVERY_MIN_TIME 5
#define DELIVERY_MAX_TIME 15

pthread_mutex_t order_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t order_threads_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t calling_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t call_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t oven_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t oven_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t phone_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cook_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cook_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t deliverer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t deliverer_available = PTHREAD_COND_INITIALIZER;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int seed;
int random_number;
int rc;
int current_thread = 1;
int phone_calls = 0;
int total_revenue = 0;
int margherita_sold = 0;
int pepperoni_sold = 0;
int special_sold = 0;
int successful_orders = 0;
int failed_orders = 0;
int cooks_occupied = 0;
int ovens_occupied = 0;
int deliverers_occupied = 0;
double total_service_time = 0;
double total_cooling_time = 0;
int max_service_time = 0;
int max_cooling_time = 0;
