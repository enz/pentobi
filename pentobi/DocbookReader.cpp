//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#include "DocbookReader.h"

#include <QFile>
#include <QLocale>
#include <QStack>

using namespace std;

//-----------------------------------------------------------------------------

DocbookReader::DocbookReader(QObject* parent)
    : QObject(parent)
{
    m_locale = QLocale::system().name();
    m_lang = m_locale.left(m_locale.indexOf("_"_L1));
    QString fileName = ":/help/index_%1.docbook"_L1.arg(m_locale);
    if (! QFile::exists(fileName))
    {
        fileName = ":/help/index_%1.docbook"_L1.arg(m_lang);
        if (! QFile::exists(fileName))
            fileName = ":/help/index.docbook"_L1;
    }
    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly) || ! m_doc.setContent(&file))
        return;
    m_pageIds.append("index"_L1);
    QDomElement elem = m_doc.documentElement().firstChildElement();
    while(! elem.isNull())
    {
        if (elem.tagName() == "chapter"_L1)
            m_pageIds.append(elem.attribute("id"_L1));
        elem = elem.nextSiblingElement();
    }
    setText();
}

void DocbookReader::addHeader(QString& text) const
{
    // Maybe explicitly setting the text/background colors can be avoided in
    // the future but presently TextArea doesn't fully support the palette in
    // Basic style dark mode
    text.append("<head><style>body{background-color:");
    text.append(m_colorBackground.name());
    text.append(";color:");
    text.append(m_colorText.name());
    text.append(";line-height:115%}:link{text-decoration:none;color:");
    text.append(m_colorLink.name());
    text.append("}</style></head><body>"_L1);
}

void DocbookReader::addNavigation(QString& text)
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
    addHeader(text);
    text.append("<p width=100% align=right>"_L1);
    if (prevId >= 0)
    {
        text.append("<a href="_L1);
        text.append(m_pageIds[prevId]);
        text.append(">"_L1);
        //: Go to previous page of user manual
        text.append(tr("Previous"));
        text.append("</a> | "_L1);
    }
    if (m_pageId != "index"_L1)
    {
        text.append("<a href=index>"_L1);
        //: Go to table of contents of user manual
        text.append(tr("Contents"));
        text.append("</a> | "_L1);
    }
    if (nextId >= 0)
    {
        text.append("<a href="_L1);
        text.append(m_pageIds[nextId]);
        text.append(">"_L1);
        //: Go to next page of user manual
        text.append(tr("Next"));
        text.append("</a> | "_L1);
    }
    text.append("<a href=close>"_L1);
    text.append(tr("Close"));
    text.append("</a></p>"_L1);
    if (prevId >= 0)
        m_prevPageId = m_pageIds[prevId];
    else
        m_prevPageId.clear();
    if (nextId >= 0)
        m_nextPageId = m_pageIds[nextId];
    else
        m_nextPageId.clear();
}

void DocbookReader::addPage(const QString& id, QString& text)
{
    QDomElement elem = m_doc.documentElement().firstChildElement();
    while(! elem.isNull())
    {
        if (elem.tagName() == "chapter"_L1 && elem.attribute("id"_L1) == id)
        {
            handleNode(elem, 1, text);
            break;
        }
        elem = elem.nextSiblingElement();
    }
}

void DocbookReader::addTableOfContents(QString& text)
{
    QString bookTitle;
    QStringList chapterTitles;
    QDomElement elem = m_doc.documentElement().firstChildElement();
    while(! elem.isNull())
    {
        if (! elem.hasAttribute("xml:lang"_L1))
        {
            if (elem.tagName() == "title"_L1)
            {
                elem = findLocalized(elem);
                bookTitle = elem.text();
            }
            else if (elem.tagName() == "chapter"_L1)
            {
                QDomElement childElem = elem.firstChildElement();
                while (! childElem.isNull())
                {
                    if (childElem.tagName() == "title"_L1)
                    {
                        childElem = findLocalized(childElem);
                        chapterTitles.append(childElem.text());
                        break;
                    }
                    childElem = childElem.nextSiblingElement();
                }
            }
        }
        elem = elem.nextSiblingElement();
    }
    addHeader(text);
    text.append("<h1>"_L1);
    text.append(bookTitle);
    text.append("</h1>"_L1);
    for (int i = 0; i < chapterTitles.size(); ++i)
    {
        text.append("<a href="_L1);
        if (i + 1 < m_pageIds.size())
            text.append(m_pageIds[i + 1]);
        text.append(">"_L1);
        text.append(chapterTitles[i]);
        text.append("</a><br/>"_L1);
    }
}

