#include <CYaml.h>
#include <CStrParse.h>
#include <CUtf8.h>
#include <cassert>
#include <iostream>

namespace {
  inline int hexCharValue(char c) {
    if (isdigit(c)) return (c - '0');

    return (tolower(c) - 'a' + 10);
  }
}

//---

CYaml::
CYaml()
{
}

CYaml::
~CYaml()
{
}

bool
CYaml::
parse(const std::string &filename)
{
  filename_ = filename;

  if (! openFile())
    return false;

  processLines();

  closeFile();

  return true;
}

bool
CYaml::
openFile()
{
  fp_ = fopen(filename_.c_str(), "r");
  if (! fp_) return false;

  return true;
}

void
CYaml::
closeFile()
{
  if (fp_)
    fclose(fp_);

  fp_ = nullptr;
}

bool
CYaml::
readLine(std::string &line)
{
  line = "";

  if (lineBuffer_.valid) {
    for (int i = 0; i < lineBuffer_.indent; ++i)
      line += " ";

    line += lineBuffer_.str;

    lineBuffer_.valid = false;

    return true;
  }

  if (feof(fp_)) return false;

  char c = char(fgetc(fp_));

  while (! feof(fp_) && c != '\n') {
    line += c;

    c = char(fgetc(fp_));
  }

  return true;
}

void
CYaml::
unreadLine(const std::string &str, int indent)
{
  assert(! lineBuffer_.valid);

  lineBuffer_.str    = str;
  lineBuffer_.indent = indent;
  lineBuffer_.valid  = true;
}

bool
CYaml::
readNextLine(std::string &line, int &indent)
{
  indent = 0;

  while (! feof(fp_)) {
    std::string line1;

    if (! readLine(line1))
      return false;

    CStrParse parse(line1);

    parse.skipSpace();

    uint indent1 = parse.getPos();

    std::string line2;
    bool        hasNonSpace = false;

    while (! parse.eof()) {
      if      (parse.isChar('\'')) {
        uint pos = parse.getPos();

        while (! parse.eof()) {
          if (parse.isChar('\'')) {
            parse.skipChar();

            if (parse.isChar('\''))
              parse.skipChar();
            else
              break;
          }
          else
            parse.skipChar();
        }

        line2 += parse.getAt(pos);

        hasNonSpace = true;
      }
      else if (parse.isChar('"')) {
        uint pos = parse.getPos();

        parse.skipString();

        line2 += parse.getAt(pos);

        hasNonSpace = true;
      }
      else if (parse.isChar('#')) {
        break;
      }
      else {
        if (! hasNonSpace && ! parse.isSpace())
          hasNonSpace = true;

        line2 += parse.readChar();
      }
    }

    if (! hasNonSpace)
      continue;

    line   = line2;
    indent = int(indent1);

    return true;
  }

  return false;
}

void
CYaml::
print(std::ostream &) const
{
}

void
CYaml::
processLines()
{
  int indent = 0;

  scope_ = loadScope(indent);

  if (isDebug()) {
    std::cerr << "--- Lines ---\n";

    printScopeLines(std::cerr, scope_, 0);
  }

  processScope(scope_);

  if (isDebug()) {
    std::cerr << "\n";

    std::cerr << "--- Data ---\n";

    CYamlStream os;

    printScopeData(os, scope_, 0);

    std::cerr << os.str();
  }
}

CYaml::Scope *
CYaml::
loadScope(int &indent)
{
  Scope *scope = new Scope(indent);

  std::string line;
  int         indent1;

  while (readNextLine(line, indent1)) {
    if      (indent1 < scope->indent()) {
      unreadLine(line, indent1);

      indent = indent1;

      break;
    }
    else if (indent1 == scope->indent()) {
      CStrParse parse(line);

      if (parse.isString("- ") || parse.isStringEnd("-")) {
        parse.skipChars(2);

        parse.skipSpace();

        std::string line1 = parse.getAt();

        if (line1 != "")
          unreadLine(line1, indent1 + int(parse.getPos()));

        line = "-";
      }

      scope->addLine(line);
    }
    else {
      unreadLine(line, indent1);

      Scope *scope1 = loadScope(indent1);

      scope->addScope(scope1);

      if (indent1 < indent) {
        indent = indent1;
        break;
      }
    }
  }

  return scope;
}

