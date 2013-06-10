#include "CXMLI.h"

CXMLText::
CXMLText(const string &text) :
 text_(text)
{
}

void
CXMLText::
print(ostream &os) const
{
  os << text_;
}

ostream &
operator<<(ostream &os, const CXMLText &text)
{
  text.print(os);

  return os;
}
