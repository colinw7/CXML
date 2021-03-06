#ifndef CXML_TOKEN_H
#define CXML_TOKEN_H

#include <iostream>

class CXMLTag;
class CXMLComment;
class CXMLText;
class CXMLExecute;

class CXMLToken {
 public:
  enum Type {
    CXML_TOKEN_NONE,
    CXML_TOKEN_COMMENT,
    CXML_TOKEN_TAG,
    CXML_TOKEN_TEXT,
    CXML_TOKEN_EXECUTE
  };

 public:
  CXMLToken(CXMLTag *ptag, Type type);

  virtual ~CXMLToken();

  Type getType() const { return type_; }

  bool isComment() const { return type_ == CXML_TOKEN_COMMENT; }
  bool isTag    () const { return type_ == CXML_TOKEN_TAG    ; }
  bool isText   () const { return type_ == CXML_TOKEN_TEXT   ; }
  bool isExecute() const { return type_ == CXML_TOKEN_EXECUTE; }

  virtual CXMLComment *getComment() const { return 0; }
  virtual CXMLTag     *getTag    () const { return 0; }
  virtual CXMLText    *getText   () const { return 0; }
  virtual CXMLExecute *getExecute() const { return 0; }

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CXMLToken &token);

 private:
  CXMLToken(const CXMLToken &rhs);
  CXMLToken &operator=(const CXMLToken &rhs);

 protected:
  CXMLTag *ptag_ { 0 };
  Type     type_ { CXML_TOKEN_NONE };
};

//------

class CXMLCommentToken : public CXMLToken {
 public:
  CXMLCommentToken(CXMLTag *ptag, CXMLComment *comment);

  CXMLComment *getComment() const { return comment_; }

  void print(std::ostream &os) const;

 private:
  CXMLCommentToken(const CXMLCommentToken &rhs);
  CXMLCommentToken &operator=(const CXMLCommentToken &rhs);

 private:
  CXMLComment *comment_ { 0 };
};

//------

class CXMLTagToken : public CXMLToken {
 public:
  CXMLTagToken(CXMLTag *ptag, CXMLTag *tag);

  CXMLTag *getTag() const { return tag_; }

  void print(std::ostream &os) const;

 private:
  CXMLTagToken(const CXMLTagToken &rhs);
  CXMLTagToken &operator=(const CXMLTagToken &rhs);

 private:
  CXMLTag *tag_ { 0 };
};

//------

class CXMLTextToken : public CXMLToken {
 public:
  CXMLTextToken(CXMLTag *ptag, CXMLText *text);

  CXMLText *getText() const { return text_; }

  void print(std::ostream &os) const;

 private:
  CXMLTextToken(const CXMLTextToken &rhs);
  CXMLTextToken &operator=(const CXMLTextToken &rhs);

 private:
  CXMLText *text_ { 0 };
};

//------

class CXMLExecuteToken : public CXMLToken {
 public:
  CXMLExecuteToken(CXMLTag *ptag, CXMLExecute *exec);

  CXMLExecute *getExecute() const { return exec_; }

  void print(std::ostream &os) const;

 private:
  CXMLExecuteToken(const CXMLExecuteToken &rhs);
  CXMLExecuteToken &operator=(const CXMLExecuteToken &rhs);

 private:
  CXMLExecute *exec_ { 0 };
};

#endif
