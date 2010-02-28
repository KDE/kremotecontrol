/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kremotecontrolapplet.h"
 

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QIcon>
#include <QDBusConnection>
#include <QGraphicsGridLayout>

#include <Plasma/Svg>
#include <Plasma/Theme>
#include <Plasma/Animator>
#include <Plasma/Extender>
#include <Plasma/ExtenderItem>
#include <Plasma/PopupApplet>
#include <Plasma/Label>
#include <Plasma/Separator>
#include <Plasma/Slider>
#include <Plasma/PushButton>
#include <Plasma/CheckBox>
#include <Plasma/ComboBox>
#include <Plasma/IconWidget>
 
 
 class RemoteWidget {
  
   private:
    Plasma::Label *m_name;
    Plasma::CheckBox *m_eventsIgnored;
    Plasma::ComboBox *m_modes;
    QGraphicsGridLayout *m_layout;
   public:
    RemoteWidget(QGraphicsWidget *parent, const QString &name, const QStringList &modes, const QString &currentMode, bool eventsIgnored );
    QGraphicsGridLayout  *layout();
};
 
 
RemoteWidget::RemoteWidget(QGraphicsWidget *parent, const QString& name, const QStringList& modes, const QString& currentMode, bool eventsIgnored) {
      m_layout = new QGraphicsGridLayout();
      m_name = new Plasma::Label(parent); 
      m_name->setAlignment(Qt::AlignRight);
      m_name->setText(name);
      m_modes = new Plasma::ComboBox(parent);      
      m_modes->addItem(currentMode);
      foreach(const QString mode, modes){
	if(mode != currentMode){
	  m_modes->addItem(mode);
	}
      }
      m_eventsIgnored = new Plasma::CheckBox(parent);
      m_eventsIgnored->setChecked(eventsIgnored);
      m_layout->addItem(m_name,0,0);
      m_layout->addItem(m_name,0,1);
      m_layout->addItem(m_modes,0,2);
      m_layout->addItem(m_eventsIgnored,0,3);      
}

QGraphicsGridLayout* RemoteWidget::layout() {
  return m_layout;
}


 
KRemotecontrolApplet::KRemotecontrolApplet(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args), m_isEmbedded(false) {
    m_svg = new Plasma::Svg(this);
    m_svg->setImagePath("widgets/background");
    setBackgroundHints(DefaultBackground);
    setPopupIcon(QIcon());
    resize(128, 128);
    m_svg->setImagePath("widgets/hisc-app-krcd");
    m_svg->setContainsMultipleImages(true);
    setStatus(Plasma::ActiveStatus);
}
 
 
KRemotecontrolApplet::~KRemotecontrolApplet()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
    }
}
 
void KRemotecontrolApplet::init()
{
 
    // A small demonstration of the setFailedToLaunch function
//     if (m_icon.isNull()) {
//         setFailedToLaunch(true, "No world to say hello");
//     }
    if (!m_isEmbedded && extender() && !extender()->hasItem("remotemanagement")) {
        Plasma::ExtenderItem *eItem = new Plasma::ExtenderItem(extender());
        eItem->setName("remotemanagement");
        initExtenderItem(eItem);
        extender()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
     m_pulseAnimation = Plasma::Animator::create(Plasma::Animator::PulseAnimation);
     m_pulseAnimation->setTargetWidget(this);
     connectSignalsAndSlots(); 
} 

void KRemotecontrolApplet::connectSignalsAndSlots() {
    const QStringList& remote_sources = dataEngine("kremotecontrol")->query("remotes")["remotes"].toStringList();
    foreach (const QString &remote_source, remote_sources) {
	m_remotes <<remote_source;
        dataUpdated(remote_source, dataEngine("kremotecontrol")->query(remote_source));
    } 
    QDBusConnection::sessionBus().connect("org.kde.kded", "/modules/kremotecontrol", "org.kde.krcd", "buttonPressed",  this, SLOT(flashIcon()));
}

 
void KRemotecontrolApplet::paintInterface(QPainter* painter, const QStyleOptionGraphicsItem* option, const QRect& contentsRect)
{
    int minSize = qMin(contentsRect.height(), contentsRect.width());
    QRect contentsSquare = QRect(contentsRect.x() + (contentsRect.width() - minSize) / 2, contentsRect.y() + (contentsRect.height() - minSize) / 2, minSize, minSize);
    m_svg->paint(painter, contentsSquare);       
}

 
void KRemotecontrolApplet::initExtenderItem(Plasma::ExtenderItem *item)  {
    if (!m_isEmbedded && item->name() == "remotemanagement") {
      m_controls = new QGraphicsWidget(item);
      m_controls->setMinimumWidth(360);
      m_extenderLayout = new QGraphicsGridLayout(item);
      m_controls->setLayout(m_extenderLayout);
      item->setWidget(m_controls);
//       item->setTitle(i18n("Remote Control Management"));
      m_isEmbedded=true;
    }
}

void KRemotecontrolApplet::updateStatus(const QString& remoteName, const QStringList& modes, const QString& currentMode, bool eventsIgnored) {
  RemoteWidget *widget = new RemoteWidget(m_controls, remoteName, modes, currentMode, eventsIgnored);  
  m_extenderLayout->addItem(widget->layout(),1,0);
}


void KRemotecontrolApplet::dataUpdated(const QString& source, const Plasma::DataEngine::Data& data) {
  
    if (source.startsWith(QLatin1String("remotes"))) {
      m_remotes.clear();
      m_remotes = data["remotes"].toStringList();
    }
    else if( m_remotes.contains(source)){
      updateStatus(source, data["modes"].toStringList(),data["currentModes"].toString(),data["eventsIgnored"].toBool());
    }
}

void KRemotecontrolApplet::enableIcon(bool enable) {
     
}

void KRemotecontrolApplet::flashIcon() {
  m_pulseAnimation->start();
}

 
 
#include "kremotecontrolapplet.moc"