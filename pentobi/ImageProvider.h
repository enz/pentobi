//-----------------------------------------------------------------------------
/** @file pentobi/ImageProvider.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_IMAGE_PROVIDER_H
#define PENTOBI_IMAGE_PROVIDER_H

#include <QQuickImageProvider>

//-----------------------------------------------------------------------------

class ImageProvider
    : public QQuickImageProvider
{
    Q_OBJECT

public:
    ImageProvider();

    QPixmap requestPixmap(const QString& id, QSize* size,
                          const QSize& requestedSize) override;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_IMAGE_PROVIDER_H
