//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ComputerColorDialog.h"

using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::ColorIterator;

//-----------------------------------------------------------------------------

ComputerColorDialog::ComputerColorDialog(QWidget* parent,
                                         GameVariant gameVariant,
                                         ColorMap<bool>& computerColor)
    : QDialog(parent),
      m_computerColor(computerColor),
      m_gameVariant(gameVariant)
{
    setWindowTitle(tr("Computer Color"));
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    if (m_gameVariant == game_variant_duo)
    {
        createCheckBox(layout, Color(0), tr("Blue"));
        createCheckBox(layout, Color(1), tr("Green"));
    }
    else if (m_gameVariant == game_variant_classic)
    {
        createCheckBox(layout, Color(0), tr("Blue"));
        createCheckBox(layout, Color(1), tr("Yellow"));
        createCheckBox(layout, Color(2), tr("Red"));
        createCheckBox(layout, Color(3), tr("Green"));
    }
    else
    {
        createCheckBox(layout, Color(0), tr("Blue/Red"));
        createCheckBox(layout, Color(1), tr("Yellow/Green"));
    }
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    buttonBox->setFocus();
}

void ComputerColorDialog::accept()
{
    if (m_gameVariant == game_variant_duo)
    {
        for (ColorIterator i(2); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    }
    else if (m_gameVariant == game_variant_classic)
    {
        for (ColorIterator i(4); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_gameVariant == game_variant_classic_2);
        m_computerColor[Color(0)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(2)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(1)] = m_checkBox[1]->isChecked();
        m_computerColor[Color(3)] = m_checkBox[1]->isChecked();
    }
    QDialog::accept();
}

void ComputerColorDialog::createCheckBox(QLayout* layout, Color c,
                                         const QString& text)
{
    QCheckBox* checkBox = new QCheckBox(text);
    checkBox->setChecked(m_computerColor[c]);
    layout->addWidget(checkBox);
    m_checkBox[c.to_int()] = checkBox;
}

//-----------------------------------------------------------------------------
