#include <CXMLLib.h>

CXMLExecute::
CXMLExecute(const std::string &id) :
 id_(id)
{
}

void
CXMLExecute::
addOption(const std::string &name, const std::string &value)
{
  options_.push_back(Option(name, value));
}

void
CXMLExecute::
print(std::ostream &os) const
{
  os << id_;
}

std::ostream &
operator<<(std::ostream &os, const CXMLExecute &exec)
{
  exec.print(os);

  return os;
}
