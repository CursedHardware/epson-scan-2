#include "translationstring.h"

static TranslationString translation;

TranslationString::TranslationString()
{
}

TranslationString& TranslationString::GetString()
{
    return translation;
}

void TranslationString::SetString()
{
    TRANSLATIONSTR temp_translation;

    temp_translation.MAINSETTING_003 = QObject::tr("MAINSETTING_003");

    this->translation_str = temp_translation;
}
