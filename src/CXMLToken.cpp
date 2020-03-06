#include <CXMLToken.h>
#include <CXMLTag.h>
#include <CXMLComment.h>
#include <CXMLText.h>
#include <CXMLExecute.h>
#include <CStrUtil.h>

CXMLToken::
CXMLToken(CXMLTag *ptag, CXMLToken::Type type) :
 ptag_(ptag), type_(type)
{
  if (ptag_)
    ptag_->addChild(this);
}

CXMLToken::
~CXMLToken()
{
}

std::ostream &
operator<<(std::ostream &os, const CXMLToken &token)
{
  token.print(os);

  return os;
}

//------

CXMLCommentToken::
CXMLCommentToken(CXMLTag *ptag, CXMLComment *comment) :
 CXMLToken(ptag, CXML_TOKEN_COMMENT), comment_(comment)
{
}

void
CXMLCommentToken::
print(std::ostream &os) const
{
  int depth = ptag_->getDepth();

  std::string pad = CStrUtil::duplicate("  ", depth);

  os << pad << *comment_;
}

//------

CXMLTagToken::
CXMLTagToken(CXMLTag *ptag, CXMLTag *tag) :
 CXMLToken(ptag, CXML_TOKEN_TAG), tag_(tag)
{
}

void
CXMLTagToken::
print(std::ostream &os) const
{
  int depth = ptag_->getDepth();

  std::string pad = CStrUtil::duplicate("  ", depth);

  os << pad << *tag_;
}

//------

CXMLTextToken::
CXMLTextToken(CXMLTag *ptag, CXMLText *text) :
 CXMLToken(ptag, CXML_TOKEN_TEXT), text_(text)
{
}

void
CXMLTextToken::
print(std::ostream &os) const
{
  int depth = ptag_->getDepth();

  std::string pad = CStrUtil::duplicate("  ", depth);

  os << pad << *text_;
}

//------

CXMLExecuteToken::
CXMLExecuteToken(CXMLTag *ptag, CXMLExecute *exec) :
 CXMLToken(ptag, CXML_TOKEN_EXECUTE), exec_(exec)
{
}

void
CXMLExecuteToken::
print(std::ostream &os) const
{
  int depth = ptag_->getDepth();

  std::string pad = CStrUtil::duplicate("  ", depth);

  os << pad << *exec_;
}
