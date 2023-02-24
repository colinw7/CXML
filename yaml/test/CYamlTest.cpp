#include <CYaml.h>
#include <iostream>

int
main(int argc, char **argv)
{
  CYaml *yaml = new CYaml;

  std::string filename;

  for (auto i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg(&argv[i][1]);

      std::cerr << "Unhandled option: " << arg << "\n";
    }
    else
      filename = argv[i];
  }

  if (filename == "") {
    std::cerr << "Missing filename\n";
    exit(1);
  }

  yaml->setDebug(true);

  if (! yaml->parse(filename)) {
    std::cerr << "Parse failed\n";
    exit(1);
  }

  yaml->print(std::cerr);

  delete yaml;

  exit(0);
}
