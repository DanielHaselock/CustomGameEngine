#ifdef NSHIPPING

#include <catch2/catch_session.hpp>

int main(int argc, char* argv[])
{
	// transfers command line argument to dynamic collection so we can add our own argument easily
	std::vector<char const*> arguments;
	for (int i = 0; i < argc; i++)
	{
		arguments.push_back(argv[i]);
	}

	/************************************\
	*                                    *
	*    uncomment line(s) from below    *
	*    to run all test cases marked    *
	*    by at least 1 of the            *
	*    uncommented tags                *
	*                                    *
	*    By default :                    *
	*    If no test are specified, no    *
	*    test are ran                    *
	*                                    *
	\************************************/

	//arguments.push_back("[all],");
	//arguments.push_back("[vector],");
	//arguments.push_back("[matrice],");

	//arguments.push_back("Degree,");
	//arguments.push_back("Radian,");
	arguments.push_back("Vector2,");
	arguments.push_back("Vector3,");
	arguments.push_back("Vector4,");

	//arguments.push_back("Matrice2,");
	arguments.push_back("Matrice3,");
	arguments.push_back("Matrice4,");

	arguments.push_back("Quaternion,");

	/************************************\
	*                                    *
	*    uncomment line(s) from below    *
	*    to only run specific section    *
	*    from the test selected above    *
	*                                    *
	*    By default :                    *
	*    If no section are specified,    *
	*    all section are ran             *
	*                                    *
	\************************************/

	//arguments.push_back("-c Instantiation");
	//arguments.push_back("-c Accessor");
	//arguments.push_back("-c Comparator");
	//arguments.push_back("-c Constant");
	//arguments.push_back("-c Arithmetic");
	//arguments.push_back("-c Transformation");
	//arguments.push_back("-c Functionality");
	//arguments.push_back("-c Extra");
	//arguments.push_back("-c Debug");
	//arguments.push_back("-c Conversion");

	/************************************\
	*                                    *
	*    add more catch command line     *
	*    argument here                   *
	*                                    *
	\************************************/
	//arguments.push_back("-o test file.txt"); // catch2 will output the test result to "test file.txt" instead of the console
	/*
	std::string argument = std::string("-o ") + __DATE__ + ' ' + __TIME__ + ".txt";
	std::replace(argument.begin(), argument.end(), ':', '.');
	arguments.push_back(argument.c_str()); // catch2 will output the test result to a file name after the current date instead of the console
	*/

	// running catch test with command line argument + custom argument from main
	return Catch::Session().run((int)arguments.size(), &arguments[0]);
}

#endif

#ifdef SHIPPING
int main()
{
	return 0;
}
#endif
