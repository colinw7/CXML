#include "CXMLI.h"

CXMLTag::
CXMLTag(CXMLTag *parent, const string &name, const CXMLTag::OptionArray &options) :
 parent_(parent), name_(name), options_(), children_(), preserveSpace_(false),
 line_num_(0), char_num_(0)
{
  if (parent != NULL)
    preserveSpace_ = parent->preserveSpace_;

  uint num_options = options.size();

  for (uint i = 0; i < num_options; ++i) {
    if      (options[i]->getName() == "xml:space") {
      if      (options[i]->getValue() == "preserve")
        preserveSpace_ = true;
      else if (options[i]->getValue() == "default" ||
               options[i]->getValue() == "")
        preserveSpace_ = false;
      else
        cerr << "Invalid value " << options[i]->getValue() <<
                "for xml:space" << endl;
    }
    else if (options[i]->getName() == "xml:lang")
      ;
    else
      options_.push_back(options[i]);
  }
}

CXMLTag::TokenArray
CXMLTag::
getChildren() const
{
  TokenArray children;

  CXMLTagTokens::const_iterator pc1, pc2;

  for (pc1 = children_.begin(), pc2 = children_.end(); pc1 != pc2; ++pc1)
    children.push_back(*pc1);

  return children;
}

int
CXMLTag::
getDepth() const
{
  int depth = 0;

  const CXMLTag *tag = this;

  while (tag->parent_ != NULL) {
    ++depth;

    tag = tag->parent_;
  }

  return depth;
}

string
CXMLTag::
getText() const
{
  string str;

  uint num_children = getNumChildren();

  for (uint i = 0; i < num_children; ++i) {
    const CXMLToken *token = getChild(i);

    if      (token->isText()) {
      CXMLText *text = token->getText();

      str += text->getText();
    }
    else if (token->isTag()) {
      CXMLTag *tag = token->getTag();

      str += tag->getText();
    }
  }

  return str;
}

void
CXMLTag::
addChild(CXMLToken *child)
{
  children_.add(child);
}

bool
CXMLTag::
find(const string &def, string &value)
{
  CXMLFindVisitor visitor(this, def);

  return visitor.find(value);
}

void
CXMLTag::
print(ostream &os) const
{
  int num_options = getNumOptions();

  os << "<" << name_;

  for (int i = 0; i < num_options; ++i)
    os << " " << getOption(i)->getName () <<
          "=\"" << getOption(i)->getValue() << "\"";

  int num_children = getNumChildren();

  if (num_children > 0) {
    os << ">";

    for (int i = 0; i < num_children; ++i)
      os << endl << "  " << *getChild(i);

    os << endl << "</" << name_ << ">";
  }
  else
    os << "/>";
}

ostream &
operator<<(ostream &os, const CXMLTag &tag)
{
  tag.print(os);

  return os;
}

//------

CXMLTagOption::
CXMLTagOption(const string &name, const string &value) :
 name_(name), value_(value)
{
}

void
CXMLTagOption::
print(ostream &os) const
{
  os << name_ << "=\"" << value_ << "\"";
}

//------

CXMLTagTokens::
CXMLTagTokens() :
 tokens_()
{
}

CXMLTagTokens::
~CXMLTagTokens()
{
  clear();
}

void
CXMLTagTokens::
add(CXMLToken *token)
{
  tokens_.push_back(token);
}

void
CXMLTagTokens::
clear()
{
  std::for_each(tokens_.begin(), tokens_.end(), CDeletePointer());

  tokens_.clear();
}
