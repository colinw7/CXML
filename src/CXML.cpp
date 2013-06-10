#include "CXMLI.h"

CXML::
CXML() :
 factory_(),
 tokens_ (),
 debug_  (false)
{
  factory_ = new CXMLFactory;
}

void
CXML::
setFactory(CXMLFactory *factory)
{
  factory_ = factory;
}

void
CXML::
addToken(CXMLToken *token)
{
  tokens_.push_back(token);
}

bool
CXML::
read(const string &filename, CXMLTag **tag)
{
  CXMLParser parser(*this);

  bool flag = parser.read(filename, tag);

  return flag;
}

bool
CXML::
readString(const string &str, CXMLTag **tag)
{
  CXMLParser parser(*this);

  bool flag = parser.readString(str, tag);

  return flag;
}

bool
CXML::
find(CXMLTag *tag, const string &def, string &value)
{
  return tag->find(def, value);
}

CXMLComment *
CXML::
createComment(const string &str) const
{
  return factory_->createComment(str);
}

CXMLTag *
CXML::
createTag(CXMLTag *parent, const string &name, CXMLTag::OptionArray &options) const
{
  return factory_->createTag(parent, name, options);
}

CXMLTagOption *
CXML::
createTagOption(const string &name, const string &value) const
{
  return factory_->createTagOption(name, value);
}

CXMLText *
CXML::
createText(const string &str) const
{
  return factory_->createText(str);
}

//-------

bool
CXML::
write(const string &filename, const CXMLTag *tag)
{
  CFile file(filename);

  if (! file.open(CFileBase::WRITE))
    return false;

  writeTag(&file, tag, "");

  return true;
}

void
CXML::
writeToken(CFile *file, const CXMLToken *token, const string &prefix)
{
  if      (token->isTag()) {
    CXMLTag *child_tag = token->getTag();

    writeTag(file, child_tag, prefix);
  }
  else if (token->isText()) {
    CXMLText *child_text = token->getText();

    writeText(file, child_text, prefix);
  }
  else if (token->isComment()) {
    CXMLComment *comment = token->getComment();

    writeComment(file, comment, prefix);
  }
}

void
CXML::
writeTag(CFile *file, const CXMLTag *tag, const string &prefix)
{
  file->write(CStrUtil::strprintf("%s<%s", prefix.c_str(), tag->getName().c_str()));

  CXMLTag::OptionArray options = tag->getOptions();

  CXMLTag::OptionArray::iterator p1 = options.begin();
  CXMLTag::OptionArray::iterator p2 = options.end  ();

  for ( ; p1 != p2; ++p1)
    file->write(CStrUtil::strprintf(" %s=\"%s\"", (*p1)->getName().c_str(),
                (*p1)->getValue().c_str()));

  CXMLTag::TokenArray children = tag->getChildren();

  if (! children.empty()) {
    file->write(">\n");

    string prefix1 = prefix + "  ";

    for (uint i = 0; i < children.size(); ++i) {
      const CXMLToken *child_token = children[i];

      writeToken(file, child_token, prefix1);
    }

    file->write(CStrUtil::strprintf("%s</%s>\n", prefix.c_str(), tag->getName().c_str()));
  }
  else
    file->write("/>\n");
}

void
CXML::
writeText(CFile *file, const CXMLText *text, const string &)
{
  string str = CXMLNamedCharMgrInst->encodeString(text->getText());

  file->write(CStrUtil::strprintf("%s\n", str.c_str()));
}

void
CXML::
writeComment(CFile *file, const CXMLComment *comment, const string &prefix)
{
  file->write(CStrUtil::strprintf("%s%s\n", prefix.c_str(), comment->getString().c_str()));
}

//-------

CXMLComment *
CXMLFactory::
createComment(const string &str)
{
  return new CXMLComment(str);
}

CXMLTag *
CXMLFactory::
createTag(CXMLTag *parent, const string &name, CXMLTag::OptionArray &options)
{
  return new CXMLTag(parent, name, options);
}

CXMLTagOption *
CXMLFactory::
createTagOption(const string &name, const string &value)
{
  return new CXMLTagOption(name, value);
}

CXMLText *
CXMLFactory::
createText(const string &str)
{
  return new CXMLText(str);
}
