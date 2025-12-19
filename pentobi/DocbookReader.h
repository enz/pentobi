//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_DOCBOOK_READER_H
#define PENTOBI_DOCBOOK_READER_H

#include <QColor>
#include <QObject>
#include <QDomDocument>
#include <QtQml/qqmlregistration.h>

using namespace Qt::StringLiterals;

//-----------------------------------------------------------------------------

/** Converts help in Docbook format into Qt rich text.
    This class parses Docbook files included in the resources and converts them
    into the subset of HTML supported by TextEdit.RichText. */
class DocbookReader
    : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QColor colorBackground MEMBER m_colorBackground WRITE setColorBackground)
    Q_PROPERTY(QColor colorLink MEMBER m_colorLink WRITE setColorLink)
    Q_PROPERTY(QColor colorText MEMBER m_colorText WRITE setColorText)
    Q_PROPERTY(QString pageId MEMBER m_pageId WRITE setPageId)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    // DocbookReader needs to know the width of the text area to scale and
    // center the images (center tag/attribute doesn't work in TextEdit)
    Q_PROPERTY(qreal textWidth MEMBER m_textWidth WRITE setTextWidth)

public:
    DocbookReader(QObject* parent = nullptr);

    void setPageId(const QString& pageId);

    const QString& text() const { return m_text; }

signals:
    void navigationTextChanged();
    void textChanged();

private:
    qreal m_textWidth = 0;

    qreal m_imageWidth = 0;

    QColor m_colorBackground = "white";

    QColor m_colorLink = "blue";

    QColor m_colorText = "black";

    QString m_pageId = {"index"_L1};

    QString m_text;

    QString m_nextPageId;

    QString m_prevPageId;

    QString m_navigationText;

    QString m_locale;

    QString m_lang;

    QStringList m_pageIds;

    QDomDocument m_doc;


    void addHeader(QString& text) const;

    void addNavigation(QString& text);

    void addPage(const QString& id, QString& text);

    void addTableOfContents(QString& text);

    void handleChildren(const QDomNode& node, int headerLevel,
                        QString& text) const;

    QDomNode handleNode(const QDomNode& node, int headerLevel,
                        QString& text) const;

    void setColorBackground(const QColor& color);

    void setColorLink(const QColor& color);

    void setColorText(const QColor& color);

    void setText();

    void setTextWidth(qreal textWidth);
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_DOCBOOK_READER_H
