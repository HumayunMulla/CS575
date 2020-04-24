#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {  
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    double a;
    double b;
	double sum;
};

/* Global variables */
double num_integral=0.0;
double a=0.0, b=0.0, midpoint=0.0, partitions=0.0, delx=0.0, height=0, sum = 0.0, area=0.0, weight=0.0;
int load=0, N=0, threadID=0;

/* Quadrature function declaration */
double quadrature(double a, double b);

/* Thread start function: to compute each thread values  */
static void *
thread_start(void *arg)
{
    struct thread_info *tinfo = arg;
	double sum;
    sum = quadrature(tinfo->a, tinfo->b);
	tinfo->sum = sum;
	return arg;
}

int
main(int argc, char *argv[])
{
    int s, tnum, opt, num_threads;
    struct thread_info *tinfo;
    pthread_attr_t attr;
    
	if (argc < 4)
        handle_error_en(1, "incomplete_cmd_args");
	
    a = atof(argv[1]);  // to take input of lower limit for integral
    b = atof(argv[2]);  // to take input of upper limit for integral
    N = atof(argv[3]);  // to take for number of samples for integral
	num_threads = atoi(argv[4]);

    load = N / num_threads;
    /* printf("load: %d\n",load); */

    weight = (b - a) / N;
    /* printf("weight: %.2f\n",weight); */

    /* Initialize thread creation attributes */

    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    /* Allocate memory for pthread_create() arguments */

    tinfo = calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL)
        handle_error("calloc");

    /* Create one thread for each command-line argument */

    for (tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum].thread_num = tnum + 1;
        /* printf("\nThread : %d\n",tinfo[tnum].thread_num); */

        tinfo[tnum].a = a;
        /* printf("tinfo[tnum].a : %.3f\n",tinfo[tnum].a); */

        b = weight * load + a;
        tinfo[tnum].b = b;
        /* printf("tinfo[tnum].b : %.3f\n",tinfo[tnum].b); */
        /* The pthread_create() call stores the thread ID into
            corresponding element of tinfo[] */
        
        s = pthread_create(&tinfo[tnum].thread_id, &attr,
                            &thread_start, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");

        a = b;
    }

    /* Destroy the thread attributes object, since it is no
        longer needed */

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");

    /* Now join with each thread, and display its returned value */

    for (tnum = 0; tnum < num_threads; tnum++) {
        s = pthread_join(tinfo[tnum].thread_id, NULL);
        if (s != 0)
            handle_error_en(s, "pthread_join");

        /*printf("Sum for thread %d is %.3f\n",
				tnum, tinfo[tnum].sum);*/
        num_integral += tinfo[tnum].sum;
    }
    printf("\nIntegral : %.30f\n\n", num_integral); 

    free(tinfo);
    exit(EXIT_SUCCESS);
}

/*  quadrature func will be called each time to calcualte the area
    of each function f(x) by providing limits such as a & b.
*/
double quadrature(double a, double b) {

    midpoint = (b + a) / 2; // calculate midpoint as it will be used to measure the height of function f(x)
    delx = b - a;   // width of the function f(x)
    height = sin(midpoint) / midpoint; // weight of the function f(x) also the slope of the line or the integral
    area = delx * height;

    
    return area; // return the calculated area for each function f(x)

    // initializing values of variables that are used in this func.
    midpoint = 0.0; a = 0.0; b = 0.0, delx = 0.0, height = 0.0, area = 0.0;     
} // End of quadrature function