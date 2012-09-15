//-----------------------------------------------------------------------------
/** @file libpentobi_gui/InitialRatingDialog.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_INITIAL_RATING_DIALOG_H
#define LIBPENTOBI_GUI_INITIAL_RATING_DIALOG_H

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
