#pragma once
//用于测试 math类的性能对比
#include "geometry.h"
#include <vector> 
#include <iostream> 
#include <iostream>
#include <time.h>
#include <windows.h>
using namespace std;

//clock_t  start;
//clock_t  finish;
//float duration;
int TestMathmain() {


	matrix_t<4, 4, float> A = matrix_t<4, 4, float>();
	matrix_t<4, 4, float> B = matrix_t<4, 4, float>();
	
	start = clock();
	matrix_t<4, 4, float> C;
	for (int i = 0; i < 100000; i++)
	{
		//C = A * B; // 0.613  0.622  //0.554
		//multiply(C, A, B); //0.59   //0.33-0.31
		//
		(C, A, B); //0.40
		matrix_mul(C, A, B); //0.40
	}

	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("pos tramsform----- %f seconds\n", duration);

	return 0;
}

