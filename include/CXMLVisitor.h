#ifndef CXML_VISITOR_H
#define CXML_VISITOR_H

#include <accessor.h>
#include <vector>
#include <string>

class CXMLTag;
class CXMLTagOption;
class CXMLText;

class CXMLVisitor {
 public:
  CXMLVisitor(CXMLTag *tag);

  virtual ~CXMLVisitor() { }

  bool process();

  virtual bool processTagStart(CXMLTag *) { return true; }
  virtual bool processTagOption(CXMLTagOption *) { return true; }
  virtual bool processTagOptionsEnd(CXMLTag *) { return true; }
  virtual bool processTagChildTag(CXMLTag *) { return true; }
  virtual bool processTagChildText(CXMLText *) { return true; }
  virtual bool processTagEnd(CXMLTag *) { return true; }

  ACCESSOR(Break     , bool, brk)
  ACCESSOR(DepthFirst, bool, depthFirst)

 private:
  bool subProcess(CXMLTag *tag);
  void error(CXMLTag *tag, const std::string &msg) const;

 private:
  CXMLVisitor(const CXMLVisitor &rhs);
  CXMLVisitor &operator=(const CXMLVisitor &rhs);

 private:
  CXMLTag *tag_        { nullptr };
  bool     brk_        { false };
  bool     depthFirst_ { true };
};

//------

class CXMLPrintVisitor : public CXMLVisitor {
 public:
  CXMLPrintVisitor(std::ostream &os, CXMLTag *tag);

  virtual ~CXMLPrintVisitor() { }

  bool isShowOptions() const { return showOptions_; }
  void setShowOptions(bool b) { showOptions_ = b; }

  bool isShowText() const { return showText_; }
  void setShowText(bool b) { showText_ = b; }

 private:
  bool processTagStart(CXMLTag *tag) override;
  bool processTagOption(CXMLTagOption *opt) override;
  bool processTagOptionsEnd(CXMLTag *tag) override;
  bool processTagChildTag(CXMLTag *child_tag) override;
  bool processTagChildText(CXMLText *child_text) override;
  bool processTagEnd(CXMLTag *tag) override;

  void prefix();

 private:
  std::ostream &os_;
  uint          depth_       { 0 };
  bool          showOptions_ { true };
  bool          showText_    { true };
};

//------

class CXMLFindVisitor : public CXMLVisitor {
 public:
  using Strings = std::vector<std::string>;

 public:
  CXMLFindVisitor(CXMLTag *tag, const std::string &match);

  virtual ~CXMLFindVisitor() { }

  const std::string &match() const { return match_; }
  void setMatch(const std::string &s);

  bool isSingleMatch() const { return singleMatch_; }
  void setSingleMatch(bool b) { singleMatch_ = b; }

  bool find(std::string &value);
  bool find(Strings &values);

 private:
  void initMatch();

  bool processTagStart(CXMLTag *tag) override;
  bool processTagOption(CXMLTagOption *opt) override;
  bool processTagOptionsEnd(CXMLTag *tag) override;
  bool processTagChildTag(CXMLTag *child_tag) override;
  bool processTagChildText(CXMLText *child_text) override;
  bool processTagEnd(CXMLTag *tag) override;

  bool matchTag() const;
  bool matchOption(const std::string &optionStr) const;
  bool matchTagFields() const;

 private:
  std::string match_;
  Strings     matchFields_;
  Strings     matchOptions_;
  bool        matchHasOption_ { false };
  bool        singleMatch_    { false };
  Strings     tagFields_;
  Strings     results_;
};

#endif
