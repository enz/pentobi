//-----------------------------------------------------------------------------
/** @file libpentobi_gui/ComputerColorDialog.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
#define LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H

#include <array>
#include <QtGui>
#include "libpentobi_base/GameVariant.h"
#include "libpentobi_base/ColorMap.h"

using namespace std;
using libpentobi_base::GameVariant;
using libpentobi_base::Color;
using libpentobi_base::ColorMap;

//-----------------------------------------------------------------------------

class ComputerColorDialog
    : public QDialog
{
    Q_OBJECT

public:
    ComputerColorDialog(QWidget* parent, GameVariant gameVariant,
                        ColorMap<bool>& computerColor);

public slots:
    void accept();

private:
    ColorMap<bool>& m_computerColor;

    GameVariant m_gameVariant;

    array<QCheckBox*, 4> m_checkBox;

    void createCheckBox(QLayout* layout, Color c, const QString& text);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_COMPUTER_COLOR_DIALOG_H
