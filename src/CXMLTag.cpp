#include <CXMLLib.h>
#include <algorithm>

CXMLTag::
CXMLTag(CXMLTag *parent, const std::string &name, const CXMLTag::OptionArray &options) :
 parent_(parent), name_(name)
{
  if (parent != 0)
    preserveSpace_ = parent->preserveSpace_;

  // TODO: default preserveSpace values for tags
  if (name == "script")
    preserveSpace_ = true;

  uint num_options = options.size();

  for (uint i = 0; i < num_options; ++i) {
    if      (options[i]->getName() == "xml:space") {
      if      (options[i]->getValue() == "preserve")
        preserveSpace_ = true;
      else if (options[i]->getValue() == "default" ||
               options[i]->getValue() == "")
        preserveSpace_ = false;
      else
        std::cerr << "Invalid value " << options[i]->getValue() << "for xml:space" << std::endl;
    }
    else if (options[i]->getName() == "xml:lang")
      ;
    else
      options_.push_back(options[i]);
  }
}

CXMLTag::
~CXMLTag()
{
  for (auto &child : children_)
    delete child;
}

int
CXMLTag::
getDepth() const
{
  int depth = 0;

  const CXMLTag *tag = this;

  while (tag->parent_ != 0) {
    ++depth;

    tag = tag->parent_;
  }

  return depth;
}

std::string
CXMLTag::
getText(bool recurse) const
{
  std::string str;

  for (auto &child : children_) {
    if      (child->isText()) {
      CXMLText *text = child->getText();

      str += text->getText();
    }
    else if (child->isTag()) {
      if (recurse) {
        CXMLTag *tag = child->getTag();

        str += tag->getText();
      }
    }
  }

  return str;
}

void
CXMLTag::
addChild(CXMLToken *child)
{
  children_.push_back(child);
}

bool
CXMLTag::
find(const std::string &def, std::string &value)
{
  CXMLFindVisitor visitor(this, def);

  return visitor.find(value);
}

void
CXMLTag::
print(std::ostream &os) const
{
  int num_options = getNumOptions();

  os << "<" << name_;

  for (int i = 0; i < num_options; ++i)
    os << " " << getOption(i)->getName () << "=\"" << getOption(i)->getValue() << "\"";

  int num_children = getNumChildren();

  if (num_children > 0) {
    os << ">";

    for (int i = 0; i < num_children; ++i)
      os << std::endl << "  " << *getChild(i);

    os << std::endl << "</" << name_ << ">";
  }
  else
    os << "/>";
}

std::ostream &
operator<<(std::ostream &os, const CXMLTag &tag)
{
  tag.print(os);

  return os;
}

//------

CXMLTagOption::
CXMLTagOption(const std::string &name, const std::string &value) :
 name_(name), value_(value)
{
}

void
CXMLTagOption::
print(std::ostream &os) const
{
  os << name_ << "=\"" << value_ << "\"";
}

//------

#if 0
CXMLTagTokens::
CXMLTagTokens()
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
  for (auto &t : tokens_)
    delete t;

  tokens_.clear();
}
#endif
