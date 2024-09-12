#ifndef URANDOM_H
#define URANDOM_H

#include <random>

class Trandom{
	public:
		typedef uint32_t TrandomNumber;

    static TrandomNumber gen() { return rand(); }

    static TrandomNumber gen(TrandomNumber _min, TrandomNumber _max) {
        return (rand() % (_max - _min + 1)) + _min;
    }

    static TrandomNumber gen(TrandomNumber _max) {
        return (rand() % (_max + 1));
    }
};

#endif
