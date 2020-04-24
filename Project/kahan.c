#include<stdio.h>

float kahan(float samples[], int count) {
	float s = samples[0];
	float c = 0.0f;
	for (int i = 1; i < count; i++) {
		float y = samples[i] - c;
		float t = s + y;
		c = (t - s) - y;
		s = t;
	}
	return s;
}

int main() {
	int N = 1000000;
	float samples[N];
	for (int i = 1; i <= N; i++)
		samples[i-1] = 1.0/i;

	printf("%.16g\n", kahan(samples, N));
}