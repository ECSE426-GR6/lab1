#include <stdio.h>
#include "arm_math.h"


typedef struct StateVars{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_state;

int Kalmanfilter_asm(float* InputArray, float* OutputArray, kalman_state* kstate, int Length);
int test_asm(int in);

int Kalmanfilter_C(float* inputarray, float* outputarray, kalman_state* kstate, int length){
	int i = 0;
	int rtn = 0;

	while (i < length) {
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p  / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * ( inputarray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		outputarray[i] = kstate->x;
		i++;
	}

	//Check if any of the values are nan
	if (kstate->p != kstate->p) rtn = -1;
	if (kstate->k != kstate->k) rtn = -1;
	if (kstate->x != kstate->x) rtn = -1;
	if (kstate->p != kstate->p) rtn = -1;

	return rtn;
}

void kalman_init(float q, float r, float p, float x, kalman_state* kstate)
{
	kstate->q = q;
	kstate->r = r;
	kstate->p = p;
	kstate->x = x;
	kstate->k = 0;
}

void printArray(float* array, int len){
	int i = 0;
	while (i < len) {
		printf("\t%f\n", array[i]);
		i++;
	}
}

void subtraction(float* inputarray, float* outputarray, int length, float* resultarray)
{
	int v = 0;
		while(v < length){
		resultarray[v]=inputarray[v] - outputarray[v];
			v++;
		}
}

float average(float* resultarray, float avg, int length){

	int y = 0;
	float t = 0;
	while(y<length){
	t += *( resultarray+y);
		y++;
	}
	avg = t/length;
	return avg;
}

float stddev(float avg, float* resultarray, float devout, int length){

int m = 0;
float b = 0;
	while(m<length){
	b += (*( resultarray+m)-avg)*(*( resultarray+m)-avg);
	m++;
	}
devout = sqrt(b/length);
	return devout;
}

void convolution(float* inputarray, float* outputarray, int resultlength, float* convolute, int length){
	float extendin[19];
	float extendout[19];
	int j= 0;
	int k = 0;
	int z = 0;
	while (j<=resultlength){
		extendin[j] = 0;
		extendout[j] = 0;
		j++;
	}

	while (k<length){
		extendin[k] = *(inputarray+k);
		extendout[k] = *(outputarray+k);
		k++;
	}

	while(z < 19){
		int q = 0;
		while(q < 19){
			if (z >= q){
				convolute[z] += *(extendin+q)*(*(extendout+z-q));
				q++;
			}
			else{
				break;
			}
		}
		z++;
	}
}

void correlation(float* inputarray, float* outputarray, int resultlength, float* correlate, int length){
	float extendin[19];
	float extendout[19];
	int j= 0;
	int k = 0;
	while (j<=resultlength){
		extendin[j] = 0;
	    extendout[j] = 0;
		j++;
	}

	while (k<length){
		extendin[k] = *(inputarray+k);
		extendout[length-1-k] = *(outputarray+k);
		k++;
	}
}

void new_convolution(float* in, float* out, float* res, const int len){
	int corr_len = (len*2) - 1;
	int i = 0;
	int jmin, jmax, j;

	while (i < corr_len) {
		j = 0;

		jmin = (i >= len - 1) ? i - (len - 1) : 0;
		jmax = (i < len - 1) ? i : len - 1;

		res[i] = 0;

		j = jmin;
		while(j <= jmax){
			res[i] += in[j] * out[i - j];
			j++;
		}
		i++;
	}
}

int main()
{
	const int size = 10;
	float input[size] = {1,2,3,4.5,5,6,7,8,9,10};
	float output_c[size];
	//float output_asm[10];

	float sub_result[size];
	float corr_result_lib[(size*2) - 1];
	float conv_result_lib[(size*2) - 1];
	float stdev, avg_diff;

	float conv_result_usr[(size*2) - 1];

	kalman_state state;
	state.q = 2;
	state.r = 10;
	state.x = 0;
	state.p = 0;
	state.k = 0;



	Kalmanfilter_C(&input[0], &output_c[0], &state, 10);

	arm_sub_f32 (input, output_c, sub_result, size);
	arm_std_f32 (sub_result, size, &stdev);
	arm_mean_f32 (sub_result, size, &avg_diff);
	arm_correlate_f32(input, size, output_c, size, corr_result_lib);
	arm_conv_f32(input, size, output_c, size, conv_result_lib);
	new_convolution(input, output_c, conv_result_usr, size);

	// printf("Input:\n");
	// printArray(input, size);
	printf("\nOutput:\n");
	printArray(output_c, size);
	// printf("\nDifference:\n");
	// printArray(sub_result, size);
	// printf("\nStandard Dev: %f:\n", stdev);
	// printf("\nMean Difference: %f:\n", avg_diff);
	// printf("\nLib Correlation:\n");
	// printArray(corr_result_lib, (size * 2) - 1);
	printf("\nLib Convolution:\n");
	printArray(conv_result_lib, (size * 2) - 1);
	printf("\nUsr convolution:\n");
	printArray(conv_result_usr, (size * 2) - 1);

	return 0;
}
