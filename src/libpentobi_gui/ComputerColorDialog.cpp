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
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(new QLabel(tr("Computer plays:")));
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
    auto nuPlayers = get_nu_players(m_variant);
    auto nuColors = get_nu_colors(m_variant);
    if (nuPlayers == nuColors || m_variant == Variant::classic_3)
        for (Color c : Color::Range(nuPlayers))
            m_computerColor[c] = m_checkBox[c.to_int()]->isChecked();
    else
    {
        LIBBOARDGAME_ASSERT(nuPlayers == 2 && nuColors == 4);
        m_computerColor[Color(0)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(2)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(1)] = m_checkBox[1]->isChecked();
        m_computerColor[Color(3)] = m_checkBox[1]->isChecked();
    }
    QDialog::accept();
}

void ComputerColorDialog::createCheckBox(QLayout* layout, Color c)
{
    auto checkBox = new QCheckBox(getPlayerString(c));
    checkBox->setChecked(m_computerColor[c]);
    layout->addWidget(checkBox);
    m_checkBox[c.to_int()] = checkBox;
}

QString ComputerColorDialog::getPlayerString(Color c)
{
    auto nuPlayers = get_nu_players(m_variant);
    auto nuColors = get_nu_colors(m_variant);
    auto i = c.to_int();
    if (nuPlayers == 2 && nuColors == 4)
        return i == 0 || i == 2 ? tr("&Blue/Red") : tr("&Yellow/Green");
    if (i == 0)
        return tr("&Blue");
    if (i == 1)
        return nuColors == 2 ? tr("&Green") : tr("&Yellow");
    if (i == 2)
        return tr("&Red");
    LIBBOARDGAME_ASSERT(i == 3);
    return tr("&Green");
}

//-----------------------------------------------------------------------------
