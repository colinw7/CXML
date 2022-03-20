#include <CXMLParser.h>
#include <CXML.h>
#include <CXMLToken.h>
#include <CXMLExecute.h>
#include <CXMLNamedChar.h>

#include <CRegExp.h>
#include <CStrParse.h>
#include <CStrUtil.h>
#include <CThrow.h>
#include <CUtf8.h>
#include <cstdio>

CXMLParser::
CXMLParser(CXML &xml) :
 xml_(xml)
{
}

CXMLParser::
~CXMLParser()
{
}

bool
CXMLParser::
read(const std::string &filename, CXMLTag **tag)
{
  if (! CFile::exists(filename)) {
    CTHROW("File " + filename + " does not exist");
    return false;
  }

  root_tag_ = nullptr;
  tag_      = nullptr;

  file_ = std::make_unique<CFile>(filename);

  readLoop();

  file_ = nullptr;

  if (lookChar() != EOF)
    return false;

  *tag = root_tag_;

  return true;
}

bool
CXMLParser::
readString(const std::string &str, CXMLTag **tag)
{
  unreadChars(str);

  root_tag_ = nullptr;
  tag_      = nullptr;

  file_ = nullptr;

  readLoop();

  if (lookChar() != EOF)
    return false;

  *tag = root_tag_;

  return true;
}

bool
CXMLParser::
readStringOptions(const std::string &str, CXMLTag::OptionArray &options)
{
  unreadChars(str);

  return readTagOptions(options);
}

void
CXMLParser::
readLoop()
{
  bool skipped = skipSpaces();

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
      if (! readText(skipped))
        break;
    }

    if (! tag_ || ! tag_->getPreserveSpace())
      skipped = skipSpaces();
    else
      skipped = false;
  }
}

bool
CXMLParser::
isDocType()
{
  std::string str;

  if (! matchString("<!DOCTYPE "))
    return false;

  unreadChars("<!DOCTYPE ");

  return true;
}

bool
CXMLParser::
readDocType()
{
  std::string str;

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
      if      (isENTITY()) {
        (void) readENTITY();
      }
      else if (c == '"')
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

    str += char(readChar());

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  str += char(readChar());

  if (xml_.getDebug())
    std::cerr << "Doc Type: " << str << "\n";

  return true;
}

bool
CXMLParser::
isCDATA()
{
  std::string str;

  if (! matchString("<![CDATA["))
    return false;

  unreadChars("<![CDATA[");

  return true;
}

