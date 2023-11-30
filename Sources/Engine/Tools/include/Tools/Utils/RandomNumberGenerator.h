#include <random>

namespace Tools::Utils
{
	class RandomNumberGenerator
	{
	private:
		static std::mt19937 rng;

	public:
		static void initRNG();
		static int getInt(int min, int max);
		static double getDouble(double min, double max);
	};

	std::mt19937 RandomNumberGenerator::rng;

	void RandomNumberGenerator::initRNG()
	{
		rng.seed(std::random_device()());
	}

	int RandomNumberGenerator::getInt(int min, int max)
	{
		std::uniform_int_distribution<std::mt19937::result_type> udist(min, max);
		return udist(rng);
	}

	double RandomNumberGenerator::getDouble(double min, double max)
	{
		std::uniform_real_distribution<> udist(min, max);
		return udist(rng);
	}
}