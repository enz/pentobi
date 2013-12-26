//-----------------------------------------------------------------------------
/** @file pentobi/ShowMessage.h
    @author Markus Enzenberger
    @copyright GNU General Public License version 3 or later */
//-----------------------------------------------------------------------------

#ifndef PENTOBI_SHOW_MESSAGE_H
#define PENTOBI_SHOW_MESSAGE_H

#include <QString>

class QMessageBox;
class QWidget;

//-----------------------------------------------------------------------------

void initQuestion(QMessageBox& msgBox, const QString& text,
                  const QString& infoText = "");

void showError(QWidget* parent, const QString& text,
               const QString& infoText = "", const QString& detailText = "");

void showInfo(QWidget* parent, const QString& text,
              const QString& infoText = "", const QString& detailText = "",
              bool withIcon = false);

void showFatal(const QString& detailedText);

//-----------------------------------------------------------------------------

#endif // PENTOBI_SHOW_MESSAGE_H
