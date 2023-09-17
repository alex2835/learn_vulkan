
#include <vulkan/Application.hpp>

int main()
{
	try
	{
		vulkan::Application app;
		app.run();
	}
	catch( std::exception& e )
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}