#ifndef CXML_H
#define CXML_H

#include <CAutoPtr.h>
#include <CXMLTag.h>

class CXMLComment;
class CXMLTagOption;
class CXMLText;
class CXMLFactory;

class CFile;

class CXML {
 private:
  typedef std::vector<CXMLToken *> TokenList;

  CAutoPtr<CXMLFactory> factory_;
  TokenList             tokens_;
  bool                  debug_;

 public:
  CXML();

  void setDebug(bool debug) { debug_ = debug; }
  bool getDebug() const { return debug_; }

  void setFactory(CXMLFactory *factory);

  void addToken(CXMLToken *token);

  uint getNumTokens() const { return tokens_.size(); }

  CXMLToken *getToken(uint i) const { return tokens_[i]; }

  bool read(const std::string &filename, CXMLTag **tag);

  bool readString(const std::string &str, CXMLTag **tag);

  bool write(const std::string &filename, const CXMLTag *tag);

  bool find(CXMLTag *tag, const std::string &def, std::string &value);

  CXMLComment   *createComment(const std::string &str) const;
  CXMLTag       *createTag(CXMLTag *parent, const std::string &name,
                           CXMLTag::OptionArray &options) const;
  CXMLTagOption *createTagOption(const std::string &name, const std::string &value) const;
  CXMLText      *createText(const std::string &str) const;

 private:
  void writeToken  (CFile *file, const CXMLToken   *token  , const std::string &prefix);
  void writeTag    (CFile *file, const CXMLTag     *tag    , const std::string &prefix);
  void writeText   (CFile *file, const CXMLText    *text   , const std::string &prefix);
  void writeComment(CFile *file, const CXMLComment *comment, const std::string &prefix);
};

class CXMLFactory {
 public:
  CXMLFactory() { }

  virtual ~CXMLFactory() { }

  virtual CXMLComment   *createComment(const std::string &str);
  virtual CXMLTag       *createTag(CXMLTag *parent, const std::string &name,
                                   CXMLTag::OptionArray &options);
  virtual CXMLTagOption *createTagOption(const std::string &name, const std::string &value);
  virtual CXMLText      *createText(const std::string &str);
};

#endif
