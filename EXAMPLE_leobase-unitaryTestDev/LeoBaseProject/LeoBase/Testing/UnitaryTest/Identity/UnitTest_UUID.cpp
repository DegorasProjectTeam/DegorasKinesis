

// Basic tests.
M_DECLARE_UNIT_TEST(ConsoleConfig, BasicTest1)
M_DECLARE_UNIT_TEST(ConsoleConfig, BasicTest2)

// MOVE THIS TO OTHER TEST
M_DECLARE_UNIT_TEST(ConsoleRedirect, BasicTest1)

// Implementations.

M_DEFINE_UNIT_TEST(ConsoleConfig, BasicTest1)
{
    std::cout << "Start test..." << std::endl;

    // Configure the console.
    zmqutils::utils::ConsoleConfig& console_cfg = zmqutils::utils::ConsoleConfig::getInstance();
    console_cfg.configureConsole(true, true, false);

    // Wait for closing as an infinite loop until ctrl-c.
    console_cfg.waitForClose();

    std::cout << "End test..." << std::endl;

    // Restore the console.
    console_cfg.restoreConsole();
}

M_DEFINE_UNIT_TEST(ConsoleConfig, BasicTest2)
{
    std::cout << "Start test..." << std::endl;

    // Configure the console.
    zmqutils::utils::ConsoleConfig& console_cfg = zmqutils::utils::ConsoleConfig::getInstance();
    console_cfg.configureConsole(true, false, true);

    // Wait for closing as an infinite loop until ctrl-c.
    console_cfg.waitForClose();

    std::cout << "End test..." << std::endl;

    // Restore the console.
    console_cfg.restoreConsole();
}

M_DEFINE_UNIT_TEST(ConsoleRedirect, BasicTest1)
{

}

int main()
{

    std::cout << "Start test..." << std::endl;

    // Configure the console.
    zmqutils::utils::ConsoleConfig& console_cfg = zmqutils::utils::ConsoleConfig::getInstance();
    console_cfg.configureConsole(false, false, true);

    zmqutils::utils::DebugConsole console1("Debug Console 1");
    zmqutils::utils::DebugConsole console2("Debug Console 2");

    if (console1.startProcess())
    {
       console1.sendString("Hello, Debug Console 1!");
       console1.sendString("This is a test message.");
    }

    if (console2.startProcess())
    {
       console2.sendString("Hello, Debug Console 2!");
       console2.sendString("Another test message.");
    }

    std::cout << "Press Enter to close all debug consoles.\n";
    std::cin.get();

    console1.closeProcess();
    console2.closeProcess();


    std::cout << "End test..." << std::endl;

    // Close the console (cleanup)
    //consoleProcess.closeProcess();


    // Restore the console.
    //console_cfg.restoreConsole();

    // ENABLE THIS TEST ONLY IF NECESSARY

    // Start of the session.
   // M_START_UNIT_TEST_SESSION("LibZMQUtils ConsoleConfig Session")

    // Register the tests.
    //M_REGISTER_UNIT_TEST(ConsoleConfig, BasicTest1)
    //M_REGISTER_UNIT_TEST(ConsoleConfig, BasicTest2)
   // M_REGISTER_UNIT_TEST(ConsoleRedirect, BasicTest1)

    // Run the unit tests.
  // M_RUN_UNIT_TESTS()
}