void
CYaml::
printScopeLines(std::ostream &os, Scope *scope, int depth)
{
  auto printDepth = [&]() { for (int i = 0; i < depth; ++i) os << " "; };

  for (const auto &b : scope->blocks()) {
    for (const auto &l : b->lines) {
      printDepth(); os << l << "\n";
    }

    if (b->child)
      printScopeLines(os, b->child, depth + 1);
  }
}

bool
CYaml::
processScope(Scope *scope)
{
  for (auto &b : scope->blocks()) {
    if (! b->lines.empty()) {
      uint i = 0;
      uint n = uint(b->lines.size());

      while (i < n) {
        const std::string &line = b->lines[i];

        CStrParse parse(line);

        if (! parse.isString("- ") && ! parse.isStringEnd("-"))
          break;

        parse.skipChars(2);

        parse.skipSpace();

        if (scope->type() == ScalarData::Type::NONE) {
          scope->setType(ScalarData::Type::SEQUENCE);

          scope->setIsBlock(true);
        }

        Scalar name = readScalar(parse);

        scope->addValue(name);

        ++i;
      }

      //---

      while (i < n) {
        const std::string &line = b->lines[i];

        CStrParse parse(line);

        Scalar name = readScalar(parse);

        if (! parse.isString(": ") && ! parse.isStringEnd(":"))
          break;

        parse.skipChars(2);

        parse.skipSpace();

        if (scope->type() == ScalarData::Type::NONE) {
          scope->setType(ScalarData::Type::MAPPING);

          scope->setIsBlock(true);
        }

        Scalar value = readScalar(parse);

        scope->addNameValue(name, value);

        ++i;
      }

      //---

      while (i < n) {
        const std::string &line = b->lines[i];

        CStrParse parse(line);

        Scalar name = readScalar(parse);

        if (scope->parent()) {
          if (scope->parent()->type() == ScalarData::Type::NONE) {
            scope->parent()->setType(ScalarData::Type::SEQUENCE);

            scope->parent()->setIsBlock(true);
          }

          scope->parent()->setLastValue(name);
        }

        ++i;
      }

      //---

      if      (scope->type() == ScalarData::Type::SEQUENCE) {
        if (b->child) {
          if (! processScope(b->child))
            return false;

          if (b->child->hasData())
            scope->setLastValue(Scalar(b->child->extractData()));
        }
      }
      else if (scope->type() == ScalarData::Type::MAPPING) {
        if (b->child) {
          if (! processScope(b->child))
            return false;

          if (b->child->hasData())
            scope->setLastNameValue(Scalar(b->child->extractData()));
        }
      }
      else {
        if (b->child) {
          if (! processScope(b->child))
            return false;
        }
      }
    }
    else {
      if (b->child) {
        if (! processScope(b->child))
          return false;
      }
    }
  }

  return true;
}

void
CYaml::
printScopeData(CYamlStream &os, Scope *scope, int depth)
{
  auto printDepth = [&]() { os.setIndent(depth); };

  auto printScalar = [&](const Scalar &s, int depth1) {
    s.print(os, depth1);
  };

  if      (scope->type() == ScalarData::Type::SEQUENCE) {
    printDepth();

    os << "[\n";

    ++depth;

    for (auto &v : scope->sequence()) {
      printDepth();

      printScalar(v, depth);

      os << "\n";
    }

    --depth;

    printDepth();

    os << "]\n";
  }
  else if (scope->type() == ScalarData::Type::MAPPING) {
    printDepth();

    os << "{\n";

    ++depth;

    for (auto &nv : scope->mapping()) {
      printDepth();

      printScalar(nv.first, depth);

      os << ": ";

      printScalar(nv.second, depth);

      os << "\n";
    }

    --depth;

    printDepth();

    os << "}\n";
  }

  for (auto &b : scope->blocks()) {
    if (b->child)
      printScopeData(os, b->child, depth + 1);
  }
}

void
CYaml::
visit(Visitor &v) const
{
  visit(scope_, v);
}

