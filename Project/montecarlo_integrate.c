#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<unistd.h>
#include<math.h>
#include<pthread.h>

#define pthreadMAX 1000000
// pthreadMAX should be less than or equal to number of samples

struct Data {
    int number;
    float sample_size;
    float a;
    float b;
    int n_threads;
    float value;
};
void *run(void *vp) {
    struct Data *d = (struct Data *) vp;
    float xmin = d->a, xmax = d->b;
    // float c = 0.0f;
    float data = 0.0f;
    if((xmax - xmin) == 0) {
        data = sin(xmax) / xmax;
    }
    else {
        for(u_int64_t count = 0; count < d->sample_size; count++) {
            float u1 = (float)rand()/(float)RAND_MAX;
            float xcord = ((xmax - xmin) * u1) + xmin;
            float vales = sin(xcord) / xcord;
            // float y = vales - c;
            // float t = d->value + y;
            // c = (t - d->value) - y;
            // d->value = t;
            data += vales;
        }
        d->value = data / d->sample_size *(xmax - xmin);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 5) {
        exit(0);
    }
    float final_value = 0;
    float c = 0.0f;
	   
	pthread_t tids[pthreadMAX]; // updated to handle 1000000
	
    float a = atoi(argv[1]);
    float b = atoi(argv[2]);
    float sample_size = atoi(argv[3]);
    float n_threads = atoi(argv[4]);
    struct Data *data = (struct Data *) malloc(sizeof(struct Data) * n_threads);
    for (int i = 0; i < n_threads; i++) {
        data[i].number = i;
        data[i].sample_size = sample_size;
        data[i].a = a;
        data[i].b = b;
        data[i].n_threads = n_threads;
        data[i].value = 0;
        int rv = pthread_create(&tids[i], NULL, run, (void*) &data[i]);
        assert(rv==0);
    }

    for (int i = 0; i < n_threads; i++) {
        int rv = pthread_join(tids[i], NULL);
        assert(rv==0);
        // float y = data[i].value - c;
        // float t = final_value + y;
        // c = (t - final_value) - y;
        // final_value = t;
    }

    for (int i = 0; i < n_threads; i++) {
        float y = data[i].value - c;
        float t = final_value + y;
        c = (t - final_value) - y;
        final_value = t;   
    }

    printf("%0.10f\n", final_value/n_threads);
    return 0;
}