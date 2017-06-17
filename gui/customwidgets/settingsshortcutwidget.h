#ifndef SETTINGSSHORTCUTWIDGET_H
#define SETTINGSSHORTCUTWIDGET_H

#include <QDialog>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QString>
#include <QPushButton>
#include "components/actionmanager/actionmanager.h"

class SettingsShortcutWidget : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsShortcutWidget(const QStringList &actionList, const QStringList &keyList, QWidget *parent = 0);
    ~SettingsShortcutWidget();
    QHBoxLayout hBox;
    QComboBox action, key;
    QCheckBox ctrl, alt, shift;
    QPushButton okButton, cancelButton;
    QString text();

signals:

public slots:
};

#endif // SETTINGSSHORTCUTWIDGET_H
