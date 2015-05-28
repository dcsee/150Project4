#include <iostream>
using namespace std;

int main(){

	int length = (512 - 64);
	cout << "length is: " << length << endl;
	length = (length + 0x3F) & (~0x3F);
	cout << "the new length is: " << length << endl;
	return 0;

}