QDomElement DocbookReader::findLocalized(const QDomElement& elem) const
{
    QString name = elem.tagName();
    QDomElement result = elem;
    QDomElement e = elem;
    while (true)
    {
        e = e.nextSiblingElement();
        if (e.isNull() || e.tagName() != name)
            break;
        QString l = e.attribute("xml:lang"_L1);
        if (l.isEmpty())
            break;
        if (l == m_locale)
        {
            result = e;
            break;
        }
        if (l == m_lang)
            result = e;
    }
    return result;
}

void DocbookReader::handleChildren(const QDomNode& node, int headerLevel,
                                   QString& text) const
{
    QDomNode child = node.firstChild();
    while (! child.isNull())
    {
        child = handleNode(child, headerLevel, text);
        child = child.nextSibling();
    }
}

QDomNode DocbookReader::handleNode(const QDomNode& node, int headerLevel,
                                   QString& text) const
{
    if (node.isText())
    {
        text.append(node.toText().data());
        return node;
    }
    if (! node.isElement())
    {
        handleChildren(node, headerLevel, text);
        return node;
    }
    QDomElement elem = node.toElement();
    if (elem.hasAttribute("xml:lang"_L1))
        return node;
    QString name = elem.tagName();
    if (name == "guibutton"_L1
        || name == "guilabel"_L1
        || name == "guimenu"_L1
        || name == "guimenuitem"_L1
        || name == "keysym"_L1)
    {
        text.append("<i>"_L1);
        handleChildren(elem, headerLevel, text);
        text.append("</i>"_L1);
        return node;
    }
    if (name == "imagedata"_L1)
    {
        text.append("<div style=\"margin-left:"_L1);
        text.append(QString::number((m_textWidth - m_imageWidth) / 2));
        text.append("\"><img src=qrc:/help/"_L1);
        text.append(elem.attribute("fileref"_L1));
        text.append(" width="_L1);
        text.append(QString::number(m_imageWidth));
        text.append("></div>"_L1);
    }
    if (name == "listitem"_L1)
    {
        text.append("<blockquote>"_L1);
        handleChildren(elem, headerLevel, text);
        text.append("</blockquote>"_L1);
        return node;
    }
    if (name == "para"_L1)
    {
        text.append("<p>"_L1);
        QDomElement localizedElem = findLocalized(elem);
        handleChildren(localizedElem, headerLevel, text);
        text.append("</p>"_L1);
        return localizedElem;
    }
    if (name == "sect1"_L1)
    {
        ++headerLevel;
        handleChildren(elem, headerLevel, text);
        return node;
    }
    if (name == "term"_L1)
    {
        handleChildren(elem, headerLevel, text);
        text.append("\n"_L1);
        return node;
    }
    if (name == "title"_L1)
    {
        text.append(headerLevel > 1 ? "<h2>"_L1 : "<h1>"_L1);
        QDomElement localizedElem = findLocalized(elem);
        handleChildren(localizedElem, headerLevel, text);
        text.append(headerLevel > 1 ? "</h2>"_L1 : "</h1>"_L1);
        return localizedElem;
    }
    if (name == "varlistentry"_L1)
    {
        text.append("<div>"_L1);
        handleChildren(elem, headerLevel, text);
        text.append("</div>"_L1);
        return node;
    }
    handleChildren(elem, headerLevel, text);
    return node;
}

void DocbookReader::setColorBackground(const QColor& color)
{
    m_colorBackground = color;
    setText();
}

void DocbookReader::setColorLink(const QColor& color)
{
    m_colorLink = color;
    setText();
}

void DocbookReader::setColorText(const QColor& color)
{
    m_colorText = color;
    setText();
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
    m_text.clear();
    addHeader(m_text);
    addNavigation(m_text);
    m_text.append("<p>"_L1);
    if (m_pageId == "index"_L1)
        addTableOfContents(m_text);
    else
        addPage(m_pageId, m_text);
    emit textChanged();
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
