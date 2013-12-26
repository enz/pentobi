//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GameInfoDialog.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef LIBPENTOBI_GUI_GAME_INFO_DIALOG_H
#define LIBPENTOBI_GUI_GAME_INFO_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
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

    QLineEdit* m_event;

    QLineEdit* m_round;

    QLineEdit* m_time;

    bool acceptLine(QLineEdit* lineEdit, string& value);

    QLineEdit* createLine(const QString& label, const string& text);

    QLineEdit* createPlayerName(const QString& label, Color c);
};

//-----------------------------------------------------------------------------

#endif // LIBPENTOBI_GUI_GAME_INFO_DIALOG_H
