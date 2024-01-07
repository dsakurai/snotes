//
// Created by Daisuke Sakurai on 2024/01/07.
//

#ifndef SIMPLEMARKDOWN_ABOUT_THIS_APP_H
#define SIMPLEMARKDOWN_ABOUT_THIS_APP_H

#include <QString>
#include <QCoreApplication>

namespace about_this_app {
    // organization name and application name should be accessed through
    // QCoreApplication.
    inline
    auto setOrganizationName(const auto& name) {
        QCoreApplication::setOrganizationName(name);
    }
    
    inline
    auto setApplicationName(const auto& name) {
        QCoreApplication::setApplicationName(name);
    }
    
}

#endif //SIMPLEMARKDOWN_ABOUT_THIS_APP_H
