#include <stdio.h>
#include <vector>
#include <iostream>
#include<stdint.h>
using namespace std;

int main(){
	
	unsigned int step = 1;

	int* a = new int[10];
	uint8_t* u = new uint8_t[10];

	vector<int>* v = new vector<int>();
	cout << v->empty() << endl;

//	printf("a[0]: %d\n", a);
//	printf("a[1]: %d\n", &a[1]);
//	printf("a[1] - a[0]: %d\n", (&a[1] - &a[0])*sizeof(int));

/*
	std::vector<int> *v = new std::vector<int>();
	std::vector<int>::iterator itr = v->begin();
	v->insert(itr,9);
	cout << "t1: " << (*v)[0] << endl;*/
/*
	for(int i = 0; i < 10; i++){
		v->push_back(i);
		cout << "t1: " << (*v)[i] << endl;
	}
*/
/*
	std::vector<int>::iterator itr = v->begin();
	for(int i = 0; i < v->size() - 1; i++){
//		cout << "t2: " << (*v)[i] << endl;
		if(i == 4){
			v->insert(itr,9);
			break;
		}
		itr++;
	}
	
	for(int i = 0; i < v->size(); i++){
		cout << "t3: " << (*v)[i] << endl;
	}
*/
/*	
	cout << "a is: " << a << endl;
	cout << a + step << endl;
	cout << *(a + step) << endl;
	cout << a + sizeof(uint8_t) << endl;
	cout << *(a + sizeof(uint8_t)) << endl;
	cout << endl;

	printf("u is: %d\n", u);
	printf("%d\n", u[0]);
	printf("%d\n", u + step);
	printf("%d\n", *(u + step));
	printf("%d\n", (u + sizeof(uint8_t)));
	printf("%d\n", *(u + sizeof(uint8_t)));*/
}