void
CYaml::
visit(Scope *scope, Visitor &v) const
{
  if      (scope->type() == ScalarData::Type::SEQUENCE) {
    v.visitStartSequence(scope);

    for (auto &sv : scope->sequence()) {
      if (sv.type() == Scalar::Type::DATA && sv.data()->isBlock())
        visit(scope, sv.data(), v);
      else
        v.visitSequence(scope, sv);
    }

    v.visitEndSequence(scope);
  }
  else if (scope->type() == ScalarData::Type::MAPPING) {
    v.visitStartMapping(scope);

    for (auto &nv : scope->mapping()) {
      if (nv.second.type() == Scalar::Type::DATA && nv.second.data()->isBlock()) {
        v.visitMapping(scope, nv.first, nv.second);

        visit(scope, nv.second.data(), v);
      }
      else
        v.visitMapping(scope, nv.first, nv.second);
    }

    v.visitEndMapping(scope);
  }

  for (auto &b : scope->blocks()) {
    if (b->child)
      visit(b->child, v);
  }
}

void
CYaml::
visit(Scope *scope, ScalarData *data, Visitor &v) const
{
  if      (data->type() == ScalarData::Type::SEQUENCE) {
    v.visitStartSequence(scope);

    for (const auto &sv : data->sequence()) {
      if (sv.type() == Scalar::Type::DATA && sv.data()->isBlock())
        visit(scope, sv.data(), v);
      else
        v.visitSequence(scope, sv);
    }

    v.visitEndSequence(scope);
  }
  else if (data->type() == ScalarData::Type::MAPPING) {
    v.visitStartMapping(scope);

    for (const auto &nv : data->mapping()) {
      if (nv.second.type() == Scalar::Type::DATA && nv.second.data()->isBlock()) {
        v.visitMapping(scope, nv.first, nv.second);

        visit(scope, nv.second.data(), v);
      }
      else
        v.visitMapping(scope, nv.first, nv.second);
    }

    v.visitEndMapping(scope);
  }
}

