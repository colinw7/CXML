#ifndef CXML_TAG_H
#define CXML_TAG_H

#include <vector>

class CXMLToken;

#if 0
class CXMLTagTokens {
 public:
  typedef std::vector<CXMLToken *> TokenArray;

  //typedef TokenArray::const_iterator const_iterator;

 public:
  CXMLTagTokens();
 ~CXMLTagTokens();

  void add(CXMLToken *token);

  uint size() const { return tokens_.size(); }

  const TokenArray &tokens() const { return tokens_; }

  const CXMLToken *operator[](int i) const { return tokens_[i]; }

  void clear();

 private:
  TokenArray tokens_;
};
#endif

//------

class CXMLTagOption {
 public:
  CXMLTagOption(const std::string &name, const std::string &value);

  const std::string &getName () const { return name_ ; }
  const std::string &getValue() const { return value_; }

  void setValue(const std::string &value) { value_ = value; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLTagOption &opt) {
    opt.print(os);

    return os;
  }

 private:
  std::string name_;
  std::string value_;
};

//------

class CXMLTag {
 public:
  typedef std::vector<CXMLTagOption *> OptionArray;
  typedef std::vector<CXMLToken *>     TokenArray;

 public:
  CXMLTag(CXMLTag *parent, const std::string &name, const OptionArray &options);

  virtual ~CXMLTag() { }

  CXMLTag *getParent() const { return parent_; }

  const std::string &getName() const { return name_; }

  //---

  int getNumOptions() const { return options_.size(); }

  const OptionArray &getOptions() const { return options_; }

  //OptionArray::iterator getOptionsBegin() { return options_.begin(); }
  //OptionArray::iterator getOptionsEnd  () { return options_.end  (); }

  const CXMLTagOption *getOption(int i) const { return options_[i]; }

  //---

  int getNumChildren() const { return children_.size(); }

  const TokenArray &getChildren() const { return children_; }

  const CXMLToken *getChild(int i) const { return children_[i]; }

  //---

  bool getPreserveSpace() const { return preserveSpace_; }

  int getDepth() const;

  std::string getText(bool recurse=true) const;

  void addChild(CXMLToken *child);

  void setLocation(int line_num, int char_num) {
    line_num_ = line_num;
    char_num_ = char_num;
  }

  void getLocation(int *line_num, int *char_num) {
    *line_num = line_num_;
    *char_num = char_num_;
  }

  bool find(const std::string &def, std::string &value);

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLTag &tag);

 private:
  CXMLTag(const CXMLTag &rhs);
  CXMLTag &operator=(const CXMLTag &rhs);

 private:
  CXMLTag*    parent_ { 0 };
  std::string name_;
  OptionArray options_;
  TokenArray  children_;
  bool        preserveSpace_ { false };
  int         line_num_ { 1 };
  int         char_num_ { 0 };
};

#endif
