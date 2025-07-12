#ifndef UTYPETRAITS_H
#define UTYPETRAITS_H

namespace typeTraits{
	template<typename T>
	constexpr bool is_signed()
	{
		return T(-1) < T(0);
	}
}

#endif //UTYPETRAITS_H