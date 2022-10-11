#ifndef CXML_NAMED_CHAR_H
#define CXML_NAMED_CHAR_H

#include <string>
#include <map>

#define CXMLNamedCharMgrInst CXMLNamedCharMgr::getInstance()

struct CXMLNamedChar {
  const char   *name;
  unsigned int  value;

  CXMLNamedChar(const char *name1, unsigned int value1) :
   name(name1), value(value1) {
  }
};

//---

class CXMLNamedCharMgr {
 public:
  static CXMLNamedCharMgr *getInstance();

  CXMLNamedCharMgr();

  bool lookup(const std::string &name, CXMLNamedChar **named_char) const;
  bool lookup(int value, CXMLNamedChar **named_char) const;

  std::string encodeString(const std::string &str, bool printable=true) const;

 private:
  using NameValueMap = std::map<std::string,CXMLNamedChar *>;
  using ValueNameMap = std::map<int        ,CXMLNamedChar *>;

 private:
  static CXMLNamedChar named_chars_[];
  static unsigned int  num_named_chars_;

  NameValueMap name_value_map_;
  ValueNameMap value_name_map_;
};

#endif
