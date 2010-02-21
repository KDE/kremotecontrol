/*************************************************************************
* Copyright: (C) 2009 by Frank Scheffold <fscheffold@googlemail.com>    *
* Copyright: (C) 2009 by Michael Zanetti <michael_zanetti@gmx.net>      *
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
#include "profileserver.h"

#include <kdebug.h>
#include <KLocale>
#include <KLineEdit>
#include <KComboBox>
#include <kicon.h>

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
#include <kapplication.h>
#include <qmimedata.h>

DBusServiceModel::DBusServiceModel(QObject* parent): QStandardItemModel(parent) {
    setHorizontalHeaderLabels(QStringList() << i18n("Application / Node"));
    foreach(const QString &item, DBusInterface::getInstance()->getRegisteredPrograms()) {
        DBusServiceItem *dbusServiceItem = new DBusServiceItem(item);
        dbusServiceItem->setEditable(false);
        appendRow(dbusServiceItem);
        foreach(const QString &node, DBusInterface::getInstance()->getNodes(item)) {
            dbusServiceItem->appendRow(new QStandardItem(node));
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

QModelIndex DBusServiceModel::findOrInsert(const DBusAction* action, bool insert) {
  
    for(int i = 0; i < rowCount(); i++){
        QStandardItem *appItem = item(i);
        if(!appItem->index().parent().isValid()){ // Only check Applications, no Nodes
            if(appItem->data(Qt::UserRole).toString() == action->application()){
                int j = 0;
                QStandardItem *nodeItem;
                while((nodeItem = appItem->child(j++)) != 0){
                    if(nodeItem->data(Qt::DisplayRole) == action->node()){
                        kDebug() << "Found item at index:" << nodeItem->index();
                        return nodeItem->index();
                    }
                }
            }
        }
    }
    // Not found... Insert it
    if(insert){
        kDebug() << "inserting item because app seems not to be registered at DBus";
        DBusServiceItem *dbusServiceItem = new DBusServiceItem(action->application());
        dbusServiceItem->setEditable(false);
        appendRow(dbusServiceItem);
        QStandardItem *item = new QStandardItem(action->node());
        dbusServiceItem->appendRow(item);
        return item->index();
    }
    kDebug() << "Not found and not inserting... Returning invalid index";
    return QModelIndex();
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

QVariant DBusServiceItem::data(int role) const {
    if (role == Qt::DisplayRole || role == Qt::EditRole)  {
        return trimAppname(QStandardItem::data(Qt::UserRole).toString());
    }
    return QStandardItem::data(role);
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

QModelIndex DBusFunctionModel::findOrInsert(const DBusAction* action, bool insert) {
  
    for(int i = 0; i < rowCount(); i++){
        QStandardItem *functionItem = item(i);
        if(functionItem->data(Qt::UserRole).value<Prototype>() == action->function()){
            return functionItem->index();
        }
    }
    // Not found... Insert it
    if(insert){
        QList<QStandardItem*> itemList;
        QStandardItem *item = new QStandardItem(action->function().name());
        item->setData(qVariantFromValue(action->function()), Qt::UserRole);
        itemList.append(item);
        QString argString;
        foreach(const Argument &arg, action->function().args()){
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
        return item->index();
    }
    kDebug() << "Not found and not inserting... Returning invalid index";
    return QModelIndex();
}

QVariant DBusFunctionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
            case 0:
                return i18n("Function");
            case 1:
                return i18n("Parameters");
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
        itemList.append(new QStandardItem(QString(QVariant::typeToName(arg.value().type())) + ": " + arg.description()));
        itemList.append(new ArgumentsModelItem(arg));
        appendRow(itemList);
    }
    
}

QVariant ArgumentsModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
        argList.append(qVariantValue<Argument>(item(i, 1)->data(Qt::EditRole)));
    }
    return argList;
}


/*
***********************************
ArgumentDelegate
***********************************
*/

ArgumentDelegate::ArgumentDelegate(QObject *parent): QItemDelegate(parent) {
}

