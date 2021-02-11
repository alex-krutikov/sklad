#ifndef __TREBNAKLAD_H__
#define __TREBNAKLAD_H__

#include <QWidget>

#include "ui/ui_trebnaklad.h"

class TrebNakladWidget : public QWidget,
                         public Ui::TrebNakladWidget
{
  Q_OBJECT
public:
  TrebNakladWidget( QWidget *parent = 0 );
};

#endif
