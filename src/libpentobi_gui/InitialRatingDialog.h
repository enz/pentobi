//-----------------------------------------------------------------------------
/** @file libpentobi_gui/InitialRatingDialog.h
    @author Markus Enzenberger <enz@users.sourceforge.net>
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_INITIAL_RATING_DIALOG_H
#define LIBPENTOBI_GUI_INITIAL_RATING_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QDialog>

class QLabel;
class QSlider;

using namespace std;

//-----------------------------------------------------------------------------

/** Dialog that asks the user to estimate his initial rating. */
class InitialRatingDialog
    : public QDialog
{
    Q_OBJECT

public:
    InitialRatingDialog(QWidget* parent);

    int getRating() const;

public slots:
    void setRating(int rating);

private:
    int m_rating;

    QSlider* m_slider;

    QLabel* m_ratingLabel;
};

inline int InitialRatingDialog::getRating() const
{
    return m_rating;
}

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_INITIAL_RATING_DIALOG_H
