#ifndef CYaml_H
#define CYaml_H

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

class CStrParse;

//---

class CYamlStream {
 public:
  CYamlStream() { }

  std::string str() const { return ss_.str(); }

  CYamlStream &operator<<(char c) {
    ss_ << c;

    if (c == '\n')
      pos_ = 0;
    else
      ++pos_;

    return *this;
  }

  CYamlStream &operator<<(const std::string &s) {
    ss_ << s;

    for (auto &c : s) {
      if (c == '\n')
        pos_ = 0;
      else
        ++pos_;
    }

    return *this;
  }

  void setIndent(int i) {
    while (pos_ < i) {
      ss_ << ' ';

      ++pos_;
    }
  }

 private:
  std::stringstream ss_;
  int               pos_ { 0 };
};

//---

class CYaml {
 public:
  CYaml();
 ~CYaml();

  bool parse(const std::string &str);

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  void print(std::ostream &os) const;

 public:
  class ScalarData;
  class Scope;

  using Lines      = std::vector<std::string>;
  using ScopeArray = std::vector<Scope *>;

  class Scalar {
   public:
    enum class Type {
      NONE,
      STRING,
      DATA
    };

   public:
    Scalar() { }

    explicit Scalar(const std::string &str) :
     type_(Type::STRING), str_(str) {
    }

    explicit Scalar(ScalarData *data) :
     type_(Type::DATA), data_(data) {
    }

    Scalar(const Scalar &scalar) {
      type_ = scalar.type_;

      if      (scalar.type_ == Type::DATA)
        data_ = new ScalarData(*scalar.data_);
      else if (scalar.type_ == Type::STRING)
        str_ = scalar.str_;
    }

    Type type() const { return type_; }

    std::string str() const {
      if      (type_ == Type::DATA)
        return data_->str();
      else if (type_ == Type::STRING)
        return str_;
      else
        return "";
    }

    ScalarData *data() const {
      if (type_ == Type::DATA)
        return data_;
      else
        return nullptr;
    }

    Scalar &operator=(const Scalar &scalar) {
      if (type_ == Type::DATA) {
        delete data_;

        data_ = nullptr;
      }

      type_ = scalar.type_;

      if      (scalar.type_ == Type::DATA)
        data_ = new ScalarData(*scalar.data_);
      else if (scalar.type_ == Type::STRING)
        str_ = scalar.str_;

      return *this;
    }

   ~Scalar() {
      if (type_ == Type::DATA)
        delete data_;
    }

    bool isNull() const {
      return (type_ == Type::NONE);
    }

    void print(CYamlStream &os, int depth=0) const {
      if      (type_ == Type::DATA)
        data_->print(os, depth);
      else if (type_ == Type::STRING)
        os << '"' << str_ << '"';
    }

    friend bool operator<(const Scalar &lhs, const Scalar &rhs) {
      if (lhs.type_ != rhs.type_)
        return lhs.type_ < rhs.type_;

      if      (lhs.type_ == Type::STRING)
        return lhs.str_ < rhs.str_;
      else if (lhs.type_ == Type::DATA)
        return lhs.data_ < rhs.data_;

      return false;
    }

   private:
    Type        type_ { Type::NONE };
    std::string str_;
    ScalarData* data_ { nullptr };
  };

  //---

  using NameValue  = std::pair<Scalar,Scalar>;
  using NameValues = std::vector<NameValue>;
  using Sequence   = std::vector<Scalar>;

  class Mapping {
   public:
    Mapping() { }

    void addNameValue(const Scalar &name, const Scalar &value) {
      NameValue nv(name, value);

      int ind = int(names_.size());

      names_.push_back(nv);

      nameValueMap_[name] = ind;
    }

    void setLastNameValue(const Scalar &value) {
      assert(! names_.empty());

      names_.back().second = value;
    }

    bool empty() const { return names_.empty(); }

    auto begin() const { return names_.begin(); }
    auto end  () const { return names_.end  (); }

    auto begin() { return names_.begin(); }
    auto end  () { return names_.end  (); }

   private:
    using NameValue    = std::pair<Scalar,Scalar>;
    using Names        = std::vector<NameValue>;
    using NameValueMap = std::map<Scalar,int>;

    Names        names_;
    NameValueMap nameValueMap_;
  };

  //---

  class ScalarData {
   public:
    enum class Type {
      NONE,
      SEQUENCE,
      MAPPING
    };

    ScalarData() { }

    Type type() const { return type_; }

    void setType(const Type &type) {
      assert(type_ == Type::NONE);

      type_ = type;
    }

    bool isValid() const { return type_ != Type::NONE; }

    bool isBlock() const { return isBlock_; }
    void setIsBlock(bool b) { isBlock_ = b; }

    void addValue(const Scalar &value) {
      assert(type_ == Type::SEQUENCE);

      sequence_.push_back(value);
    }

    void setLastValue(const Scalar &value) {
      assert(type_ == Type::SEQUENCE);

      assert(! sequence_.empty());

      sequence_.back() = value;
    }

    void addNameValue(const Scalar &name, const Scalar &value) {
      assert(type_ == Type::MAPPING);

      mapping_.addNameValue(name, value);
    }

