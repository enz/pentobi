//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeSpeedDialog.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_SPEED_DIALOG_H
#define PENTOBI_ANALYZE_SPEED_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QInputDialog>

//-----------------------------------------------------------------------------

class AnalyzeSpeedDialog final
    : public QInputDialog
{
    Q_OBJECT

public:
    AnalyzeSpeedDialog(QWidget* parent, const QString& title);

    ~AnalyzeSpeedDialog();

    /** Get return value if dialog was accepted.
        0 = fast, 1 = normal, 2 = slow */
    int getSpeedValue() { return m_speedValue; }

public slots:
    void accept() override;

private:
    int m_speedValue = 0;

    QStringList m_items;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_SPEED_DIALOG_H
