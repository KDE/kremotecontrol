#ifndef _KCMLIRC_H_
#define _KCMLIRC_H_

#include <qstringlist.h>
#include <qmap.h>

#include <kcmodule.h>
#include <kaboutdata.h>

#include <dcopobject.h>

#include "kcmlircbase.h"
#include "iractions.h"
#include "modes.h"

class QListViewItem;

class KCMLirc: public KCModule, virtual public DCOPObject
{
	Q_OBJECT
	K_DCOP

private:
	KAboutData *myAboutData;
	KCMLircBase *theKCMLircBase;
	IRActions allActions;
	Modes allModes;
	QMap<QListViewItem *, QValueListIterator<IRAction> > actionMap;
	QMap<QListViewItem *, Mode > modeMap;

public slots:
	void configChanged();
	void updateActions();
	void updateModes();
	void slotAddMode();
	void slotRemoveMode();
	void slotAddAction();
	void slotRemoveAction();

// MOC_SKIP_BEGIN
k_dcop:
// MOC_SKIP_END
	// now just used as a proxy to AddAction class
	virtual void gotButton(QString remote, QString button);
signals:
	void haveButton(const QString &remote, const QString &button);

public:
	virtual void load();
	virtual void save();
	virtual void defaults();
	virtual int buttons();
	virtual QString quickHelp() const;
	virtual const KAboutData *aboutData() { return myAboutData; };

	KCMLirc(QWidget *parent = 0, const char *name = 0, QStringList args = QStringList());
	~KCMLirc();
};

#endif
