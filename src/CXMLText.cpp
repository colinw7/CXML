#include <CXMLText.h>
#include <CXMLNamedChar.h>

CXMLText::
CXMLText(CXMLTag *tag, const std::string &text) :
 tag_(tag), text_(text)
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
