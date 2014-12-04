#include "Colorizer.h"

struct TelegramColor
{
    QString color;
    QString name;
};

const int gPredefinedColorsCount = 8;
static TelegramColor gPredefinedColors[gPredefinedColorsCount] =
{
    {"#ee4928", "red"},
    {"#41a903", "green"},
    {"#e09602", "yellow"},
    {"#0f94ed", "blue"},
    {"#8f3bf7", "purple"},
    {"#fc4380", "pink"},
    {"#00a1c4", "cyan"},
    {"#eb7002", "orange"}
};

QString Colorizer::color(int id)
{
    return gPredefinedColors[id % gPredefinedColorsCount].color;
}

QString Colorizer::userPlaceholder(int id)
{
    return QString("app/native/assets/images/placeholders/user_placeholder_%1.png").arg(gPredefinedColors[id % gPredefinedColorsCount].name);
}

QString Colorizer::groupPlaceholder(int id)
{
    return QString("app/native/assets/images/placeholders/group_placeholder_%1.png").arg(gPredefinedColors[id % gPredefinedColorsCount].name);
}

QString Colorizer::broadcastPlaceholder(int id)
{
    return QString("app/native/assets/images/placeholders/broadcast_placeholder_%1.png").arg(gPredefinedColors[id % gPredefinedColorsCount].name);
}
