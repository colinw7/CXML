#include "CXMLI.h"

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

uint CXMLNamedCharMgr::num_named_chars_ =
      sizeof(named_chars_)/sizeof(CXMLNamedChar);

CXMLNamedCharMgr *
CXMLNamedCharMgr::
getInstance()
{
  static CXMLNamedCharMgr *instance_ = NULL;

  if (instance_ == NULL)
    instance_ = new CXMLNamedCharMgr();

  return instance_;
}

CXMLNamedCharMgr::
CXMLNamedCharMgr() :
 name_value_map_(), value_name_map_()
{
  for (uint i = 1; i < num_named_chars_; ++i) {
    name_value_map_[named_chars_[i].name ] = &named_chars_[i];
    value_name_map_[named_chars_[i].value] = &named_chars_[i];
  }
}

bool
CXMLNamedCharMgr::
lookup(const string &name, CXMLNamedChar **named_char) const
{
  NameValueMap::const_iterator p = name_value_map_.find(name);

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
  ValueNameMap::const_iterator p = value_name_map_.find(value);

  if (p != value_name_map_.end()) {
    *named_char = p->second;

    return true;
  }

  return false;
}

string
CXMLNamedCharMgr::
encodeString(const string &str) const
{
  static const char *encode_chars = "\'\"&";

  CXMLNamedChar *named_char;

  string str1;

  uint len = str.size();

  for (uint i = 0; i < len; ++i) {
    if (strchr(encode_chars, str[i]) != NULL) {
      lookup(int(str[i]), &named_char);

      str1 += "&";
      str1 += named_char->name;
      str1 += ";";
    }
    else
      str1 += str[i];
  }

  return str1;
}
