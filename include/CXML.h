#ifndef CXML_H
#define CXML_H

#include <CXMLTag.h>
#include <map>
#include <set>
#include <memory>

class CXMLComment;
class CXMLTagOption;
class CXMLText;
class CXMLFactory;

class CFile;

class CXML {
 public:
  using TokenList = std::vector<CXMLToken *>;

 public:
  CXML();

  void setDebug(bool debug) { debug_ = debug; }
  bool getDebug() const { return debug_; }

  CXMLFactory *getFactory() const { return factory_.get(); }
  void setFactory(CXMLFactory *factory);

  bool isPreserveSpaceTag(const std::string &name) const {
    return preserveSpaceTags_.find(name) != preserveSpaceTags_.end(); }
  void addPreserveSpaceTag(const std::string &name) { preserveSpaceTags_.insert(name); }

  void addToken(CXMLToken *token);

  const TokenList &getTokens() const { return tokens_; }

  uint getNumTokens() const { return tokens_.size(); }

  CXMLToken *getToken(uint i) const { return tokens_[i]; }

  void clear();

  bool read(const std::string &filename, CXMLTag **tag);

  bool readString(const std::string &str, CXMLTag **tag);

  bool write(const std::string &filename, const CXMLTag *tag);

  bool find(CXMLTag *tag, const std::string &def, std::string &value);

  CXMLComment   *createComment(CXMLTag *tag, const std::string &str) const;
  CXMLTag       *createTag(CXMLTag *parent, const std::string &name,
                           CXMLTag::OptionArray &options) const;
  CXMLTagOption *createTagOption(const std::string &name, const std::string &value) const;
  CXMLText      *createText(CXMLTag *tag, const std::string &str) const;

  void setEntity(const std::string &name, const std::string &value);
  bool getEntity(const std::string &name, std::string &value) const;

 private:
  void writeToken  (CFile *file, const CXMLToken   *token  , const std::string &prefix);
  void writeTag    (CFile *file, const CXMLTag     *tag    , const std::string &prefix);
  void writeText   (CFile *file, const CXMLText    *text   , const std::string &prefix);
  void writeComment(CFile *file, const CXMLComment *comment, const std::string &prefix);

 private:
  using Entities          = std::map<std::string,std::string>;
  using FactoryP          = std::unique_ptr<CXMLFactory>;
  using PreserveSpaceTags = std::set<std::string>;

  FactoryP          factory_;
  TokenList         tokens_;
  Entities          entities_;
  PreserveSpaceTags preserveSpaceTags_;
  bool              debug_ { false };
};

//------

class CXMLFactory {
 public:
  CXMLFactory() { }

  virtual ~CXMLFactory() { }

  virtual CXMLComment   *createComment(CXMLTag *xml, const std::string &str);
  virtual CXMLTag       *createTag(const CXML *xml, CXMLTag *parent, const std::string &name,
                                   CXMLTag::OptionArray &options);
  virtual CXMLTagOption *createTagOption(const std::string &name, const std::string &value);
  virtual CXMLText      *createText(CXMLTag *tag, const std::string &str);
};

#endif
