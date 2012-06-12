//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
#define LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H

#include <array>
#include <QtGui>
#include "libpentobi_base/Variant.h"
#include "libpentobi_base/ColorMap.h"

using namespace std;
using libpentobi_base::Variant;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;

//-----------------------------------------------------------------------------

class ComputerColorDialog
    : public QDialog
{
    Q_OBJECT

public:
    ComputerColorDialog(QWidget* parent, Variant variant,
                        ColorMap<bool>& computerColor);

public slots:
    void accept();

private:
    ColorMap<bool>& m_computerColor;

    Variant m_variant;

    array<QCheckBox*, 4> m_checkBox;

    void createCheckBox(QLayout* layout, Color c);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
