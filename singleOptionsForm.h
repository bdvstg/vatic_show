#ifndef singleOptionsForm_H__
#define singleOptionsForm_H__

#include <memory>
#include <vector>
#include <functional>

#include <QRadioButton>
#include <QScopedPointer>
#include <QScrollArea>
//#include <QLayout>
#include <QWidget>
#include "ui_emptyForm.h"

class singleOptionsForm : public QWidget
{
    Q_OBJECT

public:
    singleOptionsForm(const char* title = nullptr, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~singleOptionsForm() = default;

    void setOptions(const std::vector<std::string> options);
    void setSelected(int i);
    void setSelected(const std::string & option);
    void setCallbackSelectedChange(std::function<void(int)> f);
    int getSelected() const;

private:
    Ui::emptyForm ui;
    std::vector<QRadioButton*> mOptions;
    QScopedPointer<QScrollArea> mScrollArea;
    std::function<void(int)> mCallBackSelectedChange;
    void clearOptions();

private slots:
    void radioButtons_toggled(bool checked);
};

#endif
