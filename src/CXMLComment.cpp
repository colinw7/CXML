#include "CXMLI.h"

CXMLComment::
CXMLComment(const string &str) :
 str_(str)
{
}

void
CXMLComment::
print(ostream &os) const
{
  os << "<!--" << str_ << "-->";
}

ostream &
operator<<(ostream &os, const CXMLComment &comment)
{
  comment.print(os);

  return os;
}