bool
CXMLParser::
readCDATA()
{
  std::string str;

  if (! matchString("<![CDATA["))
    return false;

  str += "<![CDATA[";

  int c = lookChar();

  while (c != EOF) {
    if (matchString("]]>")) {
      str += "]]>";
      break;
    }

    str += char(readChar());

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  //---

  if (! tag_) {
    std::cerr << "CDATA with no current tag\n";
    return false;
  }

  CXMLText *text = xml_.createText(tag_, str);

  new CXMLTextToken(tag_, text);

  //---

  if (xml_.getDebug())
    std::cerr << "CDATA: " << str << "\n";

  return true;
}

bool
CXMLParser::
isENTITY()
{
  std::string str;

  if (! matchString("<!ENTITY "))
    return false;

  unreadChars("<!ENTITY ");

  return true;
}

bool
CXMLParser::
readENTITY()
{
  std::string str;

  if (! matchString("<!ENTITY "))
    return false;

  bool in_string1  = false;

  int c = lookChar();

  while (c != EOF) {
    if      (in_string1) {
      if (c == '"')
        in_string1 = false;

      str += char(readChar());
    }
    else {
      if      (c == '"')
        in_string1 = true;
      else if (c == '>')
        break;

      str += char(readChar());
    }

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  // entity is name "value"
  CStrParse parse(str);

  parse.skipSpace();

  auto pos = parse.getPos();

  parse.skipNonSpace();

  std::string name = parse.getBefore(pos);

  parse.skipSpace();

  std::string value;

  if (parse.readString(value, /*strip_quotes*/true))
    xml_.setEntity(name, value);

  if (xml_.getDebug())
    std::cerr << "<!ENTITY " << str << ">\n";

  return true;
}

bool
CXMLParser::
isComment()
{
  std::string str;

  if (! matchString("<!--"))
    return false;

  unreadChars("<!--");

  return true;
}

bool
CXMLParser::
readComment()
{
  std::string str;

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

    str += char(readChar());

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(str);

    return false;
  }

  if (tag_) {
    CXMLComment *comment = xml_.createComment(tag_, str);

    new CXMLCommentToken(tag_, comment);
  }

  if (xml_.getDebug())
    std::cerr << "Comment: " << str << "\n";

  return true;
}

bool
CXMLParser::
isExecute()
{
  std::string str;

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

  std::string lhs = "<?";
  std::string rhs = "?>";

  std::string data;

  int c = lookChar();

  while (c != EOF) {
    if (matchString("?>"))
      break;

    data += char(readChar());

    c = lookChar();
  }

  if (c == EOF) {
    unreadChars(lhs + data);

    return false;
  }

  if (xml_.getDebug())
    std::cerr << "Execute: " << (lhs + data + rhs) << "\n";

  //------

  return parseExecute(data);
}


bool
CXMLParser::
parseExecute(const std::string &str)
{
  CStrParse parse(str);

  if (parse.eof())
    return false;

  char c;

  parse.readChar(&c);

  if (! isNameFirstChar(c))
    return false;

  std::string id;

  id += c;

  c = parse.getCharAt();

  while (c != EOF && isNameChar(c)) {
    parse.readChar(&c);

    id += c;

    c = parse.getCharAt();
  }

  if (xml_.getDebug())
    std::cerr << "Execute: " << id;

  CXMLExecute *exec = new CXMLExecute(id);

  while (! parse.eof()) {
    parse.skipSpace();

    //------

    c = parse.getCharAt();

    if (! isNameFirstChar(c))
      break;

    //------

    std::string name;

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

    std::string value;

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
      else
        return parseError("Invalid option format");
    }

    //------

    value = replaceNamedChars(value);

    //------

    if (xml_.getDebug())
      std::cerr << " " << name << "=\"" << value << "\"";

    exec->addOption(name, value);
  }

  if (xml_.getDebug())
    std::cerr << "\n";

  CXMLExecuteToken *token = new CXMLExecuteToken(0, exec);

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

  std::string str;

  str += char(readChar());

  c = lookChar();

  if (c == '/')
    str += char(readChar());

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

  std::string name;

  name += char(c);

  c = readChar();

  while (c != EOF && isNameChar(c)) {
    name += char(c);

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

    tag->setLocation(int(line_num_), int(char_num_));

    new CXMLTagToken(tag_, tag);

    if (! tag_) {
      if (! root_tag_)
        root_tag_ = tag;
      else
        std::cerr << "Multiple root tags\n";
    }

    if (! auto_close)
      tag_ = tag;

    //------

    if (xml_.getDebug())
      std::cerr << "Tag: " << *tag << "\n";
  }
  else {
    const std::string &name1 = tag_->getName();

    if (name1 != name)
      return parseError("Start end tag mismatch <" + name + "> </" + name1 + ">");

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

    std::string name;

    c = readChar();

    while (c != EOF && isNameChar(c)) {
      name += char(c);

      c = readChar();
    }

    if (c != EOF)
      unreadChar(c);

    if (name == "")
      break;

    //------

    skipSpaces();

    //------

    std::string value;

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

          value += char(c);

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

          value += char(c);

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
      std::cerr << "Option: " << name << "=\"" << value << "\"\n";

    options.push_back(option);
  }

  return true;
}

std::string
CXMLParser::
replaceNamedChars(const std::string &value)
{
  static CRegExp *re1, *re2, *re3, *re4;

  if (! re1) {
    re1 = new CRegExp("#x[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]");
    re2 = new CRegExp("#x[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]");
    re3 = new CRegExp("#x[0-9a-fA-F][0-9a-fA-F]");
    re4 = new CRegExp("#[0-9][0-9]*");
  }

  std::string value1;

  uint i   = 0;
  uint len = uint(value.size());

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

      std::string name = value.substr(j + 1, len1);

      // hex char (4)
      if      (re1->find(name)) {
        std::string hstr = name.substr(2);

        uint h;

        CStrUtil::decodeHexString(hstr, &h);

        CUtf8::append(value1, h);
      }
      // hex char (3)
      else if (re2->find(name)) {
        std::string hstr = name.substr(2);

        uint h;

        CStrUtil::decodeHexString(hstr, &h);

        CUtf8::append(value1, h);
      }
      // hex char (2)
      else if (re3->find(name)) {
        std::string hstr = name.substr(2);

        uint h;

        CStrUtil::decodeHexString(hstr, &h);

        CUtf8::append(value1, h);
      }
      // decimal char
      else if (re4->find(name)) {
        std::string dstr = name.substr(1);

        long l;

        CStrUtil::toInteger(dstr, &l);

        CUtf8::append(value1, ulong(l));
      }
      // named char
      else {
        std::string    value2;
        CXMLNamedChar *named_char;

        if      (CXMLNamedCharMgrInst->lookup(name, &named_char))
          CUtf8::append(value1, named_char->value);
        else if (xml_.getEntity(name, value2))
          value1 += value2;
        else
          value1 += value.substr(j, i - j + 1);
      }

      ++i;
    }
    else {
      value1 += char(c);

      ++i;
    }
  }

  return value1;
}

