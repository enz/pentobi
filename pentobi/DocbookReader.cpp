//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "DocbookReader.h"

#include <QFile>
#include <QLocale>
#include <QStack>
#include <QXmlStreamReader>
#include "libboardgame_base/Assert.h"

using namespace std;

//-----------------------------------------------------------------------------

namespace {

void addHeader(QString& text)
{
    // Maybe explicitly setting the text and background colors can be avoided
    // in the future to support dark mode but right now TextArea in QtQuick
    // doesn't support dark mode anyway and with Qt 6.5/6.6, the default color
    // scheme is broken even in light mode (white h1 text on white background,
    // last tested on Ubuntu 23.10)
     text.append("<head><style>"
                 "body { background-color:white;color:black;"
                 "       line-height:115% }"
                 ":link { text-decoration:none;color:blue }"
                 "</style></head><body>"_L1);
}

} //namespace

//-----------------------------------------------------------------------------

DocbookReader::DocbookReader(QObject* parent)
    : QObject(parent)
{

    QString locale = QLocale::system().name();
    m_fileName = ":/help/index_%1.docbook"_L1.arg(locale);
    if (! QFile::exists(m_fileName))
    {
        m_fileName = ":/help/index_%1.docbook"_L1.arg(
                         locale.left(locale.indexOf("_"_L1)));
        if (! QFile::exists(m_fileName))
            m_fileName = ":/help/index.docbook"_L1;
    }
    QFile file(m_fileName);
    if (! file.open(QFile::ReadOnly))
        return;
    QXmlStreamReader reader(&file);
    if (! reader.readNextStartElement() || reader.name() != "book"_L1)
        return;
    m_pageIds.append("index"_L1);
    while (reader.readNextStartElement())
    {
        if (reader.name() == "chapter"_L1)
            m_pageIds.append(reader.attributes().value("id"_L1).toString());
        reader.skipCurrentElement();
    }
    setText();
}

QString DocbookReader::getPage(const QString& id) const
{
    QFile file(m_fileName);
    if (! file.open(QFile::ReadOnly))
        return {};
    QXmlStreamReader reader(&file);
    if (! reader.readNextStartElement() || reader.name() != "book"_L1)
        return {};
    while (reader.readNextStartElement())
    {
        if (reader.name() == "chapter"_L1)
        {
            if (reader.attributes().value("id"_L1) == id)
                break;
            reader.skipCurrentElement();
        }
        else
            reader.skipCurrentElement();
    }
    QString text;
    addHeader(text);
    bool finished = false;
    int headerLevel = 1;
    while (! finished && ! reader.atEnd())
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement:
            if (reader.name() == "guibutton"_L1
                    || reader.name() == "guilabel"_L1
                    || reader.name() == "guimenu"_L1
                    || reader.name() == "guimenuitem"_L1
                    || reader.name() == "keysym"_L1)
                text.append("<i>"_L1);
            else if (reader.name() == "imagedata"_L1)
            {
                text.append("<div style=\"margin-left:"_L1);
                text.append(QString::number((m_textWidth - m_imageWidth) / 2));
                text.append("\"><img src=qrc:/help/"_L1);
                text.append(reader.attributes().value("fileref"_L1));
                text.append(" width="_L1);
                text.append(QString::number(m_imageWidth));
                text.append("></div>"_L1);
            }
            else if (reader.name() == "listitem"_L1)
                text.append("<blockquote>"_L1);
            else if (reader.name() == "para"_L1)
                text.append("<p>"_L1);
            else if (reader.name() == "sect1"_L1)
                ++headerLevel;
            else if (reader.name() == "title"_L1)
            {
                if (headerLevel > 1)
                    text.append("<h2>"_L1);
                else
                    text.append("<h1>"_L1);
            }
            else if (reader.name() == "varlistentry"_L1)
                text.append("<div>"_L1);
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == "guibutton"_L1
                    || reader.name() == "guilabel"_L1
                    || reader.name() == "guimenu"_L1
                    || reader.name() == "guimenuitem"_L1
                    || reader.name() == "keysym"_L1)
                text.append("</i>"_L1);
            else if (reader.name() == "chapter"_L1)
                finished = true;
            else if (reader.name() == "listitem"_L1)
                text.append("</blockquote>"_L1);
            else if (reader.name() == "para"_L1)
                text.append("</p>"_L1);
            else if (reader.name() == "sect1"_L1)
                --headerLevel;
            else if (reader.name() == "title"_L1)
            {
                if (headerLevel > 1)
                    text.append("</h2>"_L1);
                else
                    text.append("</h1>"_L1);
            }
            else if (reader.name() == "varlistentry"_L1)
                text.append("</div>"_L1);
            break;
        case QXmlStreamReader::Characters:
            text.append(reader.text());
            break;
        default:
            break;
        }
    return text;
}

