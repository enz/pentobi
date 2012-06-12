//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ComputerColorDialog.h"

#include "Util.h"

using libpentobi_base::variant_classic;
using libpentobi_base::variant_classic_2;
using libpentobi_base::variant_duo;
using libpentobi_base::variant_junior;
using libpentobi_base::variant_trigon;
using libpentobi_base::variant_trigon_2;
using libpentobi_base::variant_trigon_3;
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
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(new QLabel(tr("Colors played by the computer:")));
    if (m_variant == variant_duo || m_variant == variant_junior)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
    }
    else if (m_variant == variant_classic || m_variant == variant_trigon)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
        createCheckBox(layout, Color(2));
        createCheckBox(layout, Color(3));
    }
    else if (m_variant == variant_trigon_3)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
        createCheckBox(layout, Color(2));
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == variant_classic_2
                            || m_variant == variant_trigon_2);
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
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
    if (m_variant == variant_duo || m_variant == variant_junior)
    {
        for (ColorIterator i(2); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    }
    else if (m_variant == variant_classic || m_variant == variant_trigon)
    {
        for (ColorIterator i(4); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    }
    else if (m_variant == variant_trigon_3)
    {
        m_computerColor[Color(0)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(2)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(1)] = m_checkBox[1]->isChecked();
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == variant_classic_2
                            || m_variant == variant_trigon_2);
        m_computerColor[Color(0)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(2)] = m_checkBox[0]->isChecked();
        m_computerColor[Color(1)] = m_checkBox[1]->isChecked();
        m_computerColor[Color(3)] = m_checkBox[1]->isChecked();
    }
    QDialog::accept();
}

void ComputerColorDialog::createCheckBox(QLayout* layout, Color c)
{
    QCheckBox* checkBox =
        new QCheckBox(Util::getPlayerString(m_variant, c));
    checkBox->setChecked(m_computerColor[c]);
    layout->addWidget(checkBox);
    m_checkBox[c.to_int()] = checkBox;
}

//-----------------------------------------------------------------------------
