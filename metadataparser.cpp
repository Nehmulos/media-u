#include "metadataparser.h"
#include "nwutils.h"
#include "N0Slib.h"

MetaDataParser::MetaDataParser()
{
}


QString MetaData::toJson() {
    stringstream ss;
    nw::JsonWriter jw(ss);
    NwUtils::describe(jw, "duration", this->duration);
    jw.describeArray("chapters", "chapter", this->chapters.size());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MetaDataChapter c = this->chapters[i];
        NwUtils::describe(jw, "start", c.start);
        NwUtils::describe(jw, "end", c.end);
        NwUtils::describe(jw, "title", c.title);
    }
    return QString(ss.str().c_str());
}
