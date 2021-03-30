#ifndef __USERS_H__
#define __USERS_H__

#include "ui/ui_usersdialog.h"

class UsersDialog2
    : public QDialog
    , public Ui::UsersDialog2
{
public:
    UsersDialog2(QWidget *parent = 0);
};

#endif
