//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_DOCBOOK_READER_H
#define PENTOBI_DOCBOOK_READER_H

#include <QObject>
#include <QDomDocument>
#include <QtQml/qqmlregistration.h>

class QFile;

using namespace Qt::StringLiterals;

//-----------------------------------------------------------------------------

/** Converts help in Docbook format into Qt rich text.
    This avoids using an external browser, which did not work with all
    installation directories on Ubuntu 21.10 because Firefox and Chromium have
    restricted access to local files, or using WebView, which is not supported
    on all Linux architectures or in Qt 6.0/6.1. This class parses Docbook
    files included in the resources and converts them into the subset of HTML
    supported by TextEdit.RichText. */
class DocbookReader
    : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString pageId MEMBER m_pageId WRITE setPageId)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString navigationText READ navigationText NOTIFY navigationTextChanged)
    Q_PROPERTY(qreal textWidth MEMBER m_textWidth WRITE setTextWidth)

public:
    DocbookReader(QObject* parent = nullptr);

    Q_INVOKABLE QString getNextPageId() const { return m_nextPageId; }

    Q_INVOKABLE QString getPrevPageId() const { return m_prevPageId; }

    void setPageId(const QString& pageId);

    const QString& navigationText() const { return m_navigationText; }

    const QString& text() const { return m_text; }

    void setTextWidth(qreal textWidth);

signals:
    void navigationTextChanged();
    void textChanged();

private:
    qreal m_textWidth = 0;

    qreal m_imageWidth = 0;

    QString m_pageId = {"index"_L1};

    QString m_text;

    QString m_nextPageId;

    QString m_prevPageId;

    QString m_navigationText;

    QString m_locale;

    QString m_lang;

    QStringList m_pageIds;

    QDomDocument m_doc;


    QDomElement findLocalized(const QDomElement& elem) const;

    QString getPage(const QString& id) const;

    QString getTableOfContents() const;

    void handleChildren(const QDomNode& node, int headerLevel,
                        QString& text) const;

    QDomNode handleNode(const QDomNode& node, int headerLevel,
                        QString& text) const;

    void setNavigation();

    void setText();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_DOCBOOK_READER_H