CYaml::Scalar
CYaml::
readScalar(CStrParse &parse)
{
  Scalar scalar;

  if (parse.eof())
    return scalar;

  //---

  std::string id;

  if      (parse.isChar('&')) {
    parse.skipChar();

    id = readId(parse);
  }
  else if (parse.isChar('*')) {
    parse.skipChar();

    id = readId(parse);

    return getId(id);
  }

  //---

  std::string word;

  if      (parse.isChar('"')) {
    parse.skipChar();

    while (! parse.eof()) {
      if      (parse.isChar('\\')) {
        parse.skipChar();

        if (! parse.eof()) {
          char c = parse.readChar();

          switch (c) {
            case '\"': word += '\"'; break;
            case '\\': word += '\\'; break;
            case '/' : word += '/' ; break;
            case 'b' : word += '\b'; break;
            case 'f' : word += '\f'; break;
            case 'n' : word += '\n'; break;
            case 'r' : word += '\r'; break;
            case 't' : word += '\t'; break;
            case 'x' : {
              // 2 hexadecimal digits
              long i = 0;

              for (int j = 0; j < 2; ++j) {
                if (! parse.isXDigit())
                  break;

                char c1 = parse.readChar();

                i = (i << 4) | (hexCharValue(c1) & 0xF);
              }

              CUtf8::append(word, ulong(i));

              break;
            }
            case 'u' : {
              // 4 hexadecimal digits
              long i = 0;

              for (int j = 0; j < 4; ++j) {
                if (! parse.isXDigit())
                  break;

                char c1 = parse.readChar();

                i = (i << 4) | (hexCharValue(c1) & 0xF);
              }

              CUtf8::append(word, ulong(i));

              break;
            }
            default: {
              word += c;

              break;
            }
          }
        }
        else {
          word += '\\';
          word += parse.readChar();
        }
      }
      else if (parse.isChar('"'))
        break;
      else {
        word += parse.readChar();
      }
    }

    if (parse.isChar('"'))
      parse.skipChar();

    scalar = Scalar(word);
  }
  else if (parse.isChar('\'')) {
    parse.skipChar();

    while (! parse.eof()) {
      if (parse.isChar('\'')) {
        parse.skipChar();

        if (parse.isChar('\''))
          word += parse.readChar();
        else
          break;
      }
      else
        word += parse.readChar();
    }

    scalar = Scalar(word);
  }
  else if (parse.isChar('[')) {
    ScalarData *data = new ScalarData;

    data->setType(ScalarData::Type::SEQUENCE);

    parse.skipChar();

    while (! parse.isChar(']')) {
      Scalar word1 = readScalar(parse);

      if (! word1.isNull())
        data->addValue(word1);

      if (! parse.isChar(','))
        break;

      parse.skipChar();
    }

    if (parse.isChar(']'))
      parse.skipChar();

    scalar = Scalar(data);
  }
  else if (parse.isChar('{')) {
    ScalarData *data = new ScalarData;

    data->setType(ScalarData::Type::MAPPING);

    parse.skipChar();

    NameValues nameValues;

    while (! parse.isChar('}')) {
      Scalar name = readScalar(parse);

      if (! parse.isChar(':'))
        break;

      parse.skipChar();

      Scalar value = readScalar(parse);

      data->addNameValue(name, value);

      if (! parse.isChar(','))
        break;

      parse.skipChar();
    }

    if (parse.isChar('}'))
      parse.skipChar();

    scalar = Scalar(data);
  }
  else {
    parse.skipSpace();

    int lastNonSpace = -1;

    while (! parse.eof()) {
      if      (parse.isString("- ") || parse.isStringEnd("-"))
        break;
      else if (parse.isString(": ") || parse.isStringEnd(":"))
        break;
      else if (parse.isChar(','))
        break;
      else if (parse.isChar('['))
        break;
      else if (parse.isChar(']'))
        break;
      else if (parse.isChar('{'))
        break;
      else if (parse.isChar('}'))
        break;
      else if (parse.isChar('#'))
        break;

      if (! parse.isSpace())
        lastNonSpace = int(word.size());

      word += parse.readChar();
    }

    if (lastNonSpace >= 0)
      word = word.substr(0, size_t(lastNonSpace + 1));

    scalar = Scalar(word);
  }

  setId(id, scalar);

  return scalar;
}

std::string
CYaml::
readId(CStrParse &parse) const
{
  std::string id;

  while (! parse.eof()) {
    if (parse.isSpace())
      break;

    id += parse.readChar();
  }

  return id;
}

CYaml::Scalar
CYaml::
getId(const std::string &id) const
{
  auto p = idMap_.find(id);

  if (p != idMap_.end())
    return (*p).second;

  return Scalar();
}

void
CYaml::
setId(const std::string &id, const Scalar &value)
{
  idMap_[id] = value;
}

//------

std::string
CYaml::ScalarData::
str() const
{
  CYamlStream os;

  print(os, 0);

  return os.str();
}

void
CYaml::ScalarData::
print(CYamlStream &os, int depth) const
{
  auto printDepth = [&]() { os.setIndent(depth); };

  auto printScalar = [&](const Scalar &s, int depth1) {
    s.print(os, depth1);
  };

  if (! mapping_.empty()) {
    printDepth();

    os << "{";

    if (isBlock()) {
      os << "\n";

      ++depth;
    }

    bool first = true;

    for (auto &nv : mapping_) {
      if (! first) {
        if (isBlock())
          os << ",\n";
        else
          os << ", ";
      }

      printDepth();

      printScalar(nv.first, depth);

      os << ": ";

      printScalar(nv.second, depth);

      first = false;
    }

    if (isBlock()) {
      --depth;

      os << "\n";
    }

    printDepth();

    os << "}";
  }

  if (! sequence_.empty()) {
    printDepth();

    os << "[";

    if (isBlock()) {
      os << "\n";

      ++depth;
    }

    bool first = true;

    for (auto &v : sequence_) {
      if (! first) {
        if (isBlock())
          os << ",\n";
        else
          os << ", ";
      }

      printDepth();

      printScalar(v, depth);

      first = false;
    }

    if (isBlock()) {
      --depth;

      os << "\n";
    }

    printDepth();

    os << "]";
  }
}