    void setLastNameValue(const Scalar &value) {
      assert(type_ == Type::MAPPING);

      mapping_.setLastNameValue(value);
    }

    const Mapping &mapping() const {
      assert(type_ == Type::MAPPING);

      return mapping_;
    }

    const Sequence &sequence() const {
      assert(type_ == Type::SEQUENCE);

      return sequence_;
    }

    std::string str() const;

    void print(CYamlStream &os, int depth=0) const;

   private:
    Type     type_    { Type::NONE }; // type
    bool     isBlock_ { false };      // in block
    Mapping  mapping_;                // current mapping  (type == Type::MAPPING)
    Sequence sequence_;               // current sequence (type == Type::SEQUENCE)
  };

  //---

  struct Block {
    Lines  lines;
    Scope* child { nullptr };

    Block() {
    }

   ~Block() {
    }
  };

  using Blocks = std::vector<Block *>;

  class Scope {
   public:
    Scope(int indent=0) :
     indent_(indent) {
      data_ = new ScalarData;
    }

   ~Scope() {
      for (auto &b : blocks_)
        delete b;

      delete data_;
    }

    Scope *parent() const { return parent_; }

    int indent() const { return indent_; }

    void addLine(const std::string &line) {
      if (! block_) {
        block_ = new Block();

        blocks_.push_back(block_);
      }

      block_->lines.push_back(line);
    }

    bool hasBlocks() const { return ! blocks_.empty(); }

    int numBlocks() const { return int(blocks_.size()); }

    const Blocks &blocks() const { return blocks_; }
    Blocks &blocks() { return blocks_; }

    Block *block(int i) const { return blocks_[size_t(i)]; }

    void addScope(Scope *scope) {
      if (! block_) {
        block_ = new Block();

        blocks_.push_back(block_);
      }

      block_->child = scope;

      scope->parent_ = this;

      block_ = nullptr;
    }

    ScalarData::Type type() const {
      if (! data_)
        return ScalarData::Type::NONE;

      return data_->type();
    }

    void setType(const ScalarData::Type &type) {
      assert(data_);

      data_->setType(type);
    }

    bool isBlock() const {
      assert(data_);

      return data_->isBlock();
    }

    void setIsBlock(bool b) {
      assert(data_);

      data_->setIsBlock(b);
    }

    void addValue(const Scalar &value) {
      assert(data_);

      data_->addValue(value);
    }

    void setLastValue(const Scalar &value) {
      assert(data_);

      data_->setLastValue(value);
    }

    void addNameValue(const Scalar &name, const Scalar &value) {
      assert(data_);

      data_->addNameValue(name, value);
    }

    void setLastNameValue(const Scalar &value) {
      assert(data_);

      data_->setLastNameValue(value);
    }

    const Mapping &mapping() const {
      assert(data_);

      return data_->mapping();
    }

    const Sequence &sequence() const {
      assert(data_);

      return data_->sequence();
    }

    ScalarData *extractData() {
      ScalarData *data = data_;

      data_ = nullptr;

      return data;
    }

    bool hasData() const {
      return (data_ && data_->isValid());
    }

    Scope(const Scope &) = delete;
    Scope &operator=(const Scope &) = delete;

   private:
    Scope*      parent_ { nullptr };
    int         indent_ { -1 };
    Blocks      blocks_;
    Block*      block_  { nullptr };
    ScalarData* data_   { nullptr };
  };

  //---

 public:
  class Visitor {
   public:
    Visitor() { }

    virtual ~Visitor() { }

    virtual void visitStartSequence(Scope *) { }
    virtual void visitSequence(Scope *scope, const Scalar &value) = 0;
    virtual void visitEndSequence(Scope *) { }

    virtual void visitStartMapping(Scope *) { }
    virtual void visitMapping(Scope *scope, const Scalar &name, const Scalar &value) = 0;
    virtual void visitEndMapping(Scope *) { }
  };

  void visit(Visitor &v) const;

 private:
  void visit(Scope *scope, Visitor &v) const;

  void visit(Scope *scope, ScalarData *data, Visitor &v) const;

 private:
  bool openFile();
  void closeFile();

  bool readLine(std::string &str);
  bool readNextLine(std::string &str, int &indent);

  Scope *loadScope(int &indent);

  void printScopeLines(std::ostream &os, Scope *scope, int depth);

  void printScopeData(CYamlStream &os, Scope *scope, int depth);

  bool processScope(Scope *scope);

  void unreadLine(const std::string &str, int indent);

  void processLines();

  Scalar readScalar(CStrParse &parse);

  std::string readId(CStrParse &parse) const;

  Scalar getId(const std::string &id) const;
  void setId(const std::string &id, const Scalar &value);

 private:
  using IdMap = std::map<std::string,Scalar>;

  struct LineBuffer {
    bool        valid { false };
    std::string str;
    int         indent { 0 };
  };

  std::string filename_;
  FILE*       fp_          { nullptr };
  LineBuffer  lineBuffer_;
  IdMap       idMap_;
  Scope*      scope_       { nullptr };
  bool        debug_       { false };
};

#endif
