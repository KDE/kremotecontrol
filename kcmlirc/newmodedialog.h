#ifndef NEWMODEDIALOG_H
#define NEWMODEDIALOG_H

#include <QDialog>
#include "newmode.h"

class NewModeDialog : public QDialog, public Ui_NewMode
{
    Q_OBJECT

public:
    explicit NewModeDialog( QWidget* parent );
    ~NewModeDialog();

private slots:
    void slotTextChanged( const QString& newText );
};


#endif /* NEWMODEDIALOG_H */
