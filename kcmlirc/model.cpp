/*************************************************************************
* Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
*                                                                       *
* This program is free software; you can redistribute it and/or         *
* modify it under the terms of the GNU General Public License as        *
* published by the Free Software Foundation; either version 2 of        *
* the License or (at your option) version 3 or any later version        *
* accepted by the membership of KDE e.V. (or its successor approved     *
* by the membership of KDE e.V.), which shall act as a proxy            *
* defined in Section 14 of version 3 of the license.                    *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
*************************************************************************/

/**
*
*  Created on: 01.02.2009
*      @author Frank Scheffold and Michael Zanetti
*/

#include "model.h"

#include <QtAlgorithms>
#include <kdebug.h>
#include <QVariant>
#include <KLocale>
#include <QSpinBox>
#include <KLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <KComboBox>



/*
***********************************
DBusServiceItem
***********************************
*/



DBusServiceItem::DBusServiceItem(const QString &item) {
    setData(item, Qt::UserRole);
    setFlags(Qt::ItemIsEnabled);
}

DBusServiceItem::DBusServiceItem(const QString &item,  const QStringList &objects)  {
    new DBusServiceItem(item);
    foreach(const QString &object,  objects) {
        this->appendRow(new QStandardItem(object));
    }
}


QVariant DBusServiceItem::data(int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)  {
        return trimAppname(QStandardItem::data(Qt::UserRole).toString());
    }
    else if (role == Qt::UserRole) {
        return QStandardItem::data(role);
    }
    return QVariant();
}

QString DBusServiceItem::trimAppname(const QString& appName) {
    int lastIndex = appName .lastIndexOf(".") + 1;
    if (lastIndex < appName.size()) {
        QString s = appName;
        QString domainName = appName;
        s.remove(0, lastIndex);
        domainName.remove(lastIndex -1, domainName.length());
        return  s.append(" (").append( domainName).append(')');;
    }
    return appName;
}

/*
***********************************
DBusFunctionModel
***********************************
*/

DBusFunctionModel::DBusFunctionModel(QObject *parent):QStandardItemModel(parent) {
    qRegisterMetaType<Prototype>("Prototype");
}

void DBusFunctionModel::appendRow ( const Prototype &prototype ) {
    QList<QStandardItem*> itemList;
    QStandardItem *item = new QStandardItem(prototype.name());
    item->setData(qVariantFromValue(prototype), Qt::UserRole);
    itemList.append(item);
    itemList.append(new QStandardItem(prototype.argumentList()));
    itemList.append(new QStandardItem(prototype.prototype()));
    QStandardItemModel::appendRow(itemList);
}

Prototype DBusFunctionModel::getPrototype(int index) const {
    return QStandardItemModel::item(index)->data(Qt::UserRole).value<Prototype>();
}

QVariant DBusFunctionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18n("Function");
            case 1:
                return i18n("Parameter");
            case 2:
                return i18n("Prototype");
            }
        }
    }
    return QVariant();
}

/*
***********************************
ArgumentDelegate
***********************************
*/


ArgumentDelegate::ArgumentDelegate(QObject *parent)
        : QItemDelegate(parent)
{
}

QWidget *ArgumentDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/* option */,
                                        const QModelIndex &index) const
{
    QWidget *editor;
    unsigned int maxInt = -1;
//    kDebug() << "creaing edtor for:" << index.model()->data(index, Qt::EditRole);
    switch (index.model()->data(index, Qt::EditRole).type()) {
    case QVariant::Int: {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setMaximum(maxInt/2);
        spinBox->setMinimum(-maxInt/2);
        editor = spinBox;
    }
    break;
    case QVariant::UInt: {
        QSpinBox *spinBox = new QSpinBox(parent);
        spinBox->setMaximum(maxInt/2);
        editor = spinBox;
    }
    break;
    case QVariant::Double:
        editor = new QDoubleSpinBox(parent);
        break;
    case QVariant::Bool:
        editor = new KComboBox(parent);
        break;
    case QVariant::StringList: {
        KLineEdit *lineEdit = new KLineEdit(parent);
        lineEdit->setToolTip(i18n("A comma-separated list of Strings"));
        editor = lineEdit;
    }
    break;
    case QVariant::ByteArray:
    case QVariant::String:
    default:
        editor = new KLineEdit(parent);
    }
    return editor;
}


void ArgumentDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{

    switch (index.model()->data(index, Qt::EditRole).type()) {
    case QVariant::UInt:
    case QVariant::Int: {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(index.model()->data(index, Qt::EditRole).toInt());
    }
    break;
    case QVariant::Double: {
        QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
        doubleSpinBox->setValue(index.model()->data(index, Qt::EditRole).toDouble(NULL));
    }
    case QVariant::Bool: {
        KComboBox *comboBox = static_cast<KComboBox*>(editor);
        comboBox->addItem(i18nc("True", "Value is true"));
        comboBox->addItem(i18nc("False", "Value is false"));
        comboBox->setCurrentIndex(index.model()->data(index, Qt::EditRole).toBool() ? 0 : 1);
    }
    break;
    case QVariant::StringList: {
        KLineEdit *listLineEdit = static_cast<KLineEdit*>(editor);
        QString value;
        value.clear();
        foreach(const QString &tmp, index.model()->data(index, Qt::EditRole).toStringList()) {
            if (!value.isEmpty()) {
                value.append(',');
            }
            value += tmp;
        }
        listLineEdit->setText(value);
    }
    break;
    case QVariant::ByteArray:
    case QVariant::String:
    default: {
        KLineEdit *lineEdit = static_cast<KLineEdit*>(editor);
        lineEdit->setText(index.model()->data(index, Qt::EditRole).toString());
    }
    }

}


void ArgumentDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QVariant value;
    switch (index.model()->data(index, Qt::EditRole).type()) {
    case QVariant::Int:
    case QVariant::UInt:
        value = QVariant(static_cast<QSpinBox*>(editor)->value());
        break;
    case QVariant::Double:
        value = QVariant(static_cast<QDoubleSpinBox*>(editor)->value());
        break;
    case QVariant::Bool:
        value = QVariant(static_cast<KComboBox*>(editor)->currentIndex() == 0 ? true : false);
        break;
    case QVariant::StringList:
        value = QVariant(static_cast<KLineEdit*>(editor)->text().split(','));
        break;
    case QVariant::ByteArray:
    case QVariant::String:
    default: {
        value = QVariant(static_cast<KLineEdit*>(editor)->text());
    }
    }
    kDebug() << "setting value" << value;
    model->setData(index, value, Qt::EditRole);
}

void ArgumentDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}




/*
***********************************
ArgumentsModelItem
***********************************
*/

ArgumentsModelItem::ArgumentsModelItem ( const QString & text ):QStandardItem(text) {
    setFlags(Qt::ItemIsEnabled);
}

ArgumentsModelItem::ArgumentsModelItem ( const QVariant &data ) {
    setData(data, Qt::EditRole);
    kDebug() << "creating model item:" << data;
    if (data.type() == QVariant::StringList) {
        setToolTip(i18n("A comma-separated list of Strings"));
    }

}

QVariant ArgumentsModelItem::data ( int role ) const {

    if (role == Qt::DisplayRole && (QStandardItem::data(role).type() == QVariant::StringList)) {
        QString retList;
        retList.clear();
        foreach(const QString &tmp, QStandardItem::data(role).toStringList()) {
            if (!retList.isEmpty()) {
                retList.append(',');
            }
            retList += tmp;
        }
        return QVariant(retList);
    } else {
        return QStandardItem::data(role);
    }
}

/*
***********************************
ProfileModel
***********************************
*/



ProfileModel::ProfileModel(QObject* parent): QStandardItemModel(parent)
{
    qRegisterMetaType<ProfileAction*>("ProfileAction*");
}

ProfileModel::ProfileModel(const Profile* profile, QObject* parent): QStandardItemModel(parent)
{
    ProfileModel();
    foreach(ProfileAction *action, profile->actions()) {
        appendRow(action);
    }
    sort(0, Qt::DescendingOrder);
}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18n("Name");
            case 1:
                return i18n("Options");
            case 2:
                return i18n("Comment");
            case 3:
                return i18n("Mapped remote button");
            }
        }
    }
    return QVariant();
}

ProfileAction* ProfileModel::getProfileAction(int index) const
{
    return QStandardItemModel::item(index)->data(Qt::UserRole).value<ProfileAction*>();
}


void ProfileModel::appendRow(ProfileAction *action)
{
    QList<QStandardItem*> row;
    QStandardItem *item = new QStandardItem(action->name());
    item->setData(qVariantFromValue(action), Qt::UserRole);
    row.append(item);
    row.append(new QStandardItem(QString::number(action->arguments().size())));
    if (!(action->comment().isEmpty())) {
        QStandardItem *tItem = new  QStandardItem(action->comment());
        tItem->setToolTip(action->comment());
        row.append(tItem);
    } else {
        row.append(new QStandardItem("-"));
    }
    if (!action->buttonName().isEmpty()) {
        row.append(new QStandardItem(action->buttonName()));
    } else {
        row.append(new QStandardItem("-"));
    }
    QStandardItemModel::appendRow(row);
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex& index) const
{
    return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable);
}


/*
***********************************
RemoteButtonModel
***********************************
*/


RemoteButtonModel::RemoteButtonModel(QObject* parent): QStandardItemModel(parent)
{
    qRegisterMetaType<RemoteControlButton*>("RemoteControlButton*");
}


RemoteButtonModel::RemoteButtonModel( const QList<RemoteControlButton> &buttonList, QObject* parent): QStandardItemModel(parent)
{
    RemoteButtonModel();
    foreach(const RemoteControlButton &tButton, buttonList) {
        appendRow(tButton);
    }
    sort(0, Qt::AscendingOrder);
}


void RemoteButtonModel::appendRow( const RemoteControlButton &button)
{
    QList<QStandardItem*> row;
    m_buttonList.append(button);
    QStandardItem *item = new QStandardItem(m_buttonList.last().description());
    item->setData(qVariantFromValue(&m_buttonList.last()), Qt::UserRole);
    row.append(item);
    if (m_buttonList.last().id() != RemoteControlButton::Unknown) {
        row.append(new QStandardItem(m_buttonList.last().name()));
    }
    QStandardItemModel::appendRow(row);
}



Solid::Control::RemoteControlButton* RemoteButtonModel::getButton(int index) const
{
    return QStandardItemModel::item(index)->data(Qt::UserRole).value<RemoteControlButton*>();
}


Qt::ItemFlags RemoteButtonModel::flags(const QModelIndex& index) const
{
    return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable);
}


QVariant RemoteButtonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18n("Button");
            case 1:
                return i18n("Identifier");
            }
        }
    }
    return QVariant();
}



RemoteModel::RemoteModel(QObject* parent): QStringListModel(parent)
{
}

RemoteModel::RemoteModel(const QStringList &strings, QObject *parent) : QStringListModel(strings, parent)
{
    QStringListModel::sort(0, Qt::AscendingOrder);
}

QVariant RemoteModel::headerData(int section, Qt::Orientation o, int role) const
{
    if (role == Qt::DisplayRole) {
        return i18n("Remote");
    }
    return QAbstractListModel::headerData(section,o,role);
}
