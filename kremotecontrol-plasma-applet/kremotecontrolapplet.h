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

#ifndef KREMOTECONTROLAPPLET_H
#define KREMOTECONTROLAPPLET_H



#include <KIcon>

#include <Plasma/PopupApplet> 
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <Plasma/DataEngine>
#include <Plasma/Animation>
 
class QSizeF;
class QGraphicsGridLayout;

namespace Plasma
{
    class Svg;
    class Label;
    class ExtenderItem;
    class ComboBox;
    class Slider;
}
 
// Define our plasma Applet
class KRemotecontrolApplet : public Plasma::PopupApplet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        KRemotecontrolApplet(QObject *parent, const QVariantList &args);
        ~KRemotecontrolApplet();
 
        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
	void init();
	void initExtenderItem(Plasma::ExtenderItem *item);
    
    public Q_SLOTS:
	void dataUpdated(const QString& source, const Plasma::DataEngine::Data& data);
	void flashIcon();
	void enableIcon(bool enable);
	
    private:
        Plasma::Svg *m_svg;
	QGraphicsWidget *m_controls;    
	Plasma::Label *m_remoteLabel;
	bool m_isEmbedded;
	Plasma::Animation *m_pulseAnimation;
	void connectSignalsAndSlots();
	void updateStatus(const QString &remoteName, const QStringList &modes, const QString &currentMode, bool eventsIgnored);
	
	QStringList m_remotes;
	QGraphicsGridLayout *m_extenderLayout;
	
};


 
// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(kremotecontrolapplet, KRemotecontrolApplet);

#endif // KREMOTECONTROLAPPLET_H
