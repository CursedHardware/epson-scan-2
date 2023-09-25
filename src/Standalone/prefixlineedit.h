#ifndef PREFIXLINEEDIT_H
#define PREFIXLINEEDIT_H

#include <QLineEdit>

#define DEFAULT_PREFIX "img"

class PrefixLineedit : public QLineEdit
{
public:
    PrefixLineedit(QWidget *parent = 0);

    QString current_prefix;

    void initialize(std::string default_prefix);

    QString set_prefix(QString prefix);

    void get_prefix(std::string* prefix);

    //void update_ui(SCANITEM document_source_para);

    //void item_event(int value);

private:
    bool changed;

    void BanMessageBox(void);

    //void set_enabled(bool enabled);

    //void add_item(std::list<float> item_list);

    //void select_item(std::list<float> item_list, DocumentSource select);
};

#endif // PREFIXLINEEDIT_H
