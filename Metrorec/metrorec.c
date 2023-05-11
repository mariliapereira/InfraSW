#include <pthread.h>

struct estacao {
    pthread_cond_t car_ready, ready_to_embark, ready_to_leave, station_empty;
    pthread_mutex_t mutex;
    int free_spots;
    int passengers;
    int embarking;
};

void estacao_init(struct estacao *station) {
    station->passengers = 0;    
    station->embarking = 0;
    station->free_spots = 0;
    pthread_mutex_init(&station->mutex, NULL);
    pthread_cond_init(&station->car_ready, NULL);     
    pthread_cond_init(&station->ready_to_embark, NULL); 
    pthread_cond_init(&station->ready_to_leave, NULL);
    pthread_cond_init(&station->station_empty, NULL);
}

void estacao_preecher_vagao(struct estacao * station, int assentos) {
    pthread_mutex_lock(&station->mutex);
    while(station->free_spots > 0) {
        printf("another car waiting with %d free spots\n", station->free_spots);
        pthread_cond_wait(&station->station_empty, &station->mutex);
    }   
    station->free_spots = assentos; 
    //printf("Car waiting with %d free spots\n", station->free_spots);
    pthread_cond_broadcast(&station->car_ready);
    pthread_cond_wait(&station->ready_to_leave, &station->mutex);
    pthread_cond_signal(&station->station_empty);
    //printf("Car leaving with %d free spots\n", station->free_spots);
    pthread_mutex_unlock(&station->mutex); 
}

void estacao_espera_pelo_vagao(struct estacao * station) {
    pthread_mutex_lock(&station->mutex);   
    station->passengers++; 
    //printf("%d passengers waiting\n", station->passengers);
    while (station->free_spots == station->embarking) {
        pthread_cond_wait(&station->car_ready, &station->mutex);
    }
    station->embarking++;        
    station->passengers--;
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
    if(station->free_spots == 0 || (station->passengers == 0 && station->embarking == 0)) {
        pthread_cond_signal(&station->ready_to_leave);
    }
    //printf("Embarque realizado\n");
    pthread_mutex_unlock(&station->mutex); 
}