bool
CXMLParser::
readText(bool skipped)
{
  std::string str;

  if (skipped)
    str += " ";

  int c = readChar();

  if (c == EOF)
    return true;

  str += char(c);

  while (! isComment() && ! isTag()) {
    c = readChar();

    if (c == EOF)
      break;

    if (c == '\n' && tag_ && ! tag_->getPreserveSpace()) {
      str += ' ';

      while (! isComment() && ! isTag()) {
        c = readChar();

        if (! isspace(c)) {
          str += char(c);

          break;
        }
      }
    }
    else
      str += char(c);
  }

  //----

  if (! tag_)
    return parseError("Text with no current tag");

  //----

  std::string str1;

  if (! tag_->getPreserveSpace()) {
    bool isSpace1 = (! str.empty() && isspace(str[0]));
    bool isSpace2 = (! str.empty() && isspace(str[str.size() - 1]));

    str1 = CStrUtil::stripSpaces(str);

    if (isTag()) {
      if (isSpace1)
        str1 = " " + str1;

      if (isSpace2)
        str1 = str1 + " ";
    }
  }
  else
    str1 = str;

  if (str1 == "")
    return true;

  //----

  str1 = replaceNamedChars(str1);

  CXMLText *text = xml_.createText(tag_, str1);

  new CXMLTextToken(tag_, text);

  if (xml_.getDebug())
    std::cerr << "Text: >" << str1 << "<\n";

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
matchString(const std::string &str)
{
  std::string str1;

  auto len = str.size();

  for (size_t i = 0; i < len; ++i) {
    int c = lookChar();

    if (c != str[i]) {
      unreadChars(str1);
      return false;
    }

    str1 += char(readChar());
  }

  return true;
}

bool
CXMLParser::
skipSpaces()
{
  bool skipped = false;

  int c = readChar();

  while (c != EOF && isspace(c)) {
    c = readChar();

    skipped = true;
  }

  if (c != EOF)
    unreadChar(c);

  return skipped;
}

int
CXMLParser::
lookChar()
{
  if (buffer_.size() == 0)
    fillBuffer();

  if (buffer_.size() > 0)
    return buffer_[buffer_.size() - 1];
  else
    return EOF;
}

int
CXMLParser::
readChar()
{
  if (buffer_.size() == 0)
    fillBuffer();

  if (buffer_.size() > 0) {
    int c = buffer_[buffer_.size() - 1];

    if (c == '\n') {
      ++line_num_;

      char_num_ = 0;

      line_ = "";
    }
    else {
      line_ += char(c);

      ++char_num_;
    }

    buffer_.pop_back();

    return c;
  }
  else
    return EOF;
}

void
CXMLParser::
fillBuffer()
{
  int c = EOF;

  if (file_)
    c = file_->getC();

  if (c == EOF)
    return;

  if (c == '&') {
    std::string name;

    c = file_->getC();

    while (c != EOF && c != ';') {
      name += char(c);

      c = file_->getC();
    }

    std::string str;

    if (c == ';') {
      std::string value;

      if (xml_.getEntity(name, value))
        str = value;
      else
        str = "&" + name + ";";
    }
    else
      str = "&" + name;

    auto len = str.size();

    for (int i = int(len - 1); i >= 0; --i)
      buffer_.push_back(str[size_t(i)]);
  }
  else
    buffer_.push_back(c);
}

void
CXMLParser::
unreadChars(const std::string &str)
{
  for (int i = int(str.size() - 1); i >= 0; --i) {
    if (str[size_t(i)] == '\n') {
      --line_num_;

      line_ = "";

      char_num_ = 256;
    }
    else
      --char_num_;

    buffer_.push_back(str[size_t(i)]);
  }
}

void
CXMLParser::
unreadChar(int c)
{
  if (c == '\n') {
    --line_num_;

    line_ = "";

    char_num_ = 256;
  }
  else
    --char_num_;

  buffer_.push_back(c);
}

bool
CXMLParser::
parseError(const char *fmt, ...)
{
  va_list vargs;

  va_start(vargs, fmt);

  std::cerr << "error: " << CStrUtil::vstrprintf(fmt, &vargs) << "\n";

  va_end(vargs);

  outputLineChar();

  return false;
}

bool
CXMLParser::
parseError(const std::string &str)
{
  std::cerr << "error: " << str << "\n";

  outputLineChar();

  return false;
}

void
CXMLParser::
outputLineChar()
{
  uint ll = uint(line_.size());

  std::cerr << "  " << line_ << " (" << line_num_ << ":" << char_num_ << ")\n";

  uint i = 0;

  std::cerr << "  ";

  for ( ; i < char_num_ && i < ll; ++i)
    std::cerr << " ";

  std::cerr << "^";

  for ( ; i < ll; ++i)
    std::cerr << " ";

  std::cerr << "\n";
}
