#include <pthread.h>

struct estacao {
    pthread_cond_t car_ready;    
    pthread_cond_t ready_to_embark;   
    pthread_cond_t ready_to_leave;
    pthread_cond_t car_left; 
    pthread_cond_t station_empty;
    pthread_mutex_t mutex;
    //int passengers;
    int embarking;
    int free_spots;
};

void estacao_init(struct estacao *station) {
    //station->passengers = 0;    
    station->embarking = 0;
    station->free_spots = 0;
    pthread_cond_init(&station->car_ready, NULL);     
    pthread_cond_init(&station->ready_to_embark, NULL); 
    pthread_cond_init(&station->ready_to_leave, NULL);
    pthread_cond_init(&station->car_left, NULL);
    pthread_cond_init(&station->station_empty, NULL);
    pthread_mutex_init(&station->mutex, NULL);
}

void estacao_preencher_vagao(struct estacao * station, int assentos) {
    pthread_mutex_lock(&station->mutex);
    while(station->free_spots > 0) {  //dispensável porque no pdf diz para "assumir que nunca haverá mais que um carro na estação por vez", mas como já tinha feito, deixei
        //printf("another car waiting with %d free spots\n", station->free_spots);
        pthread_cond_wait(&station->station_empty, &station->mutex);
    }  
    station->free_spots = assentos; 
    //printf("Car waiting with %d free spots\n", station->free_spots);
    pthread_cond_broadcast(&station->car_ready);
    pthread_cond_wait(&station->ready_to_leave, &station->mutex);
    //printf("Car leaving with %d free spots\n", station->free_spots);
    pthread_cond_signal(&station->car_left);
    pthread_cond_signal(&station->station_empty);
    pthread_mutex_unlock(&station->mutex); 
}

void estacao_espera_pelo_vagao(struct estacao * station) {
    pthread_mutex_lock(&station->mutex);   
    //station->passengers++; 
    //printf("%d passengers waiting\n", station->passengers);
    while (station->free_spots == station->embarking) {
        pthread_cond_wait(&station->car_ready, &station->mutex);
    }
    station->embarking++;        
    //station->passengers--;
    pthread_cond_signal(&station->ready_to_embark);
    pthread_mutex_unlock(&station->mutex);
}

void estacao_embarque(struct estacao * station) {
    pthread_mutex_lock(&station->mutex);
    while(station->embarking == 0) {   
        pthread_cond_wait(&station->ready_to_embark, &station->mutex);
    }
    station->embarking--;    
    station->free_spots--;
    //printf("Embarque realizado\n");
    //if(station->free_spots == 0 || (station->passengers == 0 && station->embarking == 0)) {
    if(station->free_spots == 0 || station->embarking == 0) {
        pthread_cond_signal(&station->ready_to_leave);
        pthread_cond_wait(&station->car_left, &station->mutex);
    }
    pthread_mutex_unlock(&station->mutex); 
}