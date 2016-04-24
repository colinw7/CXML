#include <CXMLLib.h>

CXMLText::
CXMLText(const std::string &text) :
 text_(text)
{
}

void
CXMLText::
print(std::ostream &os) const
{
  os << CXMLNamedCharMgrInst->encodeString(text_);
}

std::ostream &
operator<<(std::ostream &os, const CXMLText &text)
{
  text.print(os);

  return os;
}
