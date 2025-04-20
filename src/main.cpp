#include "Application.h"
#include <exception>
#include <iostream>
#include <cstdlib> // For EXIT_SUCCESS, EXIT_FAILURE

int main()
{
    Application app;

    try
    {
        if (!app.initialize())
        {
            std::cerr << "Application failed to initialize!" << std::endl;
            return EXIT_FAILURE; // Indicate failure
        }
        app.run(); // Start the main loop
    }
    catch (const std::exception &e)
    {
        // Catch potential exceptions during initialization or run
        std::cerr << "Unhandled Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        // Catch any other unknown exceptions
        std::cerr << "Unknown exception occurred!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS; // Indicate successful execution
}