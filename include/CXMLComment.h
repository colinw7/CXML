#ifndef CXML_COMMENT_H
#define CXML_COMMENT_H

class CXMLComment {
 public:
  CXMLComment(const std::string &str);

  std::string getString() const { return str_; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLComment &comment);

 private:
  std::string str_;
};

#endif
