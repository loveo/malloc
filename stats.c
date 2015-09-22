#include <stdio.h>
#include "stats.h"

int main(){
    double data[3] = {4,5,6};
    double* vals = calculate_statistics(data, 3);
    printf("mean = %f\n", vals[0]);
    printf("SD = %f\n", vals[1]);
    printf("low CI = %f\n", vals[2]);
    printf("high CI = %f\n", vals[3]);
}

double* calculate_statistics(double values[], int amount){
    double sum = 0;
    double mean;
    double variance_sum = 0;
    double variance;
    double standard_deviation;
    double low_interval;
    double high_interval;
    int i;
    double* stats = malloc(4*sizeof(double));

    /*Summarize values*/
    for(i = 0; i < amount; ++i){
        sum += values[i];
    }

    /*Calculate mean value*/
    mean = sum/amount;

    /*Calculate squared variance sum*/
    for(i = 0; i < amount; ++i){
        variance_sum += (values[i] - mean) * (values[i] - mean);
    }

    /*Calculate variance*/
    variance = variance_sum/(amount-1);

    /*Calculate standard deviation*/
    standard_deviation = sqrt(variance);

    /*Calculate low and high intervals for confidence interval, assuming 95% approximation*/
    low_interval = mean - 1.96*standard_deviation;
    high_interval = mean + 1.96*standard_deviation;

    stats[0] = mean;
    stats[1] = standard_deviation;
    stats[2] = low_interval;
    stats[3] = high_interval;

    return stats;
}