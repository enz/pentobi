//-----------------------------------------------------------------------------
/** @file pentobi/ImageProvider.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_IMAGE_PROVIDER_H
#define PENTOBI_IMAGE_PROVIDER_H

// Needed in the header because moc_*.cpp does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QQuickImageProvider>

//-----------------------------------------------------------------------------

class ImageProvider
    : public QQuickImageProvider
{
public:
    ImageProvider();

    QPixmap requestPixmap(const QString& id, QSize* size,
                          const QSize& requestedSize) override;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_IMAGE_PROVIDER_H
