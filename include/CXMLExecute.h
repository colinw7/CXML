#ifndef CXML_EXECUTE_H
#define CXML_EXECUTE_H

class CXMLExecute {
 public:
  class Option {
   private:
    std::string name_;
    std::string value_;

   public:
    Option(const std::string &name, const std::string &value) :
     name_(name), value_(value) {
    }

    const std::string &getName () const { return name_ ; }
    const std::string &getValue() const { return value_; }
  };

 private:
  typedef std::vector<Option> OptionList;

  std::string id_;
  OptionList  options_;

 public:
  CXMLExecute(const std::string &id);

  const std::string &getId() const { return id_; }

  void addOption(const std::string &name, const std::string &value);

  uint getNumOptions() const { return options_.size(); }

  const Option &getOption(uint i) const { return options_[i]; }

  void print(std::ostream &os) const;

  friend std::ostream &operator<<(std::ostream &os, const CXMLExecute &exec);
};

#endif
