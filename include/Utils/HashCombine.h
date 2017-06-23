#pragma once

namespace Utils
{
	class HashCombine
	{
	public:
		template <class T>
		static void hash_combine(std::size_t& seed, const T& v)
		{
			seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};
}