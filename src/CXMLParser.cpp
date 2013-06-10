#include "CXMLI.h"
#include <CRegExp.h>
#include <CStrParse.h>

CXMLParser::
CXMLParser(CXML &xml) :
 xml_(xml), file_(NULL), root_tag_(NULL), tag_(NULL), buffer_(), line_num_(1), char_num_(0)
{
}

CXMLParser::
~CXMLParser()
{
}

bool
CXMLParser::
read(const string &filename, CXMLTag **tag)
{
  if (! CFile::exists(filename)) {
    CTHROW("File " + filename + " does not exist");
    return false;
  }

  root_tag_ = NULL;
  tag_      = NULL;

  file_ = new CFile(filename);

  readLoop();

  file_ = NULL;

  if (lookChar() != EOF)
    return false;

  *tag = root_tag_;

  return true;
}

bool
CXMLParser::
readString(const string &str, CXMLTag **tag)
{
  unreadChars(str);

  root_tag_ = NULL;
  tag_      = NULL;

  file_ = NULL;

  readLoop();

  if (lookChar() != EOF)
    return false;

  *tag = root_tag_;

  return true;
}

bool
CXMLParser::
readStringOptions(const string &str, CXMLTag::OptionArray &options)
{
  unreadChars(str);

  return readTagOptions(options);
}

void
CXMLParser::
readLoop()
{
  skipSpaces();

  while (lookChar() != EOF) {
    if      (isDocType()) {
      if (! readDocType())
        break;
    }
    else if (isCDATA()) {
      if (! readCDATA())
        break;
    }
    else if (isComment()) {
      if (! readComment())
        break;
    }
    else if (isExecute()) {
      if (! readExecute())
        break;
    }
    else if (isTag()) {
      if (! readTag())
        break;
    }
    else {
      if (! readText())
        break;
    }

    if (tag_ == NULL || ! tag_->getPreserveSpace())
      skipSpaces();
  }
}

bool
CXMLParser::
isDocType()
{
  string str;

  if (! matchString("<!DOCTYPE "))
    return false;

  unreadChars("<!DOCTYPE ");

  return true;
}

