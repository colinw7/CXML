#include <CXMLNamedChar.h>
#include <CUtf8.h>
#include <cstring>

CXMLNamedChar
CXMLNamedCharMgr::
named_chars_[] = {
  CXMLNamedChar("AElig" , 198),
  CXMLNamedChar("Aacute", 193),
  CXMLNamedChar("Acirc" , 194),
  CXMLNamedChar("Agrave", 192),
  CXMLNamedChar("Aring" , 197),
  CXMLNamedChar("Atilde", 195),
  CXMLNamedChar("Auml"  , 196),
  CXMLNamedChar("Ccedil", 199),
  CXMLNamedChar("ETH"   , 208),
  CXMLNamedChar("Eacute", 201),
  CXMLNamedChar("Ecirc" , 202),
  CXMLNamedChar("Egrave", 200),
  CXMLNamedChar("Euml"  , 203),
  CXMLNamedChar("Iacute", 205),
  CXMLNamedChar("Icirc" , 206),
  CXMLNamedChar("Igrave", 204),
  CXMLNamedChar("Iuml"  , 207),
  CXMLNamedChar("Ntilde", 209),
  CXMLNamedChar("Oacute", 211),
  CXMLNamedChar("Ocirc" , 212),
  CXMLNamedChar("Ograve", 210),
  CXMLNamedChar("Oslash", 216),
  CXMLNamedChar("Otilde", 213),
  CXMLNamedChar("Ouml"  , 214),
  CXMLNamedChar("THORN" , 222),
  CXMLNamedChar("Uacute", 218),
  CXMLNamedChar("Ucirc" , 219),
  CXMLNamedChar("Ugrave", 217),
  CXMLNamedChar("Uuml"  , 220),
  CXMLNamedChar("Yacute", 221),
  CXMLNamedChar("aacute", 225),
  CXMLNamedChar("acute" , 180),
  CXMLNamedChar("acirc" , 226),
  CXMLNamedChar("aelig" , 230),
  CXMLNamedChar("agrave", 224),
  CXMLNamedChar("amp"   ,  38),
  CXMLNamedChar("apos"  ,  39),
  CXMLNamedChar("aring" , 229),
  CXMLNamedChar("atilde", 227),
  CXMLNamedChar("auml"  , 228),
  CXMLNamedChar("brvbar", 166),
  CXMLNamedChar("ccedil", 231),
  CXMLNamedChar("cedil" , 184),
  CXMLNamedChar("cent"  , 162),
  CXMLNamedChar("copy"  , 169),
  CXMLNamedChar("curren", 164),
  CXMLNamedChar("deg"   , 176),
  CXMLNamedChar("divide", 247),
  CXMLNamedChar("eacute", 233),
  CXMLNamedChar("ecirc" , 234),
  CXMLNamedChar("egrave", 232),
  CXMLNamedChar("eth"   , 240),
  CXMLNamedChar("euml"  , 235),
  CXMLNamedChar("frac12", 189),
  CXMLNamedChar("frac14", 188),
  CXMLNamedChar("frac34", 190),
  CXMLNamedChar("gt"    ,  62),
  CXMLNamedChar("iacute", 237),
  CXMLNamedChar("icirc" , 238),
  CXMLNamedChar("iexcl" , 161),
  CXMLNamedChar("igrave", 236),
  CXMLNamedChar("iuml"  , 239),
  CXMLNamedChar("iquest", 191),
  CXMLNamedChar("laquo" , 171),
  CXMLNamedChar("lt"    ,  60),
  CXMLNamedChar("macr"  , 175),
  CXMLNamedChar("micro" , 181),
  CXMLNamedChar("middot", 183),
  CXMLNamedChar("nbsp"  , 160),
  CXMLNamedChar("not"   , 172),
  CXMLNamedChar("ntilde", 241),
  CXMLNamedChar("oacute", 243),
  CXMLNamedChar("ocirc" , 244),
  CXMLNamedChar("ograve", 242),
  CXMLNamedChar("ordf"  , 170),
  CXMLNamedChar("ordm"  , 186),
  CXMLNamedChar("oslash", 248),
  CXMLNamedChar("otilde", 245),
  CXMLNamedChar("ouml"  , 246),
  CXMLNamedChar("para"  , 182),
  CXMLNamedChar("plusmn", 177),
  CXMLNamedChar("pound" , 163),
  CXMLNamedChar("quot"  ,  34),
  CXMLNamedChar("raquo" , 187),
  CXMLNamedChar("reg"   , 174),
  CXMLNamedChar("sect"  , 167),
  CXMLNamedChar("shy"   , 173),
  CXMLNamedChar("sup1"  , 185),
  CXMLNamedChar("sup2"  , 178),
  CXMLNamedChar("sup3"  , 179),
  CXMLNamedChar("szlig" , 223),
  CXMLNamedChar("thorn" , 254),
  CXMLNamedChar("times" , 215),
  CXMLNamedChar("uacute", 250),
  CXMLNamedChar("ucirc" , 251),
  CXMLNamedChar("ugrave", 249),
  CXMLNamedChar("uml"   , 168),
  CXMLNamedChar("uuml"  , 252),
  CXMLNamedChar("yacute", 253),
  CXMLNamedChar("yen"   , 165),
  CXMLNamedChar("yuml"  , 255),
};

