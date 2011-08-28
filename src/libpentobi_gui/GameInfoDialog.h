//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GameInfoDialog.h */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_GAME_INFO_DIALOG_H
#define LIBPENTOBI_GUI_GAME_INFO_DIALOG_H

#include <QtGui>
#include "libpentobi_base/Game.h"

using namespace std;
using libpentobi_base::Color;
using libpentobi_base::Game;

//-----------------------------------------------------------------------------

class GameInfoDialog
    : public QDialog
{
    Q_OBJECT

public:
    GameInfoDialog(QWidget* parent, Game& game);

public slots:
    void accept();

private:
    Game& m_game;

    string m_charset;

    QFormLayout* m_formLayout;

    QLineEdit* m_playerBlue;

    QLineEdit* m_playerYellow;

    QLineEdit* m_playerRed;

    QLineEdit* m_playerGreen;

    QLineEdit* m_playerBlueRed;

    QLineEdit* m_playerYellowGreen;

    QLineEdit* m_date;

    bool acceptLine(QLineEdit* lineEdit, string& value);

    QLineEdit* createLine(const QString& label, const string& text);

    QLineEdit* createPlayerName(const QString& label, Color c);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_GAME_INFO_DIALOG_H
