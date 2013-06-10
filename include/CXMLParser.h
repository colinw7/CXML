#ifndef CXML_PARSER_H
#define CXML_PARSER_H

#include <CAutoPtr.h>

class CXMLParser {
 public:
  CXMLParser(CXML &xml);
 ~CXMLParser();

  bool read(const std::string &filename, CXMLTag **tag);

  bool readString(const std::string &str, CXMLTag **tag);

  bool readStringOptions(const std::string &str, CXMLTag::OptionArray &options);

 private:
  void readLoop();

  bool isDocType();
  bool readDocType();

  bool isCDATA();
  bool readCDATA();

  bool isComment();
  bool readComment();

  bool isExecute();
  bool readExecute();
  bool parseExecute(const std::string &str);

  bool isTag();
  bool readTag();
  bool readTagOptions(CXMLTag::OptionArray &options);

  std::string replaceNamedChars(const std::string &value);

  bool readText();

  bool isNameFirstChar(int c);
  bool isNameChar(int c);

  bool matchString(const std::string &str);
  void skipSpaces();

  int  lookChar();
  int  readChar();

  void unreadChars(const std::string &str);
  void unreadChar(int c);

  void parseError(const char *fmt, ...);
  void parseError(const std::string &str);

 private:
  CXMLParser(const CXMLParser &rhs);
  CXMLParser &operator=(const CXMLParser &rhs);

 private:
  CXML             &xml_;
  CAutoPtr<CFile>   file_;
  CXMLTag          *root_tag_;
  CXMLTag          *tag_;
  std::vector<int>  buffer_;
  uint              line_num_;
  uint              char_num_;
};

#endif
