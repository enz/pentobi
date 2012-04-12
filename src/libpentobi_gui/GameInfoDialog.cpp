//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GameInfoDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GameInfoDialog.h"

#include "LineEdit.h"
#include "libpentobi_gui/Util.h"

using libpentobi_base::game_variant_classic;
using libpentobi_base::game_variant_classic_2;
using libpentobi_base::game_variant_duo;
using libpentobi_base::game_variant_junior;
using libpentobi_base::game_variant_trigon;
using libpentobi_base::game_variant_trigon_2;
using libpentobi_base::game_variant_trigon_3;
using libpentobi_base::GameVariant;

//-----------------------------------------------------------------------------

GameInfoDialog::GameInfoDialog(QWidget* parent, Game& game)
    : QDialog(parent),
      m_game(game)
{
    m_charset = game.get_root().get_property("CA", "");
    setWindowTitle(tr("Game Info"));
    // Disable '?' button in title bar on Windows, we don't have
    // context help
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout(layout);
    m_formLayout = new QFormLayout();
    QWidget* formWidget = new QWidget();
    formWidget->setLayout(m_formLayout);
    layout->addWidget(formWidget);
    GameVariant variant = game.get_game_variant();
    if (variant == game_variant_duo || variant == game_variant_junior)
    {
        m_playerBlue = createPlayerName(tr("Player Blue:"), Color(0));
        m_playerGreen = createPlayerName(tr("Player Green:"), Color(1));
    }
    else if (variant == game_variant_classic
             || variant == game_variant_trigon)
    {
        m_playerBlue = createPlayerName(tr("Player Blue:"), Color(0));
        m_playerYellow = createPlayerName(tr("Player Yellow:"), Color(1));
        m_playerRed = createPlayerName(tr("Player Red:"), Color(2));
        m_playerGreen = createPlayerName(tr("Player Green:"), Color(3));
    }
    else if (variant == game_variant_trigon_3)
    {
        m_playerBlue = createPlayerName(tr("Player Blue:"), Color(0));
        m_playerYellow = createPlayerName(tr("Player Yellow:"), Color(1));
        m_playerRed = createPlayerName(tr("Player Red:"), Color(2));
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == game_variant_classic_2
                            || variant == game_variant_trigon_2);
        m_playerBlueRed = createPlayerName(tr("Player Blue/Red:"), Color(0));
        m_playerYellowGreen =
            createPlayerName(tr("Player Yellow/Green:"), Color(1));
    }
    m_date = createLine(tr("Date:"), m_game.get_date());
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void GameInfoDialog::accept()
{
    GameVariant variant = m_game.get_game_variant();
    string value;
    if (variant == game_variant_duo
        || variant == game_variant_junior)
    {
        if (acceptLine(m_playerBlue, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerGreen, value))
            m_game.set_player_name(Color(1), value);
    }
    else if (variant == game_variant_classic
             || variant == game_variant_trigon)
    {
        if (acceptLine(m_playerBlue, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerYellow, value))
            m_game.set_player_name(Color(1), value);
        if (acceptLine(m_playerRed, value))
            m_game.set_player_name(Color(2), value);
        if (acceptLine(m_playerGreen, value))
            m_game.set_player_name(Color(3), value);
    }
    else if (variant == game_variant_trigon_3)
    {
        if (acceptLine(m_playerBlue, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerYellow, value))
            m_game.set_player_name(Color(1), value);
        if (acceptLine(m_playerRed, value))
            m_game.set_player_name(Color(2), value);
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == game_variant_classic_2
                            || variant == game_variant_trigon_2);
        if (acceptLine(m_playerBlueRed, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerYellowGreen, value))
            m_game.set_player_name(Color(1), value);
    }
    if (acceptLine(m_date, value))
        m_game.set_date(value);
    QDialog::accept();
}

bool GameInfoDialog::acceptLine(QLineEdit* lineEdit, string& value)
{
    if (! lineEdit->isModified())
        return false;
    QString text = lineEdit->text();
    if (text.trimmed().isEmpty())
        return false;
    value = Util::convertSgfValueFromQString(text, m_charset);
    return true;
}

QLineEdit* GameInfoDialog::createLine(const QString& label, const string& text)
{
    QLineEdit* lineEdit = new LineEdit(0, 30);
    if (! text.empty())
        lineEdit->setText(Util::convertSgfValueToQString(text, m_charset));
    m_formLayout->addRow(label, lineEdit);
    return lineEdit;
}

QLineEdit* GameInfoDialog::createPlayerName(const QString& label, Color c)
{
    return createLine(label, m_game.get_player_name(c));
}

//-----------------------------------------------------------------------------
