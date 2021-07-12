#ifndef CXML_PARSER_H
#define CXML_PARSER_H

#include <CXMLTag.h>
#include <CFile.h>
#include <string>
#include <memory>

class CXML;

class CXMLParser {
 public:
  CXMLParser(CXML &xml);
 ~CXMLParser();

  bool read(const std::string &filename, CXMLTag **tag);

  bool readString(const std::string &str, CXMLTag **tag);

  bool readStringOptions(const std::string &str, CXMLTag::OptionArray &options);

  std::string replaceNamedChars(const std::string &value);

 private:
  void readLoop();

  bool isDocType();
  bool readDocType();

  bool isCDATA();
  bool readCDATA();

  bool isENTITY();
  bool readENTITY();

  bool isComment();
  bool readComment();

  bool isExecute();
  bool readExecute();
  bool parseExecute(const std::string &str);

  bool isTag();
  bool readTag();
  bool readTagOptions(CXMLTag::OptionArray &options);

  bool readText(bool skipped=false);

  bool isNameFirstChar(int c);
  bool isNameChar(int c);

  bool matchString(const std::string &str);
  bool skipSpaces();

  int  lookChar();
  int  readChar();

  void fillBuffer();

  void unreadChars(const std::string &str);
  void unreadChar(int c);

  bool parseError(const char *fmt, ...);
  bool parseError(const std::string &str);

  void outputLineChar();

 private:
  CXMLParser(const CXMLParser &rhs);
  CXMLParser &operator=(const CXMLParser &rhs);

 private:
  using FileP = std::unique_ptr<CFile>;

  CXML&            xml_;
  FileP            file_;
  CXMLTag*         root_tag_ { 0 };
  CXMLTag*         tag_ { 0 };
  std::vector<int> buffer_;
  uint             line_num_ { 1 };
  uint             char_num_ { 0 };
  std::string      line_;
};

#endif
