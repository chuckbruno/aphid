#ifndef TTG_WINDOW_H
#define TTG_WINDOW_H

#include <QMainWindow>
#include "Parameter.h"
QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

namespace ttg {

class GLWidget;
class SuperformulaControl;
class RedBlueControl;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(const ttg::Parameter * param);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
	void createActions(Parameter::Operation opt);
	void createMenus(Parameter::Operation opt);
	
private:
    GLWidget *glWidget;
	QMenu * windowMenu;
	SuperformulaControl * m_superformulaControl;
	QAction * showSFControlAct;
	RedBlueControl * m_redblueControl;
	QAction * showRBControlAct;
	
};

}
#endif
