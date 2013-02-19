//-----------------------------------------------------------------------------
/** @file pentobi/AnalyzeSpeedDialog.h */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_ANALYZE_SPEED_DIALOG_H
#define PENTOBI_ANALYZE_SPEED_DIALOG_H

// Needed in the header because moc_*.cxx does not include config.h
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QInputDialog>

//-----------------------------------------------------------------------------

class AnalyzeSpeedDialog
    : public QInputDialog
{
    Q_OBJECT

public:
    AnalyzeSpeedDialog(QWidget* parent, const QString& title);

    /** Get return value if dialog was accepted.
        0 = fast, 1 = normal, 2 = slow */
    int getSpeedValue()
    {
        return m_speedValue;
    }

public slots:
    void accept();

private:
    int m_speedValue;

    QStringList m_items;
};

//-----------------------------------------------------------------------------

#endif // PENTOBI_ANALYZE_SPEED_DIALOG_H
