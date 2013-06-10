#include "CXMLI.h"

CXMLToken::
CXMLToken(CXMLTag *ptag, CXMLToken::Type type) :
 ptag_(ptag), type_(type)
{
  if (ptag_ != NULL)
    ptag_->addChild(this);
}

CXMLToken::
~CXMLToken()
{
}

ostream &
operator<<(ostream &os, const CXMLToken &token)
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
print(ostream &os) const
{
  int depth = ptag_->getDepth();

  string pad = CStrUtil::duplicate("  ", depth);

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
print(ostream &os) const
{
  int depth = ptag_->getDepth();

  string pad = CStrUtil::duplicate("  ", depth);

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
print(ostream &os) const
{
  int depth = ptag_->getDepth();

  string pad = CStrUtil::duplicate("  ", depth);

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
print(ostream &os) const
{
  int depth = ptag_->getDepth();

  string pad = CStrUtil::duplicate("  ", depth);

  os << pad << *exec_;
}
