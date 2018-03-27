#include "scriptwidget.h"
#include "ui_scriptwidget.h"

ScriptWidget::ScriptWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::ScriptWidget)
{
    ui->setupUi(this);
}

ScriptWidget::~ScriptWidget()
{
    delete ui;
}
