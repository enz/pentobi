//-----------------------------------------------------------------------------
/** @file libpentobi_gui/InitialRatingDialog.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "InitialRatingDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------

InitialRatingDialog::InitialRatingDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Initial Rating"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto layout = new QVBoxLayout;
    setLayout(layout);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    auto label =
        new QLabel(tr("You have not yet played rated games in this game"
                      " variant. Estimate your playing strength to"
                      " initialize your rating."));
    label->setWordWrap(true);
    layout->addWidget(label);
    auto sliderBoxLayout = new QHBoxLayout;
    layout->addLayout(sliderBoxLayout);
    sliderBoxLayout->addWidget(new QLabel(tr("Beginner")));
    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setMinimum(1000);
    m_slider->setMaximum(2000);
    m_slider->setSingleStep(10);
    m_slider->setPageStep(100);
    sliderBoxLayout->addWidget(m_slider);
    sliderBoxLayout->addWidget(new QLabel(tr("Expert")));
    m_ratingLabel = new QLabel;
    layout->addWidget(m_ratingLabel);
    setRating(1000);
    connect(m_slider, &QSlider::valueChanged, this, &InitialRatingDialog::setRating);
    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &InitialRatingDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &InitialRatingDialog::reject);
}

void InitialRatingDialog::setRating(int rating)
{
    m_rating = rating;
    m_ratingLabel->setText(tr("Your initial rating: %1").arg(rating));
}

//-----------------------------------------------------------------------------
