#include <iostream>
#include <unordered_map>

#include "list.h"


typedef std::unordered_map<std::string, impl_t> impl_map_t;


void print_usage(const char name[], const impl_map_t& impl) {
	std::cout << "Usage:\n" << name << " [module name]\n\nModules:\n";

	for (const auto& it : impl) {
		std::cout << it.first << "\n";
	}

	std::cout << std::endl;
}


int main(int argc, char* argv[]) {
	impl_map_t impl = NAME_TO_IMPL;

	if (argc != 2) {
		print_usage(argv[0], impl);
		return 1;
	}

	const auto& it = impl.find(argv[1]);

	if (it == impl.end()) {
		print_usage(argv[0], impl);
		return 2;
	}

	it->second();

	return 0;
}
