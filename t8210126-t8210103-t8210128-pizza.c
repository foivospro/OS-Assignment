#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Για τη χρήση της sleep()
#include "t8210126-t8210103-t8210128-pizza.h"


// Δημιουργία δομής για τα δεδομένα κάθε παραγγελίας
typedef struct {
    int order_number;
    // Προσθέστε εδώ τα πεδία που χρειάζονται για την κάθε παραγγελία
} OrderData;

// Συνάρτηση που θα εκτελείται από κάθε νήμα πελάτη
void *customer_thread(void *arg) {
    int customer_id = *((int *)arg);
    // Εδώ θα γίνει η υλοποίηση του νήματος πελάτη
    pthread_exit(NULL);
}

// Συνάρτηση που θα εκτελείται από κάθε νήμα τηλεφωνητή
void *telephone_thread(void *arg) {
    int telephone_id = *((int *)arg);
    // Εδώ θα γίνει η υλοποίηση του νήματος τηλεφωνητή
    pthread_exit(NULL);
}

// Συνάρτηση που θα εκτελείται από κάθε νήμα παρασκευαστή
void *cook_thread(void *arg) {
    int cook_id = *((int *)arg);
    // Εδώ θα γίνει η υλοποίηση του νήματος παρασκευαστή
    pthread_exit(NULL);
}

// Συνάρτηση που θα εκτελείται από κάθε νήμα διανομέα
void *deliverer_thread(void *arg) {
    int deliverer_id = *((int *)arg);
    // Εδώ θα γίνει η υλοποίηση του νήματος διανομέα
    pthread_exit(NULL);
}

int main() {
    pthread_t customer_threads[Ncust];
    pthread_t telephone_threads[NUM_TELEPHONES];
    pthread_t cook_threads[NUM_COOKS];
    pthread_t deliverer_threads[NUM_DELIVERERS];

    // Δημιουργία νημάτων πελατών
    for (int i = 0; i < Ncust; i++) {
        int *customer_id = malloc(sizeof(int));
        *customer_id = i + 1;
        pthread_create(&customer_threads[i], NULL, customer_thread, (void *)customer_id);
    }

    // Δημιουργία νημάτων τηλεφωνητών
    for (int i = 0; i < NUM_TELEPHONES; i++) {
        int *telephone_id = malloc(sizeof(int));
        *telephone_id = i + 1;
        pthread_create(&telephone_threads[i], NULL, telephone_thread, (void *)telephone_id);
    }

    // Δημιουργία νημάτων παρασκευαστών
    for (int i = 0; i < NUM_COOKS; i++) {
        int *cook_id = malloc(sizeof(int));
        *cook_id = i + 1;
        pthread_create(&cook_threads[i], NULL, cook_thread, (void *)cook_id);
    }

    // Δημιουργία νημάτων διανομέων
    for (int i = 0; i < NUM_DELIVERERS; i++) {
        int *deliverer_id = malloc(sizeof(int));
        *deliverer_id = i + 1;
        pthread_create(&deliverer_threads[i], NULL, deliverer_thread, (void *)deliverer_id);
    }

    // Αναμονή ολοκλήρωσης όλων των νημάτων πελατών
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    // Αναμονή ολοκλήρωσης όλων των νημάτων τηλεφωνητών
    for (int i = 0; i < NUM_TELEPHONES; i++) {
        pthread_join(telephone_threads[i], NULL);
    }

    // Αναμονή ολοκλήρωσης όλων των νημάτων παρασκευαστών
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cook_threads[i], NULL);
    }

    // Αναμονή ολοκλήρωσης όλων των νημάτων διανομέων
    for (int i = 0; i < NUM_DELIVERERS; i++) {
        pthread_join(deliverer_threads[i], NULL);
    }

    // Τυπώνουμε τα τελικά αποτελέσματα
    // Προσέξτε τη χρήση του mutex print_mutex για να αποφευχθεί η ανταγωνιστική πρόσβαση στην οθόνη εκτύπωσης

    return 0;
}
