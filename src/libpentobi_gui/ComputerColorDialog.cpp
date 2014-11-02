//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.cpp
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ComputerColorDialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

using libpentobi_base::ColorIterator;

//-----------------------------------------------------------------------------

ComputerColorDialog::ComputerColorDialog(QWidget* parent,
                                         Variant variant,
                                         ColorMap<bool>& computerColor)
    : QDialog(parent),
      m_computerColor(computerColor),
      m_variant(variant)
{
    setWindowTitle(tr("Computer Colors"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto layout = new QVBoxLayout;
    setLayout(layout);
    layout->addWidget(new QLabel(tr("Colors played by the computer:")));
    for (Color::IntType i = 0; i < get_nu_players(m_variant); ++i)
        createCheckBox(layout, Color(i));
    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
    buttonBox->setFocus();
}

void ComputerColorDialog::accept()
{
    auto nu_players = get_nu_players(m_variant);
    auto nu_colors = get_nu_colors(m_variant);
    if (nu_players == nu_colors || m_variant == Variant::classic_3)
        for (ColorIterator i(nu_players); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == Variant::classic_2
                            || m_variant == Variant::trigon_2);
        m_computerColor[Color(0)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(2)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(1)] = m_checkBox[1]->isChecked();
        m_computerColor[Color(3)] = m_checkBox[1]->isChecked();
    }
    QDialog::accept();
}

void ComputerColorDialog::createCheckBox(QLayout* layout, Color c)
{
    auto checkBox = new QCheckBox(getPlayerString(m_variant, c));
    checkBox->setChecked(m_computerColor[c]);
    layout->addWidget(checkBox);
    m_checkBox[c.to_int()] = checkBox;
}

QString ComputerColorDialog::getPlayerString(Variant variant, Color c)
{
    auto i = c.to_int();
    if (variant == Variant::duo || variant == Variant::junior)
        return i == 0 ? tr("&Blue") : tr("&Green");
    if (variant == Variant::classic_2 || variant == Variant::trigon_2)
        return i == 0 || i == 2 ? tr("&Blue/Red") : tr("&Yellow/Green");
    if (i == 0)
        return tr("&Blue");
    if (i == 1)
        return tr("&Yellow");
    if (i == 2)
        return tr("&Red");
    return tr("&Green");
}

//-----------------------------------------------------------------------------
