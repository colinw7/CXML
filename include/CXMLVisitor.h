#ifndef CXML_VISITOR_H
#define CXML_VISITOR_H

#include <accessor.h>
#include <COptVal.h>

class CXMLTag;
class CXMLTagOption;
class CXMLText;

class CXMLVisitor {
 private:
  CXMLTag *tag_;
  bool     brk_;

 public:
  CXMLVisitor(CXMLTag *tag) :
   tag_(tag), brk_(false) {
  }

  virtual ~CXMLVisitor() { }

  bool process();

  virtual bool processTagStart(CXMLTag *) { return true; }
  virtual bool processTagOption(CXMLTagOption *) { return true; }
  virtual bool processTagOptionsEnd(CXMLTag *) { return true; }
  virtual bool processTagChildTag(CXMLTag *) { return true; }
  virtual bool processTagChildText(CXMLText *) { return true; }
  virtual bool processTagEnd(CXMLTag *) { return true; }

  ACCESSOR(Break, bool, brk)

 private:
  bool subProcess(CXMLTag *tag);
  void error(CXMLTag *tag, const std::string &msg) const;

 private:
  CXMLVisitor(const CXMLVisitor &rhs);
  CXMLVisitor &operator=(const CXMLVisitor &rhs);
};

class CXMLPrintVisitor : public CXMLVisitor {
 public:
  CXMLPrintVisitor(std::ostream &os, CXMLTag *tag) :
   CXMLVisitor(tag), os_(os), depth_(0) {
  }

 ~CXMLPrintVisitor() { }

 private:
  bool processTagStart(CXMLTag *tag);
  bool processTagOption(CXMLTagOption *opt);
  bool processTagOptionsEnd(CXMLTag *tag);
  bool processTagChildTag(CXMLTag *child_tag);
  bool processTagChildText(CXMLText *child_text);
  bool processTagEnd(CXMLTag *tag);

  void prefix();

 private:
  std::ostream &os_;
  uint          depth_;
};

class CXMLFindVisitor : public CXMLVisitor {
 public:
  CXMLFindVisitor(CXMLTag *tag, const std::string &def) :
   CXMLVisitor(tag), def_(def), temp_def_tag_(""), temp_def_opt_(""), result_() {
  }

 ~CXMLFindVisitor() { }

  bool find(std::string &value);

 private:
  bool processTagStart(CXMLTag *tag);
  bool processTagOption(CXMLTagOption *opt);
  bool processTagOptionsEnd(CXMLTag *tag);
  bool processTagChildTag(CXMLTag *child_tag);
  bool processTagChildText(CXMLText *child_text);
  bool processTagEnd(CXMLTag *tag);

 private:
  std::string           def_;
  std::string           temp_def_tag_;
  std::string           temp_def_opt_;
  COptValT<std::string> result_;
};

#endif
