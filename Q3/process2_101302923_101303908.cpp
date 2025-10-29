#include <cstdlib>
#include <iostream>
#include <unistd.h>

int main() {

	int counter = 0;
	while (counter >= -500) {
		std::cout << "Cycle number " << abs(counter);
		if (counter % 3 == 0) {
			std::cout << " - " << counter << " is a multiple of 3";
		}
		std::cout << std::endl;
		counter--;
		sleep(1);
	}
	return EXIT_SUCCESS;
}
