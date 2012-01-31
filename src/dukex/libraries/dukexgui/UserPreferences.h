#ifndef USERPREFERENCES_H_
#define USERPREFERENCES_H_

#include <QObject>
#include <QSettings>
#include <QMenu>
#include <vector>

class UserPreferences : public QObject {

Q_OBJECT

public:
    UserPreferences();

public:
    void loadShortcuts(QObject*);
    void saveShortcuts(QObject*);

    void loadFileHistory();
    void saveFileHistory();
    const std::vector<std::string> & history();
    const std::string history(size_t);
    void addToHistory(const std::string&);
    void removeFromHistory(const std::string&);

private:
    QSettings m_Settings;
    static const size_t m_MaxRecentFiles = 10;
    std::vector<std::string> m_vRecentFiles;
    std::vector<std::string> m_vAutoLoadPlugins;
};

#endif // USERPREFERENCES_H_
