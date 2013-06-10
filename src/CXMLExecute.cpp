#include "CXMLI.h"

CXMLExecute::
CXMLExecute(const string &id) :
 id_(id), options_()
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
print(ostream &os) const
{
  os << id_;
}

ostream &
operator<<(ostream &os, const CXMLExecute &exec)
{
  exec.print(os);

  return os;
}
