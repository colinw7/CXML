#include <CXMLLib.h>

CXMLComment::
CXMLComment(const std::string &str) :
 str_(str)
{
}

void
CXMLComment::
print(std::ostream &os) const
{
  os << "<!--" << str_ << "-->";
}

std::ostream &
operator<<(std::ostream &os, const CXMLComment &comment)
{
  comment.print(os);

  return os;
}
