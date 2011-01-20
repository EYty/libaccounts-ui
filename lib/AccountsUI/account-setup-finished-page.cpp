/*
 * This file is part of accounts-ui
 *
 * Copyright (C) 2009-2010 Nokia Corporation.
 *
 * Contact: Alberto Mardegan <alberto.mardegan@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

//libAccounts
#include <Accounts/Provider>
#include <Accounts/Account>
#include <Accounts/ServiceType>
#include <Accounts/Manager>

//Meegotouch
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MContentItem>
#include <MLocale>
#include <MAction>
#include <MLabel>
#include <MGridLayoutPolicy>
#include <MSeparator>
#include <MImageWidget>
#include <MButton>

//Qt
#include <QDebug>

//Accounts-Ui
#include "account-setup-finished-page-priv.h"
#include "accountsmanagersingleton.h"
#include "last-page-actions.h"
#include "provider-plugin-process.h"
#include "account-setup-finished-widget.h"

namespace AccountsUI {

void AccountSetupFinishedPagePrivate::actionButtonClicked()
{
    QString serviceName = sender()->property("serviceName").toString();
    qDebug() << "Invoking service" << serviceName;
    LastPageActions::executeService(serviceName);
    ProviderPluginProcess::instance()->quit();
}

AccountSetupFinishedPage::AccountSetupFinishedPage(AbstractAccountSetupContext *context)
        : MApplicationPage(),
          d_ptr(new AccountSetupFinishedPagePrivate())
{
    Q_D(AccountSetupFinishedPage);
    d->q_ptr = this;
    d->account = context->account();
    d->serviceType = context->serviceType();
    setObjectName("AccountSetupFinishedPage");
}

AccountSetupFinishedPage::~AccountSetupFinishedPage()
{
    delete d_ptr;
}

void AccountSetupFinishedPage::createContent()
{
    Q_D(AccountSetupFinishedPage);

    setComponentsDisplayMode(EscapeButton, MApplicationPageModel::Hide);

    MWidget *centralWidget = new MWidget();
    MLayout *layout = new MLayout(centralWidget);
    MLinearLayoutPolicy *layoutPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    layoutPolicy->setSpacing(0);

    QString providerName(d->account->providerName());
    // xml file that describes the ui elements for the provider
    Accounts::Provider *provider = AccountsManager::instance()->provider(providerName);
    if (provider) {
        QDomElement root = provider->domDocument().documentElement();
        QDomElement providerIcon = root.firstChildElement("icon");
        QString providerIconId = providerIcon.text();

        MContentItem *providerItem = new MContentItem(MContentItem::IconAndSingleTextLabel, this);
        providerItem->setStyleName("SetupFinishedProviderName");
        providerItem->setImageID(providerIconId);
        providerItem->setTitle(provider->displayName());
        layoutPolicy->addItem(providerItem);
    }

    MSeparator *separatorTop = new MSeparator(this);
    separatorTop->setOrientation(Qt::Horizontal);

    layoutPolicy->addItem(separatorTop);

    AccountSetupFinishedWidget *widget = new AccountSetupFinishedWidget(providerName, this);
    layoutPolicy->addItem(widget);

    //% "Add more account"
    MButton *addMoreAccountButton = new MButton(qtTrId("qtn_acc_add_more_accounts"));
    connect (addMoreAccountButton, SIGNAL(clicked()), ProviderPluginProcess::instance(), SLOT(quit()));
    MLayout *buttonsLayout = new MLayout();
    MLinearLayoutPolicy *portraitPolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Vertical);
    MLinearLayoutPolicy *landscapePolicy = new MLinearLayoutPolicy(buttonsLayout, Qt::Horizontal);

    portraitPolicy->addStretch();
    portraitPolicy->setSpacing(20);
    landscapePolicy->addStretch();
    landscapePolicy->setSpacing(20);

    const LastPageActions &lastPageActions =
        ProviderPluginProcess::instance()->lastPageActions();
    const LastPageActions::ServiceActionList actions =
        lastPageActions.serviceActions();
    foreach (LastPageActions::ServiceAction action, actions) {
        MButton *button = new MButton(qtTrId("qtn_comm_go_to_x").
                                      arg(action.title()));
        button->setProperty("serviceName", action.serviceName());
        connect(button, SIGNAL(clicked()), d, SLOT(actionButtonClicked()));

        landscapePolicy->addItem(button, Qt::AlignRight);
        portraitPolicy->addItem(button, Qt::AlignCenter);
    }

    portraitPolicy->addItem(addMoreAccountButton, Qt::AlignCenter);
    landscapePolicy->addItem(addMoreAccountButton, Qt::AlignCenter);

    portraitPolicy->addStretch();
    landscapePolicy->addStretch();

    buttonsLayout->setLandscapePolicy(landscapePolicy);
    buttonsLayout->setPortraitPolicy(portraitPolicy);
    layoutPolicy->addStretch();
    layoutPolicy->addItem(buttonsLayout);

    setCentralWidget(centralWidget);

}

void AccountSetupFinishedPage::goToApplication()
{
    /* this slot is deprecated */
}

}
