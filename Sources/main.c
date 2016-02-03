#include <stdio.h>

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

int main()
{
	float input[10] = {1,2,3,4.5,5,6,7,8,9,10};
	float output_c[10];
	float output_asm[10];

	int i;
	int compare;

	kalman_state state;
	state.q = 2;
	state.r = 10;
	state.x = 0;
	state.p = 0;
	state.k = 0;

	Kalmanfilter_asm(&input[0], &output_asm[0], &state, 10);
	
	state.x = 0;
	state.p = 0;
	state.k = 0;
	
	Kalmanfilter_C(&input[0], &output_c[0], &state, 10);

	i = 0;
	compare = 0;

	while(i < 10){
		compare = (output_c[i] == output_asm[i]);
		printf("Entry %i: %i\n", i, compare);
		printf("Entries %i: %f :: %f\n\n", i, output_c[i], output_asm[i]);
		i++;
	}

	printf("END");

	return 0;
}
