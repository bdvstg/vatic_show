#ifndef emptyForm_H__
#define emptyForm_H__

#include <QWidget>
#include "ui_emptyForm.h"

class emptyForm : public QWidget
{
    Q_OBJECT

public:
    emptyForm(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~emptyForm() = default;

private:
    Ui::emptyForm ui;
};

#endif
