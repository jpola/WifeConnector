#ifndef NETWORKPROPERIESUTILS_HPP
#define NETWORKPROPERIESUTILS_HPP

#include <QtNetwork/QNetworkConfiguration>
#include <QString>

QString getStrNetConfigState(const QNetworkConfiguration::StateFlags flag)
{
    using NetCfg = QNetworkConfiguration;

    switch (flag) {
    case NetCfg::Active:
        return "Active";
        break;
    case NetCfg::Discovered:
        return "Discovered";
        break;
    case NetCfg::Defined:
        return "Defined";
        break;
    case NetCfg::Undefined:
        return "Undefined";
        break;
    default:
        return "Unknown";
        break;
    }
}

QString getStrNetConfigType(const QNetworkConfiguration::Type type)
{
    using NetCfg = QNetworkConfiguration;

    switch (type) {
    case NetCfg::InternetAccessPoint:
        return "Internet Access Point";
        break;
    case NetCfg::ServiceNetwork:
        return "Service Network";
        break;
    case NetCfg::UserChoice:
        return "Users Choice";
        break;
    case NetCfg::Invalid:
        return "Invalid";
        break;
    default:
        return "Unknown";
        break;
    }
}


#endif // NETWORKPROPERIESUTILS_HPP
