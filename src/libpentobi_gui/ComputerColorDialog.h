//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
#define LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <array>
#include <QCheckBox>
#include <QDialog>
#include "libpentobi_base/Variant.h"
#include "libpentobi_base/ColorMap.h"

using namespace std;
using libpentobi_base::Variant;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;

//-----------------------------------------------------------------------------

class ComputerColorDialog final
    : public QDialog
{
    Q_OBJECT

public:
    ComputerColorDialog(QWidget* parent, Variant variant,
                        ColorMap<bool>& computerColor);

    void accept() override;

private:
    ColorMap<bool>& m_computerColor;

    Variant m_variant;

    array<QCheckBox*, 4> m_checkBox;

    void createCheckBox(QLayout* layout, Color c);

    QString getPlayerString(Color c);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
