#ifndef TRANSLATIONSTRING_H
#define TRANSLATIONSTRING_H

#include <QObject>

typedef struct {
    QString MAINSETTING_003;

} TRANSLATIONSTR;


class TranslationString
{
public:
    TranslationString();
    void SetString();
    void SetFilename(std::string _filename);


    static TranslationString& GetString();

    TRANSLATIONSTR translation_str;
};

#endif // TRANSLATIONSTRING_H
