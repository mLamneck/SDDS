#ifndef URANDOM_H
#define URANDOM_H

//#include <random>
#include <uPlatform.h>

class Trandom{
	public:
		typedef int32_t TrandomNumber;

    static TrandomNumber gen() { return rand(); }

	template <typename T>
    static dtypes::float32 gen(T _min, T _max) {
        float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return _min + random * (_max - _min);
    }

    static TrandomNumber gen(TrandomNumber _min, TrandomNumber _max) {
        return (rand() % (_max - _min + 1)) + _min;
    }

    static TrandomNumber gen(TrandomNumber _max) {
        return (rand() % (_max + 1));
    }
};

#endif
