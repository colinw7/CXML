#ifndef CXML_EXECUTE_H
#define CXML_EXECUTE_H

#include <string>
#include <vector>

class CXMLExecute {
 public:
  class Option {
   public:
    Option(const std::string &name, const std::string &value) :
     name_(name), value_(value) {
    }

    const std::string &getName () const { return name_ ; }
    const std::string &getValue() const { return value_; }

   private:
    std::string name_;
    std::string value_;
  };

 public:
  CXMLExecute(const std::string &id);

  const std::string &getId() const { return id_; }

  void addOption(const std::string &name, const std::string &value);

  size_t getNumOptions() const { return options_.size(); }

  const Option &getOption(uint i) const { return options_[i]; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLExecute &exec);

 private:
  using OptionList = std::vector<Option>;

  std::string id_;
  OptionList  options_;
};

#endif
