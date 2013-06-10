#ifndef CXML_TOKEN_H
#define CXML_TOKEN_H

class CXMLComment;
class CXMLText;
class CXMLExecute;

class CXMLToken {
 public:
   enum Type {
     CXML_TOKEN_COMMENT,
     CXML_TOKEN_TAG,
     CXML_TOKEN_TEXT,
     CXML_TOKEN_EXECUTE
   };

 protected:
  CXMLTag *ptag_;
  Type     type_;

 public:
  CXMLToken(CXMLTag *ptag, Type type);

  virtual ~CXMLToken();

  Type getType() const { return type_; }

  bool isComment() const { return type_ == CXML_TOKEN_COMMENT; }
  bool isTag    () const { return type_ == CXML_TOKEN_TAG    ; }
  bool isText   () const { return type_ == CXML_TOKEN_TEXT   ; }
  bool isExecute() const { return type_ == CXML_TOKEN_EXECUTE; }

  virtual CXMLComment *getComment() const { return NULL; }
  virtual CXMLTag     *getTag    () const { return NULL; }
  virtual CXMLText    *getText   () const { return NULL; }
  virtual CXMLExecute *getExecute() const { return NULL; }

  virtual void print(std::ostream &os) const = 0;

  friend std::ostream &operator<<(std::ostream &os, const CXMLToken &token);

 private:
  CXMLToken(const CXMLToken &rhs);
  CXMLToken &operator=(const CXMLToken &rhs);
};

class CXMLCommentToken : public CXMLToken {
 private:
  CXMLComment *comment_;

 public:
  CXMLCommentToken(CXMLTag *ptag, CXMLComment *comment);

  CXMLComment *getComment() const { return comment_; }

  void print(std::ostream &os) const;

 private:
  CXMLCommentToken(const CXMLCommentToken &rhs);
  CXMLCommentToken &operator=(const CXMLCommentToken &rhs);
};

class CXMLTagToken : public CXMLToken {
 private:
  CXMLTag *tag_;

 public:
  CXMLTagToken(CXMLTag *ptag, CXMLTag *tag);

  CXMLTag *getTag() const { return tag_; }

  void print(std::ostream &os) const;

 private:
  CXMLTagToken(const CXMLTagToken &rhs);
  CXMLTagToken &operator=(const CXMLTagToken &rhs);
};

class CXMLTextToken : public CXMLToken {
 private:
  CXMLText *text_;

 public:
  CXMLTextToken(CXMLTag *ptag, CXMLText *text);

  CXMLText *getText() const { return text_; }

  void print(std::ostream &os) const;

 private:
  CXMLTextToken(const CXMLTextToken &rhs);
  CXMLTextToken &operator=(const CXMLTextToken &rhs);
};

class CXMLExecuteToken : public CXMLToken {
 private:
  CXMLExecute *exec_;

 public:
  CXMLExecuteToken(CXMLTag *ptag, CXMLExecute *exec);

  CXMLExecute *getExecute() const { return exec_; }

  void print(std::ostream &os) const;

 private:
  CXMLExecuteToken(const CXMLExecuteToken &rhs);
  CXMLExecuteToken &operator=(const CXMLExecuteToken &rhs);
};

#endif
