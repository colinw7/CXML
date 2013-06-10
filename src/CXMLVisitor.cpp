#include <CXMLVisitor.h>
#include <CXMLTag.h>
#include <CXMLToken.h>
#include <CXMLText.h>

using std::string;
using std::cerr;
using std::endl;

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

  CXMLTag::OptionArray options = tag->getOptions();

  CXMLTag::OptionArray::iterator p1 = options.begin();
  CXMLTag::OptionArray::iterator p2 = options.end  ();

  for ( ; p1 != p2; ++p1) {
    if (! processTagOption(*p1))
      error(tag, "Unknown option " + (*p1)->getName());

     if (getBreak())
       return true;
  }

  processTagOptionsEnd(tag);

  if (getBreak())
    return true;

  //------

  CXMLTag::TokenArray children = tag->getChildren();

  for (uint i = 0; i < children.size(); ++i) {
    const CXMLToken *child_token = children[i];

    if      (child_token->isTag()) {
      CXMLTag *child_tag = child_token->getTag();

      if (! processTagChildTag(child_tag))
        error(child_tag, "Unknown child tag " + child_tag->getName());
    }
    else if (child_token->isText()) {
      CXMLText *text = child_token->getText();

      (void) processTagChildText(text);
    }

    if (getBreak())
      return true;
  }

  //------

  for (uint i = 0; i < children.size(); ++i) {
    const CXMLToken *child_token = children[i];

    if (child_token->isTag()) {
      CXMLTag *child_tag = child_token->getTag();

      if (! subProcess(child_tag))
        error(child_tag, "Failed to process child tag " + child_tag->getName());
    }

    if (getBreak())
      return true;
  }

  //------

  processTagEnd(tag);

  return true;
}

void
CXMLVisitor::
error(CXMLTag *tag, const string &msg) const
{
  int line_num, char_num;

  tag->getLocation(&line_num, &char_num);

  cerr << "Tag<" << tag->getName() << "> " <<
          line_num << ":" << char_num << ") " <<
          msg << endl;
}

//-------------------

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
  os_ << " " << opt->getName() << "=\"" << opt->getValue() << "\"";

  return true;
}

bool
CXMLPrintVisitor::
processTagOptionsEnd(CXMLTag *)
{
  os_ << ">" << endl;

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
  prefix();

  os_ << child_text->getText() << endl;

  return true;
}

bool
CXMLPrintVisitor::
processTagEnd(CXMLTag *tag)
{
  if (depth_ > 0)
    --depth_;

  prefix();

  os_ << "</" << tag->getName() << ">" << endl;

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

bool
CXMLFindVisitor::
find(string &value)
{
  process();

  if (! result_.getValid())
    return false;

  value = result_.getValue();

  return true;
}

bool
CXMLFindVisitor::
processTagStart(CXMLTag *tag)
{
  if (! temp_def_tag_.empty())
    temp_def_tag_ += "/";

  temp_def_tag_ += tag->getName();

  if (temp_def_tag_ == def_) {
    result_ = "??";
    setBreak(true);
  }

  return true;
}

bool
CXMLFindVisitor::
processTagOption(CXMLTagOption *opt)
{
  temp_def_opt_ = temp_def_tag_ + "<" + opt->getName() + ">";

  if (temp_def_opt_ == def_) {
    result_ = opt->getValue();
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
  return true;
}
