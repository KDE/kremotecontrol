/*
    Copyright (C) 2010 Michael Zanetti <mzanetti@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "editkeypressaction.h"

EditKeypressAction::EditKeypressAction(KeypressAction *action, QWidget* parent, Qt::WFlags flags): QWidget(parent, flags) {
    m_action = action;
    ui.setupUi(this);
    ui.pbAdd->setIcon(KIcon( QLatin1String( "list-add" )));
    ui.pbRemove->setIcon(KIcon( QLatin1String( "list-remove" )));
    ui.pbUp->setIcon(KIcon( QLatin1String( "arrow-up" )));
    ui.pbDown->setIcon(KIcon( QLatin1String( "arrow-down" )));

    m_model = new KeySequenceListModel(this);
    m_model->setList(action->keySequenceList());
    ui.listView->setModel(m_model);

    ui.cbRepeat->setChecked(m_action->repeat());

    ui.keySequenceWidget->setCheckForConflictsAgainst(0);
    ui.keySequenceWidget->setModifierlessAllowed(true);
    ui.keySequenceWidget->setClearButtonShown(false);

    connect(ui.keySequenceWidget, SIGNAL(keySequenceChanged(const QKeySequence &)), SLOT(setKeySequence(const QKeySequence &)));
    connect(ui.pbAdd, SIGNAL(clicked()), SLOT(keySequenceChanged()));
    connect(ui.pbRemove, SIGNAL(clicked()), SLOT(keySequenceChanged()));
    connect(ui.leKeySequence, SIGNAL(textChanged(QString)), SLOT(activateButtons()));
    connect(ui.listView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(activateButtons()));

    activateButtons();
}

EditKeypressAction::~EditKeypressAction() {
}

bool EditKeypressAction::checkForComplete() const {
    return ui.listView->model()->rowCount() > 0;
}

void EditKeypressAction::applyChanges(){
    m_action->setKeySequenceList(m_model->keySeqenceList());
    m_action->setRepeat(ui.cbRepeat->isChecked());
}

KeypressAction EditKeypressAction::action() const {
    KeypressAction action;
    action.setKeySequenceList(m_model->keySeqenceList());
    action.setRepeat(ui.cbRepeat->isChecked());
    return action;
}

void EditKeypressAction::keySequenceChanged() {
    activateButtons();
    emit formComplete(!m_model->keySeqenceList().isEmpty());
}

void EditKeypressAction::setKeySequence(const QKeySequence &seq) {
    if(seq.isEmpty()) {
        return;
    }
    ui.leKeySequence->setText(seq.toString(QKeySequence::NativeText));
    ui.keySequenceWidget->clearKeySequence();
}

KeySequenceItem::KeySequenceItem(const QKeySequence &seq) {
    m_sequence = seq;
}

void EditKeypressAction::on_pbAdd_clicked() {
    m_model->appendRow(new KeySequenceItem(QKeySequence(ui.leKeySequence->text(), QKeySequence::NativeText)));
    ui.leKeySequence->clear();
}

void EditKeypressAction::on_pbRemove_clicked() {
    int index = ui.listView->selectionModel()->currentIndex().row();
    m_model->removeRow(index);
}

void EditKeypressAction::on_pbUp_clicked() {
    QModelIndex index = ui.listView->selectionModel()->currentIndex();
    QKeySequence seq = m_model->data(index, Qt::UserRole).value<QKeySequence>();
    m_model->removeRow(index.row());
    m_model->insertRow(index.row() - 1, new KeySequenceItem(seq));
    ui.listView->selectionModel()->setCurrentIndex(m_model->index(index.row() - 1, 0), QItemSelectionModel::ClearAndSelect);
}

void EditKeypressAction::on_pbDown_clicked() {
    QModelIndex index = ui.listView->selectionModel()->currentIndex();
    QKeySequence seq = m_model->data(index, Qt::UserRole).value<QKeySequence>();
    m_model->removeRow(index.row());
    m_model->insertRow(index.row() + 1, new KeySequenceItem(seq));
    ui.listView->selectionModel()->setCurrentIndex(m_model->index(index.row() + 1, 0), QItemSelectionModel::ClearAndSelect);
}

void EditKeypressAction::activateButtons() {
    QModelIndex index = ui.listView->selectionModel()->currentIndex();
    ui.pbAdd->setEnabled(!QKeySequence(ui.leKeySequence->text(), QKeySequence::NativeText).isEmpty());
    ui.pbRemove->setEnabled(index.isValid());
    ui.pbUp->setEnabled(index.row() > 0);
    ui.pbDown->setEnabled(index.isValid() && (index.row() + 1 < m_model->rowCount()));
}

QVariant KeySequenceItem::data(int role) const {
    if(role == Qt::DisplayRole) {
        return m_sequence.toString(QKeySequence::PortableText);
    }
    if(role == Qt::UserRole) {
        return m_sequence;
    }
    return QStandardItem::data(role);
}

KeySequenceListModel::KeySequenceListModel(QObject *parent):
    QStandardItemModel(parent)
{

}

void KeySequenceListModel::setList(QList<QKeySequence> list) {
    foreach(const QKeySequence &seq, list) {
        insertRow(rowCount(), new KeySequenceItem(seq));
    }
}

QList<QKeySequence> KeySequenceListModel::keySeqenceList() {
    QList<QKeySequence> ret;
    for(int i = 0; i < rowCount(); ++i) {
        ret.append(item(i)->data(Qt::UserRole).value<QKeySequence>());
    }
    return ret;
}
