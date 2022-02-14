//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "DocbookReader.h"

#include <cmath>
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
    text.append(QStringLiteral(
                    "<head><style>"
                    "body { background-color:white;color:black;"
                    "       line-height:115% }"
                    ":link { text-decoration:none;color:blue }"
                    "</style></head><body>"));
}

} //namespace

//-----------------------------------------------------------------------------

DocbookReader::DocbookReader(QObject* parent)
    : QObject(parent)
{

    QString locale = QLocale::system().name();
    m_fileName = QStringLiteral(":/help/index_%1.docbook").arg(locale);
    if (! QFile::exists(m_fileName))
    {
        m_fileName = QStringLiteral(":/help/index_%1.docbook").arg(
                    locale.left(locale.indexOf(QStringLiteral("_"))));
        if (! QFile::exists(m_fileName))
            m_fileName = QStringLiteral(":/help/index.docbook");
    }
    QFile file(m_fileName);
    if (! file.open(QFile::ReadOnly))
        return;
    QXmlStreamReader reader(&file);
    if (! reader.readNextStartElement()
            || reader.name() != QStringLiteral("book"))
        return;
    m_pageIds.append(QStringLiteral("index"));
    while (reader.readNextStartElement())
    {
        if (reader.name() == QStringLiteral("chapter"))
            m_pageIds.append(
                        reader.attributes().value(
                            QStringLiteral("id")).toString());
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
    if (! reader.readNextStartElement()
            || reader.name() != QStringLiteral("book"))
        return {};
    while (reader.readNextStartElement())
    {
        if (reader.name() == QStringLiteral("chapter"))
        {
            if (reader.attributes().value(QStringLiteral("id")) == id)
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
            if (reader.name() == QStringLiteral("guibutton")
                    || reader.name() == QStringLiteral("guilabel")
                    || reader.name() == QStringLiteral("guimenu")
                    || reader.name() == QStringLiteral("guimenuitem")
                    || reader.name() == QStringLiteral("keysym"))
                text.append(QStringLiteral("<i>"));
            else if (reader.name() == QStringLiteral("imagedata"))
            {
                text.append(QStringLiteral("<div style=\"margin-left:"));
                text.append(QString::number((m_textWidth - m_imageWidth) / 2));
                text.append(QStringLiteral("\"><img src=qrc:/help/"));
                text.append(reader.attributes().value(
                                QStringLiteral("fileref")));
                text.append(QStringLiteral(" width="));
                text.append(QString::number(m_imageWidth));
                text.append(QStringLiteral("></div>"));
            }
            else if (reader.name() == QStringLiteral("listitem"))
                text.append(QStringLiteral("<blockquote>"));
            else if (reader.name() == QStringLiteral("para"))
                text.append(QStringLiteral("<p>"));
            else if (reader.name() == QStringLiteral("sect1"))
                ++headerLevel;
            else if (reader.name() == QStringLiteral("title"))
            {
                if (headerLevel > 1)
                    text.append(QStringLiteral("<h2>"));
                else
                    text.append(QStringLiteral("<h1>"));
            }
            else if (reader.name() == QStringLiteral("varlistentry"))
                text.append(QStringLiteral("<div>"));
            break;
        case QXmlStreamReader::EndElement:
            if (reader.name() == QStringLiteral("guibutton")
                    || reader.name() == QStringLiteral("guilabel")
                    || reader.name() == QStringLiteral("guimenu")
                    || reader.name() == QStringLiteral("guimenuitem")
                    || reader.name() == QStringLiteral("keysym"))
                text.append(QStringLiteral("</i>"));
            else if (reader.name() == QStringLiteral("chapter"))
                finished = true;
            else if (reader.name() == QStringLiteral("listitem"))
                text.append(QStringLiteral("</blockquote>"));
            else if (reader.name() == QStringLiteral("para"))
                text.append(QStringLiteral("</p>"));
            else if (reader.name() == QStringLiteral("sect1"))
                --headerLevel;
            else if (reader.name() == QStringLiteral("title"))
            {
                if (headerLevel > 1)
                    text.append(QStringLiteral("</h2>"));
                else
                    text.append(QStringLiteral("</h1>"));
            }
            else if (reader.name() == QStringLiteral("varlistentry"))
                text.append(QStringLiteral("</div>"));
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
    if (! reader.readNextStartElement()
            || reader.name() != QStringLiteral("book"))
        return {};
    QString bookTitle;
    QStringList chapterTitles;
    while (reader.readNextStartElement())
    {
        if (reader.name() == QStringLiteral("title"))
            bookTitle = reader.readElementText();
        else if (reader.name() == QStringLiteral("chapter"))
        {
            QString chapterTitle;
            while (reader.readNextStartElement())
            {
                if (reader.name() == QStringLiteral("title"))
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
    text.append(QStringLiteral("<h1>"));
    text.append(bookTitle);
    text.append(QStringLiteral("</h1>"));
    for (int i = 0; i < chapterTitles.size(); ++i)
    {
        text.append(QStringLiteral("<a href="));
        if (i + 1 < m_pageIds.size())
            text.append(m_pageIds[i + 1]);
        else
            LIBBOARDGAME_ASSERT(false);
        text.append(QStringLiteral(">"));
        text.append(chapterTitles[i]);
        text.append(QStringLiteral("</a><br/>"));
    }
    return text;
}

void DocbookReader::setNavigation()
{
    int prevId = -1;
    int nextId = -1;
    int i = m_pageIds.indexOf(m_pageId);
    if (i >= 0)
    {
        prevId = i - 1;
        nextId = i + 1;
        if (nextId >= m_pageIds.size())
            nextId = -1;
    }
    QString text;
    addHeader(text);
    text.append(QStringLiteral("<table width=100%><tr><td width=34%>"));
    if (prevId >= 0)
    {
        text.append(QStringLiteral("<a href="));
        text.append(m_pageIds[prevId]);
        text.append(QStringLiteral(">"));
        //: Go to previous page of user manual
        text.append(tr("Previous"));
        text.append(QStringLiteral("</a>"));
    }
    else
        text.append(QStringLiteral("&#160;"));
    text.append(QStringLiteral("</td><td width=32% align=center>"));
    if (m_pageId != QStringLiteral("index"))
    {
        text.append(QStringLiteral("<a href=index>"));
        //: Go to table of contents of user manual
        text.append(tr("Contents"));
        text.append(QStringLiteral("</a>"));
    }
    text.append(QStringLiteral("</td><td width=34% align=right>"));
    if (nextId >= 0)
    {
        text.append(QStringLiteral("<a href="));
        text.append(m_pageIds[nextId]);
        text.append(QStringLiteral(">"));
        //: Go to next page of user manual
        text.append(tr("Next"));
        text.append(QStringLiteral("</a>"));
    }
    else
        text.append(QStringLiteral("&#160;"));
    text.append(QStringLiteral("</td></tr></table>"));
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
    if (m_pageId == QStringLiteral("index"))
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
