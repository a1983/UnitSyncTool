#ifndef SPRINGOPTION_H
#define SPRINGOPTION_H

#include <QVariant>
#include <QMap>

enum OptionType
{
	OT_UNDEFINED  = 0,
	OT_BOOLEAN    = 1,
	OT_LIST       = 2,
	OT_FLOAT      = 3,
	OT_STRING     = 4,
	OT_SECTION    = 5
};

enum SpringOptionIndex {
	SOI_KEY,
	SOI_NAME,
	SOI_SECTION,
	SOI_STYLE,
	SOI_DESCRIPTION,
	SOI_TYPE,

	SOI_DEFAULT,

	SOI_NUMBER_MIN,
	SOI_NUMBER_MAX,
	SOI_NUMBER_STEP,

	SOI_STRING_MAX_LENGHT,

	SOI_LIST
};

struct SpringOption : public QMap< SpringOptionIndex, QVariant >{
};

#endif // SPRINGOPTION_H
