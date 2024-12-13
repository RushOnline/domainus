#ifndef __DOMAINUS_APPLICATION_H__
#define __DOMAINUS_APPLICATION_H__

#include <cstdint>
#include <memory>

#include <tgbot/tgbot.h>

#include "utils.h"

class Application {
public:
    using BotPtr = std::unique_ptr<TgBot::Bot>;
    static Application& getInstance();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    ~Application() = default;


    void init(int argc, char* argv[]);
    int run();

    bool addDomains(const StringList& domains);
    void sync();
    StringList findDomains(const String& mask);
    void dumpDomains();

private:
    explicit Application();

    inline TgBot::Bot& getBot() { return *_bot; }
    inline const TgBot::Api& getBotApi() { return _bot->getApi(); }
    inline TgBot::EventBroadcaster& getBotEvents() { return _bot->getEvents(); }

    BotPtr      _bot;               //< pointer to bot object
    StringList  _domains;           //< domains
    String      _dbpath;            //< domains filename
    String      _cfpath;            //< dnsmasq domains config
    String      _restart_script;    //< restart script filename
    std::list<int64_t> _chatids;        //< authorized chat ids
};

#endif//__DOMAINUS_APPLICATION_H__
