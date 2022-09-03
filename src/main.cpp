
#include <iostream>
#include <chrono>
#include <thread>
#include <utility>

#include <boost/process.hpp>
#include <boost/program_options.hpp>

#include "config/config.h"

#include "authcontroller.hpp"

namespace po = boost::program_options;
namespace postyle = boost::program_options::command_line_style;

int main(int argc, char* argv[]) {
    po::options_description desc("Options");

    desc.add_options()("config,c",
        po::value<std::string>()->default_value("./server.conf"),
        "configuration file")("http_port,p", po::value<int>(),
            "http server port")(
                "help,h", "Print this help message");

    int unix_style = postyle::unix_style | postyle::short_allow_next;

    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv)
            .options(desc)
            .style(unix_style)
            .run(),
            vm);

        po::notify(vm);

        if (vm.count("help") > 0) {
            std::cout << "USAGE: " << argv[0] << '\n' << desc << '\n';
            return EXIT_SUCCESS;
        }
    }
    catch (po::error& poe) {
        std::cerr << poe.what() << '\n'
            << "USAGE: " << argv[0] << '\n'
            << desc << '\n';
        return EXIT_FAILURE;
    }

    std::string filename = vm["config"].as<std::string>();

    std::cout << "Config filename: " << filename << std::endl;

    /* load configuration from file */
    auto config = Config::Parse(filename);
    if (config == nullptr) {
        return EXIT_FAILURE;
    }

    std::cout << "Example started " << std::endl;


	/* EXAMPLEOFCONFIG  */
    KeycloakConf kconfig = { "spatium-demo",
                            "MIIBIjANBgfurtywfuCRQIDAQAB",
                            "https://127.0.0.1/main",
                            "spatium-mock-speech",
                            {"f580e105-a3dc-4fc3-964c-a86b9b2565df"}
    };

    AuthorizationController controller(kconfig);

    while (true)
    {
        try
        {
            std::cout << "Token: " << controller.getAccessToken() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    std::cout << "Example finished " << std::endl;

    return EXIT_SUCCESS;
}
