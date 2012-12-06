//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.cpp */
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
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    layout->addWidget(new QLabel(tr("Colors played by the computer:")));
    if (m_variant == Variant::duo || m_variant == Variant::junior)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
    }
    else if (m_variant == Variant::classic || m_variant == Variant::trigon)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
        createCheckBox(layout, Color(2));
        createCheckBox(layout, Color(3));
    }
    else if (m_variant == Variant::trigon_3)
    {
        createCheckBox(layout, Color(0));
        createCheckBox(layout, Color(1));
        createCheckBox(layout, Color(2));
    }
    else
    {
        LIBBOARDGAME_ASSERT(m_variant == Variant::classic_2
                            || m_variant == Variant::trigon_2);
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
    unsigned nu_colors = get_nu_colors(m_variant);
    if (get_nu_players(m_variant) == nu_colors)
    {
        for (ColorIterator i(nu_colors); i; ++i)
            m_computerColor[*i] = m_checkBox[(*i).to_int()]->isChecked();
    }
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
    QCheckBox* checkBox = new QCheckBox(getPlayerString(m_variant, c));
    checkBox->setChecked(m_computerColor[c]);
    layout->addWidget(checkBox);
    m_checkBox[c.to_int()] = checkBox;
}

QString ComputerColorDialog::getPlayerString(Variant variant, Color c)
{
    if (variant == Variant::duo || variant == Variant::junior)
    {
        if (c == Color(0))
            return tr("&Blue");
        if (c == Color(1))
            return tr("&Green");
    }
    else if (variant == Variant::classic || variant == Variant::trigon)
    {
        if (c == Color(0))
            return tr("&Blue");
        if (c == Color(1))
            return tr("&Yellow");
        if (c == Color(2))
            return tr("&Red");
        if (c == Color(3))
            return tr("&Green");
    }
    else if (variant == Variant::trigon_3)
    {
        if (c == Color(0))
            return tr("&Blue");
        if (c == Color(1))
            return tr("&Yellow");
        if (c == Color(2))
            return tr("&Red");
    }
    else if (variant == Variant::classic_2 || variant == Variant::trigon_2)
    {
        if (c == Color(0) || c == Color(2))
            return tr("&Blue/Red");
        if (c == Color(1) || c == Color(3))
            return tr("&Yellow/Green");
    }
    LIBBOARDGAME_ASSERT(false);
    return "";
}

//-----------------------------------------------------------------------------
