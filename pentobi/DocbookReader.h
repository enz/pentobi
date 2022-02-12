//-----------------------------------------------------------------------------
/** @file pentobi/DocbookReader.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_DOCBOOK_READER_H
#define PENTOBI_DOCBOOK_READER_H

#include <QObject>

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

    Q_PROPERTY(QString pageId MEMBER m_pageId WRITE setPageId NOTIFY pageIdChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString navigationText READ navigationText NOTIFY navigationTextChanged)
    Q_PROPERTY(qreal textWidth MEMBER m_textWidth WRITE setTextWidth NOTIFY textWidthChanged)

public:
    DocbookReader(QObject* parent = nullptr);

    void setPageId(const QString& pageId);

    const QString& navigationText() const { return m_navigationText; }

    const QString& text() const { return m_text; }

    void setTextWidth(qreal textWidth);

signals:
    void navigationTextChanged();
    void pageIdChanged();
    void textChanged();
    void textWidthChanged();

private:
    qreal m_textWidth = 0;

    qreal m_imageWidth = 0;

    QString m_pageId = {QStringLiteral("index")};

    QString m_text;

    QString m_navigationText;

    QString m_fileName;

    QStringList m_pageIds;


    QString getNavigationText() const;

    QString getPage(const QString& id) const;

    QString getTableOfContents() const;

    void setText();
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_DOCBOOK_READER_H