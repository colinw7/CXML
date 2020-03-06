#include <CXMLComment.h>

CXMLComment::
CXMLComment(CXMLTag *tag, const std::string &str) :
 tag_(tag), str_(str)
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