uint CXMLNamedCharMgr::num_named_chars_ = sizeof(named_chars_)/sizeof(CXMLNamedChar);

CXMLNamedCharMgr *
CXMLNamedCharMgr::
getInstance()
{
  static CXMLNamedCharMgr *instance_ = nullptr;

  if (! instance_)
    instance_ = new CXMLNamedCharMgr();

  return instance_;
}

CXMLNamedCharMgr::
CXMLNamedCharMgr()
{
  for (uint i = 1; i < num_named_chars_; ++i) {
    name_value_map_[named_chars_[i].name      ] = &named_chars_[i];
    value_name_map_[int(named_chars_[i].value)] = &named_chars_[i];
  }
}

bool
CXMLNamedCharMgr::
lookup(const std::string &name, CXMLNamedChar **named_char) const
{
  auto p = name_value_map_.find(name);

  if (p != name_value_map_.end()) {
    *named_char = p->second;

    return true;
  }

  return false;
}

bool
CXMLNamedCharMgr::
lookup(int value, CXMLNamedChar **named_char) const
{
  auto p = value_name_map_.find(value);

  if (p != value_name_map_.end()) {
    *named_char = p->second;

    return true;
  }

  return false;
}

// internal UTF8 string to XML text
std::string
CXMLNamedCharMgr::
encodeString(const std::string &str, bool printable) const
{
  static const char *encode_chars = "<\'\"&";

  CXMLNamedChar *named_char;

  std::string str1;

  int pos = 0;
  int len = int(str.size());

  while (pos < len) {
    ulong l = CUtf8::readNextChar(str, pos);

    if (l <= 0x7f) {
      uchar c = uchar(l);

      if (strchr(encode_chars, c) != nullptr) {
        lookup(int(c), &named_char);

        str1 += "&";
        str1 += named_char->name;
        str1 += ";";
      }
      else if (! isspace(c) && ! isprint(c) && printable) {
        str1 += "&#";

        int i1 = c/100;
        int i2 = (c - i1*100)/10;
        int i3 = c - i1*100 - i2*10;

        if (i1 > 0) str1 += char('0' + i1);
        if (i2 > 0) str1 += char('0' + i2);

        str1 += char('0' + i3);

        str1 += ";";
      }
      else
        str1 += char(c);
    }
    else {
      static char buffer[16];

      str1 += "&#x";

      int i1 = (l >> 24) & 0xFF;
      int i2 = (l >> 16) & 0xFF;
      int i3 = (l >> 8 ) & 0xFF;
      int i4 =  l        & 0xFF;

      std::string str2;

      ::sprintf(buffer, "%02x", i1); str2 += buffer;
      ::sprintf(buffer, "%02x", i2); str2 += buffer;
      ::sprintf(buffer, "%02x", i3); str2 += buffer;
      ::sprintf(buffer, "%02x", i4); str2 += buffer;

      size_t j = 0;

      while (str2[j] == '0')
        ++j;

      str1 += str2.substr(j);
      str1 += ";";
    }
  }

  return str1;
}
