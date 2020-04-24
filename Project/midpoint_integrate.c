#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<math.h>
#include<pthread.h>


struct Data {
    int id;
    double start;
    double end;
    double space;
    double samples;
};

void *run(void *vp) {
    struct Data *d = (struct Data *) vp;
    double t_start = d->start + (d->space * d->id);
    double t_end = t_start + d->space;
    double interval = (t_end - t_start) / d->samples;
    double sum_area = 0;
    for(double i = t_start + interval; i <= t_end; i = i + interval)
    {
        double midpoint = i - (interval / 2);
        double height = (sin(midpoint)) / midpoint;
        double area = height * interval;
        sum_area = sum_area + area;
    }
    free(vp);
    double * result = (double *) malloc(sizeof(double));
    *result = sum_area;
    return result;
}

int main(int argc, char *argv[])
{
    pthread_t *tids;
    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int sample_size = atoi(argv[3]);
    int n_threads = atoi(argv[4]);
    double space_size = ((double)end - (double)start) / n_threads;
    double thread_samples = sample_size / n_threads;
    double sum = 0;

    tids = (pthread_t *) malloc (n_threads * sizeof(pthread_t));
    for (int i = 0; i < n_threads; i++) {
        struct Data *data = (struct Data *) malloc(sizeof(struct Data));
        data->id = i;
        data->start = start;
        data->end = end;
        data->space = space_size;
        data->samples = thread_samples;

        int rv = pthread_create(&tids[i], NULL, run, (void*) data);
        assert(rv==0);
    }
    for (int i = 0; i < n_threads; i++) {
        void *rtval;
        int rv = pthread_join(tids[i], &rtval);
        sum += *(double*)rtval;
        free(rtval);
        assert(rv==0);
    }
    printf("%0.30f \n",sum);
    return 0;
}
