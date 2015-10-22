#include "settingsshortcutwidget.h"

SettingsShortcutWidget::SettingsShortcutWidget (const QStringList &actionList, const QStringList &keyList, QWidget *parent)
    : QDialog (parent) {
    okButton.setText ("OK");
    cancelButton.setText ("Cancel");
    connect (&okButton, SIGNAL (pressed()), this, SLOT (accept()));
    connect (&cancelButton, SIGNAL (pressed()), this, SLOT (reject()));

    action.addItems (actionList);
    key.addItems (keyList);
    ctrl.setText ("Ctrl");
    alt.setText ("Alt");
    shift.setText ("Shift");

    hBox.addWidget (&action);
    hBox.addSpacing (30);
    hBox.addWidget (&ctrl);
    hBox.addWidget (&alt);
    hBox.addWidget (&shift);
    hBox.addWidget (&key);
    hBox.addSpacing (10);
    hBox.addWidget (&okButton);
    hBox.addWidget (&cancelButton);

    action.setMinimumHeight (28);
    ctrl.setMinimumHeight (28);
    alt.setMinimumHeight (28);
    shift.setMinimumHeight (28);
    key.setMinimumHeight (28);
    okButton.setMinimumHeight (28);
    cancelButton.setMinimumHeight (28);

    setLayout (&hBox);
}

QString SettingsShortcutWidget::text() {
    QString str;
    str = action.currentText() +"=";
    if (ctrl.isChecked()) str.append ("Ctrl+");
    if (alt.isChecked()) str.append ("Alt+");
    if (shift.isChecked()) str.append ("Shift+");
    str.append (key.currentText());
    return str;
}

SettingsShortcutWidget::~SettingsShortcutWidget() {

}

