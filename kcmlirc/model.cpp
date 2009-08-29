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

DBusFunctionModel::DBusFunctionModel(QObject *parent):QStandardItemModel(parent){
    qRegisterMetaType<Prototype>("Prototype");
}

void DBusFunctionModel::appendRow ( const Prototype &prototype ){
    QList<QStandardItem*> itemList;
    QStandardItem *item = new QStandardItem(prototype.name());
    item->setData(qVariantFromValue(prototype), Qt::UserRole);
    itemList.append(item);
    itemList.append(new QStandardItem(prototype.argumentList()));
    itemList.append(new QStandardItem(prototype.prototype()));
    QStandardItemModel::appendRow(itemList);
}

Prototype DBusFunctionModel::getPrototype(int index) const{
    return QStandardItemModel::item(index)->data(Qt::UserRole).value<Prototype>();
}

QVariant DBusFunctionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
	if (role == Qt::DisplayRole) {
	    //            kDebug()<< "section is " << section;
	    switch (section) {
		case 0:
		    return i18n("Function");
		case 1:
		    return i18n("Parameter");
		case 2:
		    return i18n("Prototype");
		default:
		    return QVariant();
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
//    kDebug() << "creaing edtor for:" << index.model()->data(index, Qt::EditRole);
    switch (index.model()->data(index, Qt::EditRole).type()) {
    case QVariant::Int:
    case QVariant::UInt:
        editor = new QSpinBox(parent);
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
//    kDebug() << "setting value" << value;
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
