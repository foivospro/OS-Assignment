#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

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

pthread_mutex_t tel_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cook_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oven_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t deliverer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t revenue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tel_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cook_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t oven_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t deliverer_cond = PTHREAD_COND_INITIALIZER;

int total_revenue = 0;
int margherita_sold = 0;
int pepperoni_sold = 0;
int special_sold = 0;
int successful_orders = 0;
int failed_orders = 0;