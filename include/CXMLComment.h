#ifndef CXML_COMMENT_H
#define CXML_COMMENT_H

#include <string>
#include <iostream>

class CXMLTag;

class CXMLComment {
 public:
  CXMLComment(CXMLTag *tag, const std::string &str);

  CXMLTag *tag() const { return tag_; }

  const std::string &getString() const { return str_; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLComment &comment);

 private:
  CXMLTag*    tag_ { nullptr };
  std::string str_;
};

#endif
