#include <CXMLLib.h>
#include <CStrUtil.h>

CXML::
CXML()
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

void
CXML::
clear()
{
  tokens_.clear();
}

bool
CXML::
read(const std::string &filename, CXMLTag **tag)
{
  CXMLParser parser(*this);

  bool flag = parser.read(filename, tag);

  return flag;
}

bool
CXML::
readString(const std::string &str, CXMLTag **tag)
{
  CXMLParser parser(*this);

  bool flag = parser.readString(str, tag);

  return flag;
}

bool
CXML::
find(CXMLTag *tag, const std::string &def, std::string &value)
{
  return tag->find(def, value);
}

CXMLComment *
CXML::
createComment(const std::string &str) const
{
  return factory_->createComment(str);
}

CXMLTag *
CXML::
createTag(CXMLTag *parent, const std::string &name, CXMLTag::OptionArray &options) const
{
  return factory_->createTag(parent, name, options);
}

CXMLTagOption *
CXML::
createTagOption(const std::string &name, const std::string &value) const
{
  return factory_->createTagOption(name, value);
}

CXMLText *
CXML::
createText(const std::string &str) const
{
  return factory_->createText(str);
}

//-------

bool
CXML::
write(const std::string &filename, const CXMLTag *tag)
{
  CFile file(filename);

  if (! file.open(CFileBase::Mode::WRITE))
    return false;

  writeTag(&file, tag, "");

  return true;
}

void
CXML::
writeToken(CFile *file, const CXMLToken *token, const std::string &prefix)
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
writeTag(CFile *file, const CXMLTag *tag, const std::string &prefix)
{
  file->write(CStrUtil::strprintf("%s<%s", prefix.c_str(), tag->getName().c_str()));

  for (const auto &o : tag->getOptions())
    file->write(CStrUtil::strprintf(" %s=\"%s\"", o->getName().c_str(), o->getValue().c_str()));

  CXMLTag::TokenArray children = tag->getChildren();

  if (! children.empty()) {
    file->write(">\n");

    std::string prefix1 = prefix + "  ";

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
writeText(CFile *file, const CXMLText *text, const std::string &)
{
  std::string str = CXMLNamedCharMgrInst->encodeString(text->getText());

  file->write(CStrUtil::strprintf("%s\n", str.c_str()));
}

void
CXML::
writeComment(CFile *file, const CXMLComment *comment, const std::string &prefix)
{
  file->write(CStrUtil::strprintf("%s%s\n", prefix.c_str(), comment->getString().c_str()));
}

//---

void
CXML::
setEntity(const std::string &name, const std::string &value)
{
  entities_[name] = value;
}

bool
CXML::
getEntity(const std::string &name, std::string &value) const
{
  auto p = entities_.find(name);

  if (p == entities_.end())
    return false;

  value = p->second;

  return true;
}

//-------

CXMLComment *
CXMLFactory::
createComment(const std::string &str)
{
  return new CXMLComment(str);
}

CXMLTag *
CXMLFactory::
createTag(CXMLTag *parent, const std::string &name, CXMLTag::OptionArray &options)
{
  return new CXMLTag(parent, name, options);
}

CXMLTagOption *
CXMLFactory::
createTagOption(const std::string &name, const std::string &value)
{
  return new CXMLTagOption(name, value);
}

CXMLText *
CXMLFactory::
createText(const std::string &str)
{
  return new CXMLText(str);
}
