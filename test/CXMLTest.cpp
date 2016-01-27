#include <CXMLLib.h>
#include <cstring>

int
main(int argc, char **argv)
{
  std::vector<std::string> filenames;

  bool        debug = false;
  bool        dump    = false;
  std::string find    = "";
  bool        single  = false;
  bool        options = true;
  bool        text    = true;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "debug") == 0)
        debug = true;
      else if (strcmp(&argv[i][1], "dump") == 0)
        dump = true;
      else if (strcmp(&argv[i][1], "find") == 0) {
        ++i;

        if (i < argc)
          find = argv[i];
      }
      else if (strcmp(&argv[i][1], "single") == 0)
        single = true;
      else if (strcmp(&argv[i][1], "nooptions") == 0)
        options = false;
      else if (strcmp(&argv[i][1], "notext") == 0)
        text = false;
      else
        std::cerr << "Invalid option: " << argv[i] << std::endl;
    }
    else
      filenames.push_back(argv[i]);
  }

  for (const auto &filename : filenames) {
    CXML xml;

    CXMLTag *tag;

    xml.setDebug(debug);

    if (! xml.read(filename, &tag))
      continue;

    if (dump) {
      std::cout << "-------------" << std::endl;

      std::cout << *tag << std::endl;

      std::cout << "-------------" << std::endl;
    }

    if (find != "") {
      CXMLFindVisitor visitor(tag, find);

      visitor.setSingleMatch(single);

      CXMLFindVisitor::Strings results;

      if (visitor.find(results)) {
        for (const auto &r : results)
          std::cout << r << std::endl;
      }
      else
        std::cout << "Not found" << std::endl;
    }
    else {
      CXMLPrintVisitor visitor(std::cout, tag);

      visitor.setShowOptions(options);
      visitor.setShowText   (text);

      visitor.process();
    }

    if (dump)
      std::cout << "-------------" << std::endl;
  }
}