QWidget *ArgumentDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &index) const {
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

void ArgumentDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

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

void ArgumentDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
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

void ArgumentDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const {
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
//        kDebug() << "got arg:" << arg.description() << "with type" << arg.value();
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

ProfileModel::ProfileModel(QObject *parent): QStandardItemModel(parent) {
    setHorizontalHeaderLabels(QStringList() << i18n("Profile Name"));
    foreach(Profile *profile, ProfileServer::allProfiles()){
        QStandardItem *item = new QStandardItem(profile->name());
        item->setData(qVariantFromValue(profile), Qt::UserRole);
        appendRow(item);
    }    
}

Profile* ProfileModel::profile(const QModelIndex& index) const {
    if(index.isValid()){
        return qVariantValue<Profile*>(index.data(Qt::UserRole));
    }
    return 0;
}

QModelIndex ProfileModel::find(const ProfileAction* action) const {
    for(int i = 0; i < rowCount(); i++){
        kDebug() << "checking item" << item(i)->data(Qt::UserRole).value<Profile*>()->profileId() << "for" << action->profileId();
        QStandardItem *profileItem = item(i);
        if(profileItem->data(Qt::UserRole).value<Profile*>()->profileId() == action->profileId()){
            return profileItem->index();
        }
    }
    // Not found...
    return QModelIndex();    
}


/*
***********************************
ActionTemplateModel
***********************************
*/

ActionTemplateModel::ActionTemplateModel(QObject* parent): QStandardItemModel(parent) {
}

ActionTemplateModel::ActionTemplateModel(const Profile* profile, QObject* parent): QStandardItemModel(parent) {
    refresh(profile);
}

void ActionTemplateModel::refresh(const Profile* profile) {
    clear();
    foreach(ProfileActionTemplate profileActionTemplate, profile->actionTemplates()){
      appendRow(profileActionTemplate);
    }
    sort(0, Qt::DescendingOrder);
}

QVariant ActionTemplateModel::headerData(int section, Qt::Orientation orientation, int role) const {
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

ProfileActionTemplate ActionTemplateModel::actionTemplate(const QModelIndex &index) const {
    return item(index.row())->data(Qt::UserRole).value<ProfileActionTemplate>();
}

QModelIndex ActionTemplateModel::find(const ProfileAction* action) const {
    for(int i = 0; i < rowCount(); i++){
        QStandardItem *templateItem = item(i);
        if(templateItem->data(Qt::UserRole).value<ProfileActionTemplate>().actionTemplateId() == action->actionTemplateId()){
            return templateItem->index();
        }
    }
    // Not found...
    return QModelIndex();
}

void ActionTemplateModel::appendRow(ProfileActionTemplate actionTemplate) {
    QList<QStandardItem*> row;
    QStandardItem *item = new QStandardItem(actionTemplate.actionName());
    item->setData(qVariantFromValue(actionTemplate), Qt::UserRole);
    row.append(item);

    if (!(actionTemplate.description().isEmpty())) {
        QStandardItem *tItem = new  QStandardItem(actionTemplate.description());
        tItem->setToolTip(actionTemplate.description());
        row.append(tItem);
    } else {
        row.append(new QStandardItem("-"));
    }
    row.append(new QStandardItem(QString::number(actionTemplate.function().args().size())));
    if (!actionTemplate.buttonName().isEmpty()) {
        row.append(new QStandardItem(actionTemplate.buttonName()));
    } else {
        row.append(new QStandardItem("-"));
    }
    QStandardItemModel::appendRow(row);
}

Qt::ItemFlags ActionTemplateModel::flags(const QModelIndex& index) const {
    return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable);
}


/*
***********************************
RemoteModel
***********************************
*/

RemoteModel::RemoteModel(QObject* parent): QStandardItemModel(parent) {
}

RemoteModel::RemoteModel(const RemoteList &remoteList, QObject *parent) : QStandardItemModel(parent) {
    refresh(remoteList);
}

void RemoteModel::refresh(const RemoteList &remoteList) {
    clear();
    setHorizontalHeaderLabels(QStringList() << i18n("Remotes and modes") << i18n("Button"));
    foreach(Remote *remote, remoteList){
        QList<QStandardItem*> itemList;
        QStandardItem *item = new RemoteItem(remote);
        itemList.append(item);
        item = new QStandardItem();
        item->setData(qVariantFromValue(remote), Qt::UserRole);
        itemList.append(item);
        appendRow(itemList);
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
    if(index.isValid()){
        return qVariantValue<Remote*>(index.data(Qt::UserRole))->masterMode();
    }
    return 0;
}

QModelIndex RemoteModel::find(Mode* mode) const {
    for(int i = 0; i < rowCount(); i++){
        QStandardItem *remoteItem = itemFromIndex(index(i, 0));
        if(remoteItem->data(Qt::UserRole).value<Remote*>()->masterMode() == mode){
            return remoteItem->index();
        }
        for(int j = 0; j < rowCount(remoteItem->index()); j++){
            QStandardItem *modeItem = itemFromIndex(index(j, 0, remoteItem->index()));
            if(modeItem->data(Qt::UserRole).value<Mode*>() == mode){
                return modeItem->index();
            }
        }
    }
    // Not found...
    return QModelIndex();
}

QVariant RemoteModel::data(const QModelIndex& index, int role) const {
    if(index.isValid() && index.parent().isValid()){
        if(role == Qt::DisplayRole){
            switch(index.column()){
                case 0:
                    return mode(index)->name();
                case 1:
                    return mode(index)->button();
            }
        }
    }
    return QStandardItemModel::data(index, role);
}

Qt::ItemFlags RemoteModel::flags(const QModelIndex& index) const {
    if(index.isValid()) {
        return (QStandardItemModel::flags(index) | Qt::ItemIsDropEnabled);
    }

    return QStandardItemModel::flags(index);
}

bool RemoteModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (!data->hasFormat("kremotecontrol/action"))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

    QByteArray encodedData = data->data("kremotecontrol/action");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    quint64 actionPointer;
    stream >> actionPointer;
    Action *droppedAction = reinterpret_cast<Action*>(actionPointer);
    kDebug() << "action pointer is" << droppedAction << "name is" << droppedAction->name();
    
    mode(parent)->addAction(droppedAction->clone());
    emit modeChanged(mode(parent));
    
    return true;
}

QStringList RemoteModel::mimeTypes() const {
    QStringList types;
    types << "kremotecontrol/action";
    return types;
}

Qt::DropActions RemoteModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

RemoteItem::RemoteItem(Remote *remote) {
    qRegisterMetaType<Remote*>("Remote*");
    qRegisterMetaType<Mode*>("Mode*");
    setData(qVariantFromValue(remote), Qt::UserRole);
    foreach(Mode *mode, remote->allModes()) {
        if(mode->name() != "Master"){ // Dont show the Master Mode separately
            QList<QStandardItem*> itemList;
            QStandardItem *item = new QStandardItem(mode->name());
            item->setData(qVariantFromValue(mode), Qt::UserRole);
            if(remote->defaultMode() == mode){
                QFont font = KApplication::font();
                font.setBold(true);
                item->setFont(font);
            }
            item->setIcon(KIcon(mode->iconName()));
            itemList.append(item);
            item = new QStandardItem(mode->name());
            item->setData(qVariantFromValue(mode), Qt::UserRole);
            itemList.append(item);
            appendRow(itemList);
        }
    }
}

QVariant RemoteItem::data(int role) const {
    Remote *remote = qVariantValue<Remote*>(QStandardItem::data(Qt::UserRole));
    if(role == Qt::DisplayRole) {
        return remote->name();
    }
    if(role == Qt::DecorationRole){
        return KIcon(remote->masterMode()->iconName());
    }
    return QStandardItem::data(role);
}


ActionModel::ActionModel(QObject *parent): QStandardItemModel(parent) {
    setHorizontalHeaderLabels(QStringList() << i18n("Button") << i18n("Application") << i18n("Function"));
}


void ActionModel::refresh(Mode* mode) {
    m_mode = mode;
    clear();
    setHorizontalHeaderLabels(QStringList() << i18n("Button") << i18n("Application") << i18n("Function"));
    foreach(Action *action, mode->actions()){
        QStandardItem *item = new QStandardItem();
        item->setData(qVariantFromValue(action), Qt::UserRole);
        appendRow(item);
    }
}


QVariant ActionModel::data(const QModelIndex& index, int role) const {
    if(role == Qt::DisplayRole){
        Action *action = qVariantValue<Action*>(item(index.row())->data(Qt::UserRole));
        switch(index.column()){
            case 0:
                return action->button();
            case 1:
                return action->name();
            case 2:
                return action->description();
        }
    }
    return QStandardItemModel::data(index, role);
}

Action* ActionModel::action(const QModelIndex& index) const {
    if(index.isValid()){
        return qVariantValue<Action*>(item(index.row())->data(Qt::UserRole));
    }
    return 0;
}

QModelIndex ActionModel::find(Action* action) const {
    for(int i = 0; i < rowCount(); i++){
        QModelIndex actionIndex = index(i, 0);
        QStandardItem *actionItem = itemFromIndex(actionIndex);
        if(actionItem->data(Qt::UserRole).value<Action*>() == action){
            return actionItem->index();
        }
    }
    // Not found...
    return QModelIndex();
}

Qt::ItemFlags ActionModel::flags(const QModelIndex &index) const {
    if (index.isValid()) {
        return (QStandardItemModel::flags(index) | Qt::ItemIsDragEnabled);
    }

    return QStandardItemModel::flags(index);
}

QMimeData *ActionModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    QModelIndex index = indexes.first(); // Only need column 0
    if(index.isValid()) {
        Action *dragAction = action(index);
        kDebug() << "index:" << index << "dragging action pointer is" << dragAction << "name is" << dragAction->name();
        quint64 actionPointer = reinterpret_cast<quint64>(dragAction);
        stream << actionPointer;
    }

    mimeData->setData("kremotecontrol/action", encodedData);
    return mimeData;
}

bool ActionModel::removeRows(int row, int col, const QModelIndex& parent) {
    kDebug() << "rmove Rows called";
    m_mode->removeAction(action(index(row, col, parent)));
    return QStandardItemModel::removeRows(row, col, parent);
}

Qt::DropActions ActionModel::supportedDragActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

