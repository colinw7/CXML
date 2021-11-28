#include <CXMLVisitor.h>
#include <CXMLTag.h>
#include <CXMLToken.h>
#include <CXMLText.h>
#include <CXMLNamedChar.h>
#include <CStrUtil.h>
#include <cassert>

CXMLVisitor::
CXMLVisitor(CXMLTag *tag) :
 tag_(tag)
{
}

bool
CXMLVisitor::
process()
{
  if (! subProcess(tag_)) {
    error(tag_, "Failed to process tag " + tag_->getName());
    return false;
  }

  return true;
}

bool
CXMLVisitor::
subProcess(CXMLTag *tag)
{
  processTagStart(tag);

  if (getBreak())
    return true;

  //------

  for (const auto &o : tag->getOptions()) {
    if (! processTagOption(o))
      error(tag, "Unknown option " + o->getName());

     if (getBreak())
       return true;
  }

  processTagOptionsEnd(tag);

  if (getBreak())
    return true;

  //------

  auto children = tag->getChildren();

  for (uint i = 0; i < children.size(); ++i) {
    const auto *childToken = children[i];

    if      (childToken->isTag()) {
      auto *childTag = childToken->getTag();

      if (! processTagChildTag(childTag))
        error(childTag, "Unknown child tag " + childTag->getName());

      if (getDepthFirst()) {
        if (! subProcess(childTag))
          error(childTag, "Failed to process child tag " + childTag->getName());
      }
    }
    else if (childToken->isText()) {
      auto *text = childToken->getText();

      (void) processTagChildText(text);
    }

    if (getBreak())
      return true;
  }

  //------

  if (! getDepthFirst()) {
    for (uint i = 0; i < children.size(); ++i) {
      const auto *childToken = children[i];

      if (childToken->isTag()) {
        auto *childTag = childToken->getTag();

        if (! subProcess(childTag))
          error(childTag, "Failed to process child tag " + childTag->getName());
      }

      if (getBreak())
        return true;
    }
  }

  //------

  processTagEnd(tag);

  return true;
}

void
CXMLVisitor::
error(CXMLTag *tag, const std::string &msg) const
{
  int line_num, char_num;

  tag->getLocation(&line_num, &char_num);

  std::cerr << "Tag<" << tag->getName() << "> " << line_num << ":" << char_num << ") " <<
               msg << "\n";
}

//-------------------

CXMLPrintVisitor::
CXMLPrintVisitor(std::ostream &os, CXMLTag *tag) :
 CXMLVisitor(tag), os_(os)
{
}

bool
CXMLPrintVisitor::
processTagStart(CXMLTag *tag)
{
  prefix();

  os_ << "<" << tag->getName();

  return true;
}

bool
CXMLPrintVisitor::
processTagOption(CXMLTagOption *opt)
{
  if (isShowOptions())
    os_ << " " << opt->getName() << "=\"" << opt->getValue() << "\"";

  return true;
}

bool
CXMLPrintVisitor::
processTagOptionsEnd(CXMLTag *)
{
  os_ << ">\n";

  ++depth_;

  return true;
}

bool
CXMLPrintVisitor::
processTagChildTag(CXMLTag *)
{
  return true;
}

bool
CXMLPrintVisitor::
processTagChildText(CXMLText *child_text)
{
  if (isShowText()) {
    prefix();

    std::string text = CXMLNamedCharMgrInst->encodeString(child_text->getText());

    os_ << text << "\n";
  }

  return true;
}

bool
CXMLPrintVisitor::
processTagEnd(CXMLTag *tag)
{
  if (depth_ > 0)
    --depth_;

  prefix();

  os_ << "</" << tag->getName() << ">\n";

  return true;
}

void
CXMLPrintVisitor::
prefix()
{
  for (uint i = 0; i < depth_; ++i)
    os_ << "  ";
}

//-------------------

CXMLFindVisitor::
CXMLFindVisitor(CXMLTag *tag, const std::string &match) :
 CXMLVisitor(tag), match_(match)
{
  initMatch();
}

void
CXMLFindVisitor::
setMatch(const std::string &s)
{
  match_ = s;

  initMatch();
}

void
CXMLFindVisitor::
initMatch()
{
  matchFields_ .clear();
  matchOptions_.clear();

  matchHasOption_ = false;

  CStrUtil::addFields(match_, matchFields_, "/");

  int n = matchFields_.size();

  if (n > 0) {
    const std::string &lf = matchFields_[n - 1];

    int l = lf.length();

    if (l > 0 && lf[0] == '<' && lf[l - 1] == '>') {
      matchHasOption_ = true;

      std::string matchOption = matchFields_.back().substr(1, l - 2);

      matchFields_.pop_back();

      CStrUtil::addFields(matchOption, matchOptions_, ",");
    }
  }
}

bool
CXMLFindVisitor::
find(std::string &value)
{
  Strings values;

  if (! find(values))
    return false;

  value = values[0];

  return true;
}

bool
CXMLFindVisitor::
find(Strings &values)
{
  process();

  if (results_.empty())
    return false;

  values = results_;

  return true;
}

bool
CXMLFindVisitor::
processTagStart(CXMLTag *tag)
{
  tagFields_.push_back(tag->getName());

  if (matchTag()) {
    std::string tagFieldsStr = tagFields_[0];

    for (uint i = 1; i < tagFields_.size(); ++i)
      tagFieldsStr += "/" + tagFields_[i];

    results_.push_back(tagFieldsStr);

    if (isSingleMatch())
      setBreak(true);
  }

  return true;
}

bool
CXMLFindVisitor::
processTagOption(CXMLTagOption *opt)
{
  if (matchOption(opt->getName())) {
    results_.push_back(opt->getValue());

    if (isSingleMatch())
      setBreak(true);
  }

  return true;
}

bool
CXMLFindVisitor::
processTagOptionsEnd(CXMLTag *)
{
  return true;
}

bool
CXMLFindVisitor::
processTagChildTag(CXMLTag *)
{
  return true;
}

bool
CXMLFindVisitor::
processTagChildText(CXMLText *)
{
  return true;
}

bool
CXMLFindVisitor::
processTagEnd(CXMLTag *)
{
  assert(! tagFields_.empty());

  tagFields_.pop_back();

  return true;
}

bool
CXMLFindVisitor::
matchTag() const
{
  if (matchHasOption_)
    return false;

  matchTagFields();

  return true;
}

bool
CXMLFindVisitor::
matchOption(const std::string &optionStr) const
{
  if (! matchHasOption_)
    return false;

  bool match = false;

  int no = matchOptions_.size();

  for (int i = 0; i < no; ++i) {
    if (matchOptions_[i] == "*" || matchOptions_[i] == optionStr) {
      match = true;
      break;
    }
  }

  if (! match)
    return false;

  return matchTagFields();
}

bool
CXMLFindVisitor::
matchTagFields() const
{
  int nt = tagFields_  .size();
  int nm = matchFields_.size();

  int j = 0;

  for (int i = 0; i < nt; ++i) {
    if (j >= nm)
      return false;

    if (matchFields_[j] == "...") {
      if (i < nt - 1)
        continue;

      ++j;

      if (j >= nm)
        return true;
    }

    const std::string &matchField = matchFields_[j++];

    if (matchField == "*")
      continue;

    if (matchField != tagFields_[i])
      return false;
  }

  return true;
}
