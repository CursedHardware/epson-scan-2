
#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <cstdio>

#include "../ESJsonUtils.h"

using namespace rapidjson;

typedef GenericDocument< UTF16<> >                         Document16;
typedef GenericValue< UTF16<> >                            Value16;

typedef GenericStringStream< UTF16<> >                     StringStream16;
typedef EncodedInputStream< UTF16<>, FileReadStream >      EncodedInputStream16;
typedef EncodedOutputStream< UTF16LE<>, FileWriteStream >  EncodedOutputStream16LE;

typedef GenericStringBuffer< UTF16<> >                     StringBuffer16;
typedef PrettyWriter< StringBuffer16, UTF16<>, UTF16<> >   BufferWriter16;

typedef EncodedOutputStream16LE                            EncodeStream16;
typedef PrettyWriter< EncodeStream16, UTF16LE<>, UTF16<> > FileWriter16;


typedef GenericDocument< UTF8<> >                         Document8;
typedef GenericValue< UTF8<> >                            Value8;

typedef GenericStringStream< UTF8<> >                     StringStream8;
typedef EncodedInputStream< UTF8<>, FileReadStream >      EncodedInputStream8;
typedef EncodedOutputStream< UTF8<>, FileWriteStream >  EncodedOutputStream8;

typedef GenericStringBuffer< UTF8<> >                     StringBuffer8;
typedef PrettyWriter< StringBuffer8, UTF8<>, UTF8<> >   BufferWriter8;

typedef EncodedOutputStream8                            EncodeStream8;
typedef PrettyWriter< EncodeStream8, UTF8<> > FileWriter8;
