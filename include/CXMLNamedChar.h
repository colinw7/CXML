#ifndef CXML_NAMED_CHAR_H
#define CXML_NAMED_CHAR_H

#include <map>

#define CXMLNamedCharMgrInst CXMLNamedCharMgr::getInstance()

struct CXMLNamedChar {
  const char *name;
  uint        value;

  CXMLNamedChar(const char *name, uint value) :
   name(name), value(value) {
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
  static uint          num_named_chars_;

  NameValueMap name_value_map_;
  ValueNameMap value_name_map_;
};

#endif
