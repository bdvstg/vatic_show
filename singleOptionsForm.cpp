#include "singleOptionsForm.h"

singleOptionsForm::singleOptionsForm(const char* title, QWidget *parent, Qt::WindowFlags flags)
{
    ui.setupUi(this);
    mScrollArea.reset(new QScrollArea());
    mScrollArea->setHidden(false);
    mScrollArea->setWidget(this);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->resize(300, 500);
    if (title != nullptr)
        mScrollArea->setWindowTitle(title);
    this->layout()->setSpacing(10);
}

void singleOptionsForm::clearOptions()
{
    if (mOptions.size() != 0)
    {
        for (int i = 0; i < mOptions.size(); i++)
        {
            this->layout()->removeWidget(mOptions[i]);
            delete mOptions[i];
            mOptions[i] = nullptr;
        }
        mOptions.clear();
    }
}

void singleOptionsForm::setOptions(const std::vector<std::string> options)
{
    if (mOptions.size() != 0)
    {
        clearOptions();
    }

    mOptions.resize(options.size());
    for (int i = 0; i < mOptions.size(); i++)
    {
        mOptions[i] = new QRadioButton();
        mOptions[i]->setStyleSheet("::indicator { height: 50px; }");
        mOptions[i]->setStyleSheet("font-size: 30px;");
        mOptions[i]->setText(options[i].c_str());
        connect(mOptions[i], SIGNAL(toggled(bool)), this, SLOT(on_radioButtons_toggled(bool)));
        this->layout()->addWidget(mOptions[i]);
    }
}

void singleOptionsForm::setCallbackSelectedChange(std::function<void(int)> f)
{
    mCallBackSelectedChange = f;
}

void singleOptionsForm::setSelected(int i)
{
    if (i >= 0 && i < mOptions.size())
        mOptions[i]->setChecked(true);
}

void singleOptionsForm::setSelected(const std::string & option)
{
    QString qOption = option.c_str();
    for (int i = 0; i < mOptions.size(); i++)
    {
        if (mOptions[i]->text() == qOption)
        {
            mOptions[i]->setChecked(true);
            break;
        }
    }
}

int singleOptionsForm::getSelected() const
{
    int selected = -1; // no any is checked
    for (int i = 0; i < mOptions.size(); i++)
    {
        if (mOptions[i]->isChecked())
        {
            return selected = i;
            break;
        }
    }
    return selected;
}

void singleOptionsForm::on_radioButtons_toggled(bool checked)
{
    int selected = getSelected();
    if (mCallBackSelectedChange)
    {
        mCallBackSelectedChange(selected);
    }
}