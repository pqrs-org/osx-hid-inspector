#include <cxxopts.hpp>

void output_all_properties_json(void);
void output_usages(void);

int main(int argc, char* argv[]) {
  cxxopts::Options options("hid-inspector", "List hid devices");
  options.add_options()("help", "Print help.");

  options.add_options("Output format")                                        //
      ("output-all-properties-json", "Output all properties in json format.") //
      ("output-usages", "Output usage pages and usages.");                    //

  try {
    auto parse_result = options.parse(argc, argv);

    if (parse_result.count("output-all-properties-json")) {
      output_all_properties_json();

    } else if (parse_result.count("output-usages")) {
      output_usages();

    } else {
      std::cout << options.help() << std::endl;
      std::cout << "Examples:" << std::endl;
      std::cout << "  hid-inspector --output-all-properties-json" << std::endl;
      std::cout << "  hid-inspector --output-usages" << std::endl;
      std::cout << std::endl;
    }

  } catch (const cxxopts::OptionException& e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
