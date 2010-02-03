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

#include "dbusinterface.h"

#include <kdebug.h>
#include <KLocale>
#include <KLineEdit>
#include <KComboBox>

#include <QVariant>
#include <QtAlgorithms>
#include <QSpinBox>
#include <QCheckBox>
#include <QDoubleSpinBox>


/*
***********************************
DBusServiceModel
***********************************
*/

DBusServiceModel::DBusServiceModel(QObject* parent): QStandardItemModel(parent) {
    setHorizontalHeaderLabels(QStringList() << i18n("D-Bus applications"));
    foreach(const QString &item, DBusInterface::getInstance()->getRegisteredPrograms()) {
        DBusServiceItem *dbusServiceItem = new DBusServiceItem(item);
        dbusServiceItem->setEditable(false);
        appendRow(dbusServiceItem);
        foreach(const QString &object, DBusInterface::getInstance()->getNodes(item)) {
            dbusServiceItem->appendRow(new QStandardItem(object));
        }
    }
    sort(0, Qt::AscendingOrder);
}

QString DBusServiceModel::application(const QModelIndex& index) const {
    if(index.isValid() && index.parent().isValid()){
        return data(index.parent(), Qt::UserRole).toString();
    }
    return QString();
}

QString DBusServiceModel::node(const QModelIndex& index) const {
    if(index.isValid() && index.parent().isValid()){
        return data(index, Qt::DisplayRole).toString();
    }
    return QString();
}


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
    qRegisterMetaType<Prototype*>("Prototype*");
}

void DBusFunctionModel::refresh(const QString &app, const QString &node) {
    clear();

    if(app.isEmpty()){
        return;
    }
    
    foreach(const Prototype &prototype, DBusInterface::getInstance()->getFunctions(app, node)){
        appendRow(prototype);
    }

    sort(0, Qt::AscendingOrder);
    
}

void DBusFunctionModel::appendRow(Prototype prototype) {
    QList<QStandardItem*> itemList;
    QStandardItem *item = new QStandardItem(prototype.name());
    item->setData(qVariantFromValue(prototype), Qt::UserRole);
    itemList.append(item);
    QString argString;
    foreach(const Argument &arg, prototype.args()){
        if(!argString.isEmpty()){
            argString += ", ";
        }
        argString += QString(QVariant::typeToName(arg.value().type()));
        if(!arg.description().isEmpty()){
            argString += " " + arg.description();
        }
    }
    itemList.append(new QStandardItem(argString));
//    itemList.append(new QStandardItem(prototype.name()));
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
            }
        }
    }
    return QVariant();
}

/*
***********************************
ArgumentsModel
***********************************
*/


ArgumentsModel::ArgumentsModel(QObject* parent): QStandardItemModel(parent) {
}


void ArgumentsModel::refresh(const Prototype& prototype) {
    clear();
    foreach(const Argument &arg, prototype.args()){
        QList<QStandardItem*> itemList;
        itemList.append(new QStandardItem(QString(QVariant::typeToName(arg.value().type())) + " " + arg.description()));
        itemList.append(new ArgumentsModelItem(arg));
        appendRow(itemList);
    }
    
}

QVariant ArgumentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18n("Description");
            case 1:
                return i18n("Value");
            }
        }
    }
    return QVariant();
}

QList<Argument> ArgumentsModel::arguments() const {
    QList<Argument> argList;
    for(int i = 0; i < rowCount(); i++){
        argList.append(item(i, 1)->data(Qt::UserRole));
    }
    return argList;
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
    Argument arg = qVariantValue<Argument>(index.model()->data(index, Qt::EditRole));
    kDebug() << "creaing edtor for:" << arg.description() << "value:" << arg.value();
    switch (arg.value().type()) {
        case QVariant::Int:
        case QVariant::LongLong: {
            QSpinBox *spinBox = new QSpinBox(parent);
            spinBox->setMaximum(maxInt/2);
            spinBox->setMinimum(-maxInt/2);
            spinBox->setValue(arg.value().toInt());
            editor = spinBox;
            }
            break;
        case QVariant::UInt: {
            QSpinBox *spinBox = new QSpinBox(parent);
            spinBox->setMaximum(maxInt/2);
            spinBox->setValue(arg.value().toUInt());
            editor = spinBox;
            }
            break;
        case QVariant::Double: {
            QDoubleSpinBox *dSpinBox = new QDoubleSpinBox(parent);
            dSpinBox->setValue(arg.value().toDouble(NULL));
            editor = dSpinBox;
            }
            break;
        case QVariant::Bool: {
            KComboBox *comboBox = new KComboBox(parent);
            comboBox->addItem(i18nc("True", "Value is true"));
            comboBox->addItem(i18nc("False", "Value is false"));
            comboBox->setCurrentIndex(arg.value().toBool() ? 0 : 1);
            editor = comboBox;
            }
            break;
        case QVariant::StringList: {
            KLineEdit *listLineEdit = new KLineEdit(parent);
            listLineEdit->setToolTip(i18n("A comma-separated list of Strings"));
            QString value;
            value.clear();
            foreach(const QString &tmp, arg.value().toStringList()) {
                if (!value.isEmpty()) {
                    value.append(',');
                }
                value += tmp;
            }
            listLineEdit->setText(value);

            editor = listLineEdit;
            }
            break;
        case QVariant::ByteArray:
        case QVariant::String:
        default: {
            KLineEdit *lineEdit = new KLineEdit(parent);
            lineEdit->setText(arg.value().toString());
            editor = lineEdit;
            }
    }
    return editor;
}


void ArgumentDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{

    Argument arg = qVariantValue<Argument>(index.model()->data(index, Qt::EditRole));
    switch (arg.value().type()) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::LongLong: {
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(arg.value().toInt());
    }
    break;
    case QVariant::Double: {
        QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
        doubleSpinBox->setValue(arg.value().toDouble(NULL));
    }
    case QVariant::Bool: {
        KComboBox *comboBox = static_cast<KComboBox*>(editor);
        comboBox->setCurrentIndex(arg.value().toBool() ? 0 : 1);
    }
    break;
    case QVariant::StringList: {
        KLineEdit *listLineEdit = static_cast<KLineEdit*>(editor);
        QString value;
        value.clear();
        foreach(const QString &tmp, arg.value().toStringList()) {
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
        lineEdit->setText(arg.value().toString());
    }
    }

}


void ArgumentDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QVariant value;
    Argument arg = qVariantValue<Argument>(index.model()->data(index, Qt::EditRole));
    switch (arg.value().type()) {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
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
    arg.setValue(value);
    model->setData(index, qVariantFromValue(arg), Qt::EditRole);
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

ArgumentsModelItem::ArgumentsModelItem ( const Argument &arg ) {
    setData(qVariantFromValue(arg), Qt::EditRole);
    kDebug() << "creating model item:" << arg.value() << "type:" << arg.value().type();
    if (arg.value().type() == QVariant::StringList) {
        setToolTip(i18n("A comma-separated list of Strings"));
    }

}

QVariant ArgumentsModelItem::data ( int role ) const {

    if(role == Qt::DisplayRole) {
        Argument arg = qVariantValue<Argument>(QStandardItem::data(Qt::EditRole));
        kDebug() << "got arg:" << arg.description() << "with type" << arg.value();
        if(arg.value().type() == QVariant::StringList) {
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
            return arg.value();
        }
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
    qRegisterMetaType<ProfileActionTemplate*>("ProfileActionTemplate*");
}

ProfileModel::ProfileModel(Profile* profile, QObject* parent): QStandardItemModel(parent)
{
    ProfileModel();
    foreach(ProfileActionTemplate profileActionTemplate, profile->actionTemplates()){
      appendRow(profileActionTemplate);
    }
    sort(0, Qt::DescendingOrder);
}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18nc("Profile name", "Name");
            case 1:
                return i18n("Description");
            case 2:
                return i18n("Default argument count");
            case 3:
                return i18n("Mapped remote button");
            }
        }
    }
    return QVariant();
}

ProfileActionTemplate* ProfileModel::getProfileActionTemlate(int index) const
{
    return index == -1 ? 0 : QStandardItemModel::item(index)->data(Qt::UserRole).value<ProfileActionTemplate*>();
}


void ProfileModel::appendRow(ProfileActionTemplate actionTemplate)
{
    QList<QStandardItem*> row;
    QStandardItem *item = new QStandardItem(actionTemplate.actionName());
    item->setData(qVariantFromValue(&actionTemplate), Qt::UserRole);
    row.append(item);

    if (!(actionTemplate.description().isEmpty())) {
        QStandardItem *tItem = new  QStandardItem(actionTemplate.description());
        tItem->setToolTip(actionTemplate.description());
        row.append(tItem);
    } else {
        row.append(new QStandardItem("-"));
    }
    row.append(new QStandardItem(QString::number(actionTemplate.defaultArguments().size())));
    if (!actionTemplate.buttonName().isEmpty()) {
        row.append(new QStandardItem(actionTemplate.buttonName()));
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
  return index == -1 ? 0 : QStandardItemModel::item(index)->data(Qt::UserRole).value<RemoteControlButton*>();
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


QModelIndex RemoteButtonModel::indexOfButtonName(const QString &button)
{
  for(int row = 0; row < QStandardItemModel::rowCount(); ++row){
    if(button == getButton(row)->name()){
      return indexFromItem(item(row));
    }
  }
  return QModelIndex();
}


RemoteModel::RemoteModel(QObject* parent): QStandardItemModel(parent) {
}

RemoteModel::RemoteModel(const RemoteList &remoteList, QObject *parent) : QStandardItemModel(parent) {
    refresh(remoteList);
}

void RemoteModel::refresh(const RemoteList &remoteList){
    clear();
    foreach(Remote *remote, remoteList){
        QStandardItem *item = new RemoteItem(remote);
        appendRow(item);
    }    
}

Remote *RemoteModel::remote(const QModelIndex &index) const {
    if(index.isValid() && index.parent().isValid()){
        return qVariantValue<Remote*>(index.parent().data(Qt::UserRole));
    }
    if(index.isValid()){
        return qVariantValue<Remote*>(index.data(Qt::UserRole));
    }
    return 0;
}

Mode *RemoteModel::mode(const QModelIndex &index) const {
    if(index.isValid() && index.parent().isValid()){
        return qVariantValue<Mode*>(index.data(Qt::UserRole));
    }
    return 0;
}

// QVariant RemoteModel::headerData(int section, Qt::Orientation o, int role) const {
//     if (role == Qt::DisplayRole) {
//         return i18nc("Remote name", "Remote");
//     }
//     return QAbstractListModel::headerData(section,o,role);
// }


RemoteItem::RemoteItem(Remote *remote) {
    qRegisterMetaType<Remote*>("Remote*");
    qRegisterMetaType<Mode*>("Mode*");
    setData(qVariantFromValue(remote), Qt::UserRole);
    foreach(Mode *mode, remote->allModes()) {
        QStandardItem *item = new QStandardItem(mode->name());
        item->setData(qVariantFromValue(mode), Qt::UserRole);
        appendRow(item);
    }
}

QVariant RemoteItem::data(int role) const {
    if(role == Qt::DisplayRole) {
        Remote *remote = qVariantValue<Remote*>(QStandardItem::data(Qt::UserRole));
        return remote->name();
    }
    return QStandardItem::data(role);
}