QString DocbookReader::getTableOfContents() const
{
    QFile file(m_fileName);
    if (! file.open(QFile::ReadOnly))
        return {};
    QXmlStreamReader reader(&file);
    if (! reader.readNextStartElement() || reader.name() != "book"_L1)
        return {};
    QString bookTitle;
    QStringList chapterTitles;
    while (reader.readNextStartElement())
    {
        if (reader.name() == "title"_L1)
            bookTitle = reader.readElementText();
        else if (reader.name() == "chapter"_L1)
        {
            QString chapterTitle;
            while (reader.readNextStartElement())
            {
                if (reader.name() == "title"_L1)
                    chapterTitle = reader.readElementText();
                else
                    reader.skipCurrentElement();
            }
            chapterTitles.append(chapterTitle);
        }
        else
            reader.skipCurrentElement();
    }
    QString text;
    addHeader(text);
    text.append("<h1>"_L1);
    text.append(bookTitle);
    text.append("</h1>"_L1);
    for (int i = 0; i < chapterTitles.size(); ++i)
    {
        text.append("<a href="_L1);
        if (i + 1 < m_pageIds.size())
            text.append(m_pageIds[i + 1]);
        else
            LIBBOARDGAME_ASSERT(false);
        text.append(">"_L1);
        text.append(chapterTitles[i]);
        text.append("</a><br/>"_L1);
    }
    return text;
}

void DocbookReader::setNavigation()
{
    qsizetype prevId = -1;
    qsizetype nextId = -1;
    qsizetype i = m_pageIds.indexOf(m_pageId);
    if (i >= 0)
    {
        prevId = i - 1;
        nextId = i + 1;
        if (nextId >= m_pageIds.size())
            nextId = -1;
    }
    QString text;
    addHeader(text);
    text.append("<table width=100%><tr><td width=34%>"_L1);
    if (prevId >= 0)
    {
        text.append("<a href="_L1);
        text.append(m_pageIds[prevId]);
        text.append(">"_L1);
        //: Go to previous page of user manual
        text.append(tr("Previous"));
        text.append("</a>"_L1);
    }
    else
        text.append("&#160;"_L1);
    text.append("</td><td width=32% align=center>"_L1);
    if (m_pageId != "index"_L1)
    {
        text.append("<a href=index>"_L1);
        //: Go to table of contents of user manual
        text.append(tr("Contents"));
        text.append("</a>"_L1);
    }
    text.append("</td><td width=34% align=right>"_L1);
    if (nextId >= 0)
    {
        text.append("<a href="_L1);
        text.append(m_pageIds[nextId]);
        text.append(">"_L1);
        //: Go to next page of user manual
        text.append(tr("Next"));
        text.append("</a>"_L1);
    }
    else
        text.append("&#160;"_L1);
    text.append("</td></tr></table>"_L1);
    m_navigationText = text;
    if (prevId >= 0)
        m_prevPageId = m_pageIds[prevId];
    else
        m_prevPageId.clear();
    if (nextId >= 0)
        m_nextPageId = m_pageIds[nextId];
    else
        m_nextPageId.clear();
    emit navigationTextChanged();
}

void DocbookReader::setPageId(const QString& pageId)
{
    if (m_pageId == pageId)
        return;
    m_pageId = pageId;
    setText();
}

void DocbookReader::setText()
{
    if (m_pageId == "index"_L1)
        m_text = getTableOfContents();
    else
        m_text = getPage(m_pageId);
    emit textChanged();
    setNavigation();
}

void DocbookReader::setTextWidth(qreal textWidth)
{
    if (textWidth == m_textWidth)
        return;
    m_imageWidth = 320;
    if (m_imageWidth > textWidth)
        m_imageWidth = textWidth;
    m_textWidth = textWidth;
    setText();
}

//-----------------------------------------------------------------------------