bool
CXMLParser::
readDocType()
{
  string str;

  if (! matchString("<!DOCTYPE "))
    return false;

  str += "<!DOCTYPE ";

  bool in_string1  = false;
  bool in_sbracket = false;

  int c = lookChar();

  while (c != EOF) {
    if      (in_string1) {
      if (c == '"')
        in_string1 = false;
    }
    else if (in_sbracket) {
      if      (c == '"')
        in_string1 = true;
      else if (c == ']')
        in_sbracket = false;
    }
    else {
      if      (c == '"')
        in_string1 = true;
      else if (c == '[')
        in_sbracket = true;
      else if (c == '>')
        break;
    }

    str += readChar();

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  str += readChar();

  if (xml_.getDebug())
    cerr << "Doc Type: " << str << endl;

  return true;
}

bool
CXMLParser::
isCDATA()
{
  string str;

  if (! matchString("<!CDATA["))
    return false;

  unreadChars("<!CDATA[");

  return true;
}

bool
CXMLParser::
readCDATA()
{
  string str;

  if (! matchString("<!CDATA["))
    return false;

  str += "<!CDATA[";

  int c = lookChar();

  while (c != EOF) {
    if (matchString("]]>")) {
      str += "]]>";
      break;
    }

    str += readChar();

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  if (xml_.getDebug())
    cerr << "CDATA: " << str << endl;

  return true;
}

bool
CXMLParser::
isComment()
{
  string str;

  if (! matchString("<!--"))
    return false;

  unreadChars("<!--");

  return true;
}

bool
CXMLParser::
readComment()
{
  string str;

  if (! matchString("<!--"))
    return false;

  str += "<!--";

  //------

  int c = lookChar();

  while (c != EOF) {
    if (matchString("-->")) {
      str += "-->";
      break;
    }

    str += readChar();

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  if (tag_ != NULL) {
    CXMLComment *comment = xml_.createComment(str);

    new CXMLCommentToken(tag_, comment);
  }

  if (xml_.getDebug())
    cerr << "Comment: " << str << endl;

  return true;
}

bool
CXMLParser::
isExecute()
{
  string str;

  if (! matchString("<?"))
    return false;

  unreadChars("<?");

  return true;
}

bool
CXMLParser::
readExecute()
{
  if (! matchString("<?"))
    return false;

  string lhs = "<?";
  string rhs = "?>";

  string data;

  int c = lookChar();

  while (c != EOF) {
    if (matchString("?>"))
      break;

    data += readChar();

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(lhs + data);

    return false;
  }

  if (xml_.getDebug())
    cerr << "Execute: " << (lhs + data + rhs) << endl;

  //------

  return parseExecute(data);
}


bool
CXMLParser::
parseExecute(const string &str)
{
  CStrParse parse(str);

  if (parse.eof())
    return false;

  char c;

  parse.readChar(&c);

  if (! isNameFirstChar(c))
    return false;

  string id;

  id += c;

  c = parse.getCharAt();

  while (c != EOF && isNameChar(c)) {
    parse.readChar(&c);

    id += c;

    c = parse.getCharAt();
  }

  if (xml_.getDebug())
    cerr << "Execute: " << id;

  CXMLExecute *exec = new CXMLExecute(id);

  while (! parse.eof()) {
    parse.skipSpace();

    //------

    c = parse.getCharAt();

    if (! isNameFirstChar(c))
      break;

    //------

    string name;

    c = parse.getCharAt();

    while (c != EOF && isNameChar(c)) {
      parse.readChar(&c);

      name += c;

      c = parse.getCharAt();
    }

    if (name == "")
      break;

    //------

    parse.skipSpace();

    //------

    string value;

    c = parse.getCharAt();

    if (c == '=') {
      parse.readChar(&c);

      parse.skipSpace();

      c = parse.getCharAt();

      if      (c == '\'') {
        parse.skipChar();

        while (! parse.eof()) {
          parse.readChar(&c);

          if (c == '\'')
            break;

          value += c;
        }

        if (c == '\'')
          parse.skipChar();
      }
      else if (c == '\"') {
        parse.skipChar();

        while (! parse.eof()) {
          parse.readChar(&c);

          if (c == '\"')
            break;

          value += c;
        }

        if (c == '\"')
          parse.skipChar();
      }
      else {
        parseError("Invalid option format");

        return false;
      }
    }

    //------

    value = replaceNamedChars(value);

    //------

    if (xml_.getDebug())
      cerr << " " << name << "=\"" << value << "\"";

    exec->addOption(name, value);
  }

  if (xml_.getDebug())
    cerr << endl;

  CXMLExecuteToken *token = new CXMLExecuteToken(NULL, exec);

  xml_.addToken(token);

  return true;
}

bool
CXMLParser::
isTag()
{
  int c = lookChar();

  if (c != '<')
    return false;

  string str;

  str += readChar();

  c = lookChar();

  if (c == '/')
    str += readChar();

  c = lookChar();

  if (! isNameFirstChar(c)) {
    unreadChars(str);

    return false;
  }

  unreadChars(str);

  return true;
}

bool
CXMLParser::
readTag()
{
  int c = readChar();

  if (c != '<') {
    CTHROW("readTag: internal error");
    return false;
  }

  //------

  bool end_tag = false;

  c = readChar();

  if (c == '/') {
    end_tag = true;

    c = readChar();
  }

  if (c != EOF)
    unreadChar(c);

  //------

  c = readChar();

  if (c != EOF && ! isNameFirstChar(c)) {
    CTHROW("readTag: internal error");
    return false;
  }

  string name;

  name += c;

  c = readChar();

  while (c != EOF && isNameChar(c)) {
    name += c;

    c = readChar();
  }

  if (c != EOF)
    unreadChar(c);

  //------

  skipSpaces();

  CXMLTag::OptionArray options;

  if (! readTagOptions(options))
    return false;

  //------

  skipSpaces();

  c = readChar();

  bool auto_close = false;

  if (c == '/') {
    auto_close = true;

    c = readChar();
  }

  if (end_tag && auto_close) {
    parseError("Bad /> on end tag: " + name);

    auto_close = false;
  }

  if (c != '>') {
    parseError("Bad close > on tag: " + name);

    while (c != EOF && c != '>')
      c = readChar();
  }

  //------

  if (! end_tag) {
    CXMLTag *tag = xml_.createTag(tag_, name, options);

    tag->setLocation(line_num_, char_num_);

    new CXMLTagToken(tag_, tag);

    if (tag_ == NULL) {
      if (root_tag_ == NULL)
        root_tag_ = tag;
      else
        cerr << "Multiple root tags" << endl;
    }

    if (! auto_close)
      tag_ = tag;

    //------

    if (xml_.getDebug())
      cerr << "Tag: " << *tag << endl;
  }
  else {
    const string &name1 = tag_->getName();

    if (name1 != name) {
      parseError("Start end tag mismatch <" + name + "> </" + name1 + ">");
      return false;
    }
    else
      tag_ = tag_->getParent();
  }

  return true;
}

bool
CXMLParser::
readTagOptions(CXMLTag::OptionArray &options)
{
  while (true) {
    skipSpaces();

    //------

    int c = lookChar();

    if (! isNameFirstChar(c))
      break;

    //------

    string name;

    c = readChar();

    while (c != EOF && isNameChar(c)) {
      name += c;

      c = readChar();
    }

    if (c != EOF)
      unreadChar(c);

    if (name == "")
      break;

    //------

    skipSpaces();

    //------

    string value;

    c = lookChar();

    if (c == '=') {
      c = readChar();

      skipSpaces();

      c = readChar();

      if      (c == '\'') {
        c = readChar();

        while (c != EOF) {
          if (c == '\'')
            break;

          value += c;

          c = readChar();
        }

        if (c == '\'')
          c = readChar();
      }
      else if (c == '\"') {
        c = readChar();

        while (c != EOF) {
          if (c == '\"')
            break;

          value += c;

          c = readChar();
        }

        if (c == '\"')
          c = readChar();
      }
      else {
        parseError("Invalid option format");

        if (c != EOF)
          unreadChar(c);

        return false;
      }

      if (c != EOF)
        unreadChar(c);
    }

    //------

    value = replaceNamedChars(value);

    //------

    CXMLTagOption *option = xml_.createTagOption(name, value);

    if (xml_.getDebug())
      cerr << "Option: " << name << "=\"" << value << "\"" << endl;

    options.push_back(option);
  }

  return true;
}

string
CXMLParser::
replaceNamedChars(const string &value)
{
  static CRegExp re1("#x[0-9a-fA-F][0-9a-fA-F]");
  static CRegExp re2("#[0-9][0-9]*");

  string value1;

  uint i   = 0;
  uint len = value.size();

  while (i < len) {
    char c = value[i];

    if (c == '&') {
      uint j = i++;

      while (i < len) {
        if (value[i] == ';')
          break;

        ++i;
      }

      if (i >= len) {
        value1 += value.substr(j);
        break;
      }

      uint len1 = i - j - 1;

      string name = value.substr(j + 1, len1);

      if      (re1.find(name)) {
        string hstr = name.substr(2);

        uint h;

        CStrUtil::decodeHexString(hstr, &h);

        value1 += char(h);
      }
      else if (re2.find(name)) {
        string dstr = name.substr(1);

        long l;

        CStrUtil::toInteger(dstr, &l);

        value1 += char(l);
      }
      else {
        CXMLNamedChar *named_char;

        if      (! CXMLNamedCharMgrInst->lookup(name, &named_char))
          value1 += value.substr(j, i - j + 1);
        else if (named_char->value > 255)
          value1 += value.substr(j, i - j + 1);
        else
          value1 += char(named_char->value);
      }

      ++i;
    }
    else {
      value1 += c;

      ++i;
    }
  }

  return value1;
}

bool
CXMLParser::
readText()
{
  int c = readChar();

  if (c == EOF)
    return true;

  string str;

  str += c;

  while (! isComment() && ! isTag()) {
    c = readChar();

    if (c == EOF)
      break;

    str += c;
  }

  //----

  if (tag_ == NULL) {
    cerr << "Text with no current tag" << endl;
    return false;
  }

  //----

  string str1;

  if (! tag_->getPreserveSpace()) {
    str1 = CStrUtil::stripSpaces(str);

    if (isTag()) str1 += " ";
  }
  else
    str1 = str;

  if (str1 == "")
    return true;

  //----

  str1 = replaceNamedChars(str1);

  CXMLText *text = xml_.createText(str1);

  new CXMLTextToken(tag_, text);

  if (xml_.getDebug())
    cerr << "Text: >" << str << "<" << endl;

  return true;
}

bool
CXMLParser::
isNameFirstChar(int c)
{
  return (isalpha(c) || c == '_' || c == ':');
}

bool
CXMLParser::
isNameChar(int c)
{
  return (isalnum(c) || c == '.' || c == '-' || c == '_' || c == ':');
}

bool
CXMLParser::
matchString(const string &str)
{
  string str1;

  int len = str.size();

  for (int i = 0; i < len; ++i) {
    int c = lookChar();

    if (c != str[i]) {
      unreadChars(str1);
      return false;
    }

    str1 += readChar();
  }

  return true;
}

void
CXMLParser::
skipSpaces()
{
  int c = readChar();

  while (c != EOF && isspace(c))
    c = readChar();

  if (c != EOF)
    unreadChar(c);
}

int
CXMLParser::
lookChar()
{
  if (buffer_.size() == 0) {
    int c = EOF;

    if (file_ != NULL)
      c = file_->getC();

    if (c == EOF)
      return EOF;

    buffer_.push_back(c);
  }

  return buffer_[buffer_.size() - 1];
}

int
CXMLParser::
readChar()
{
  if (buffer_.size() == 0) {
    if (file_ != NULL) {
      int c = file_->getC();

      if (c == '\n') {
        ++line_num_;

        char_num_ = 0;
      }
      else
        ++char_num_;

      return c;
    }
    else
      return EOF;
  }

  int c = buffer_[buffer_.size() - 1];

  if (c == '\n') {
    ++line_num_;

    char_num_ = 0;
  }
  else
    ++char_num_;

  buffer_.pop_back();

  return c;
}

void
CXMLParser::
unreadChars(const string &str)
{
  for (int i = str.size() - 1; i >= 0; --i) {
    if (str[i] == '\n') {
      --line_num_;

      char_num_ = 256;
    }
    else
      --char_num_;

    buffer_.push_back(str[i]);
  }
}

void
CXMLParser::
unreadChar(int c)
{
  if (c == '\n')
    --line_num_;
  else
    --char_num_;

  buffer_.push_back(c);
}

void
CXMLParser::
parseError(const char *fmt, ...)
{
  va_list vargs;

  va_start(vargs, fmt);

  cerr << line_num_ << ":" << char_num_ << "> ";
  cerr << CStrUtil::vstrprintf(fmt, &vargs) << endl;

  va_end(vargs);
}

void
CXMLParser::
parseError(const string &str)
{
  cerr << line_num_ << ":" << char_num_ << "> " << str << endl;
}
