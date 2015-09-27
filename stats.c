#include "stats.h"

char *line;
size_t len;

double times[1000];
double memories[1000]; 

double parse_double(char*);

int main(){
    int count = 0;
    int i;
    double stats[4];

    /*Read all lines*/
    while(getline(&line, &len, stdin) != -1){
        line = strtok(line, "\n");
        if(line == NULL) break;

        /*Separate data by colon*/
        times[count] = parse_double(line);
        memories[count] = parse_double(NULL);

        count++;
    }

    calculate_statistics(times, count, stats);

    printf("mean = %f\n", stats[0]);
    printf("SD = %f\n", stats[1]);
    printf("low CI = %f\n", stats[2]);
    printf("high CI = %f\n", stats[3]);

    for(i = 0; i < count; ++i)
        printf("%.0f, %.0f\n",times[i], memories[i]);

}

double parse_double(char* token){
    return strtod(strtok(token, ","), NULL);
}

void calculate_statistics(double values[], int amount, double stats[]){
    double sum = 0;
    double mean;
    double variance_sum = 0;
    double variance;
    double standard_deviation;
    double low_interval;
    double high_interval;
    int i;

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
}