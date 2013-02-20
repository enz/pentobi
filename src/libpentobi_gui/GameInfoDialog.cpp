//-----------------------------------------------------------------------------
/** @file libpentobi_gui/GameInfoDialog.cpp */
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "GameInfoDialog.h"

#include <QDialogButtonBox>
#include "LineEdit.h"
#include "libpentobi_gui/Util.h"

using libpentobi_base::Variant;

//-----------------------------------------------------------------------------

GameInfoDialog::GameInfoDialog(QWidget* parent, Game& game)
    : QDialog(parent),
      m_game(game)
{
    m_charset = game.get_root().get_property("CA", "");
    setWindowTitle(tr("Game Info"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    auto layout = new QVBoxLayout();
    setLayout(layout);
    m_formLayout = new QFormLayout();
    layout->addLayout(m_formLayout);
    auto variant = game.get_variant();
    if (variant == Variant::duo || variant == Variant::junior)
    {
        m_playerBlue = createPlayerName(tr("Player &Blue:"), Color(0));
        m_playerGreen = createPlayerName(tr("Player &Green:"), Color(1));
    }
    else if (variant == Variant::classic
             || variant == Variant::trigon)
    {
        m_playerBlue = createPlayerName(tr("Player &Blue:"), Color(0));
        m_playerYellow = createPlayerName(tr("Player &Yellow:"), Color(1));
        m_playerRed = createPlayerName(tr("Player &Red:"), Color(2));
        m_playerGreen = createPlayerName(tr("Player &Green:"), Color(3));
    }
    else if (variant == Variant::trigon_3)
    {
        m_playerBlue = createPlayerName(tr("Player &Blue:"), Color(0));
        m_playerYellow = createPlayerName(tr("Player &Yellow:"), Color(1));
        m_playerRed = createPlayerName(tr("Player &Red:"), Color(2));
    }
    else
    {
        LIBBOARDGAME_ASSERT(variant == Variant::classic_2
                            || variant == Variant::trigon_2);
        m_playerBlueRed = createPlayerName(tr("Player &Blue/Red:"), Color(0));
        m_playerYellowGreen =
            createPlayerName(tr("Player &Yellow/Green:"), Color(1));
    }
    m_date = createLine(tr("&Date:"), m_game.get_date());
    m_time = createLine(tr("&Time limits:"), m_game.get_time());
    m_event = createLine(tr("&Event:"), m_game.get_event());
    m_round = createLine(tr("R&ound:"), m_game.get_round());
    auto buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    // We assume that the user wants to edit the game info if it is still empty
    // and that he only wants to display it if not empty. Therefore, we leave
    // the focus at the first text field if it is empty and put it on the
    // button box otherwise.
    if (variant == Variant::classic_2 || variant == Variant::trigon_2)
    {
        if (! m_playerBlueRed->text().isEmpty())
            buttonBox->setFocus();
    }
    else if (! m_playerBlue->text().isEmpty())
        buttonBox->setFocus();
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

void GameInfoDialog::accept()
{
    auto variant = m_game.get_variant();
    string value;
    if (variant == Variant::duo || variant == Variant::junior)
    {
        if (acceptLine(m_playerBlue, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerGreen, value))
            m_game.set_player_name(Color(1), value);
    }
    else if (variant == Variant::classic || variant == Variant::trigon)
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
    else if (variant == Variant::trigon_3)
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
        LIBBOARDGAME_ASSERT(variant == Variant::classic_2
                            || variant == Variant::trigon_2);
        if (acceptLine(m_playerBlueRed, value))
            m_game.set_player_name(Color(0), value);
        if (acceptLine(m_playerYellowGreen, value))
            m_game.set_player_name(Color(1), value);
    }
    if (acceptLine(m_date, value))
        m_game.set_date(value);
    if (acceptLine(m_time, value))
        m_game.set_time(value);
    if (acceptLine(m_event, value))
        m_game.set_event(value);
    if (acceptLine(m_round, value))
        m_game.set_round(value);
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
    auto lineEdit = new LineEdit(0, 30);
    if (! text.empty())
    {
        lineEdit->setText(Util::convertSgfValueToQString(text, m_charset));
        lineEdit->setCursorPosition(0);
    }
    m_formLayout->addRow(label, lineEdit);
    return lineEdit;
}

QLineEdit* GameInfoDialog::createPlayerName(const QString& label, Color c)
{
    return createLine(label, m_game.get_player_name(c));
}

//-----------------------------------------------------------------------------
