#ifndef CXML_TEXT_H
#define CXML_TEXT_H

#include <string>

class CXMLTag;

class CXMLText {
 public:
  CXMLText(CXMLTag *tag, const std::string &text);

  CXMLTag *tag() const { return tag_; }

  const std::string &getText() const { return text_; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLText &text);

 private:
  CXMLTag*    tag_ { nullptr };
  std::string text_;
};

#endif
