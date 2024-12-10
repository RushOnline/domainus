#include <stdio.h>
#include <stdlib.h>

#include <string>

#include <tgbot/tgbot.h>
#include <curl/curl.h>

using String = std::string;

bool get_domain(const String& url, String& domain)
{
    bool rv = false;
    CURLUcode rc;
    CURLU *curl = curl_url();
    rc = curl_url_set(curl, CURLUPART_URL, url.c_str(), 0);
    if (!rc)
    {
        char *comp;
        rc = curl_url_get(curl, CURLUPART_HOST, &comp, 0);
        if (!rc)
        {
            domain = comp;
            rv = true;
            curl_free(comp);
        }
        curl_url_cleanup(curl);
    }
    return rv;
}

int main() {
    TgBot::Bot bot(getenv("DOMAINUS_TOKEN"));

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });

    bot.getEvents().onCommand("add", [&bot](TgBot::Message::Ptr message) {
        String domain, url = message->text.substr(5);
        if (get_domain(url, domain)) {
            bot.getApi().sendMessage(message->chat->id, "Domain added: " + domain);
        } else {
            bot.getApi().sendMessage(message->chat->id, "Invalid url: " + message->text);
        }
    });

    bot.getEvents().onCommand("del", [&bot](TgBot::Message::Ptr message) {
        String domain, url = message->text.substr(5);
        if (get_domain(url, domain)) {
            bot.getApi().sendMessage(message->chat->id, "Domain removed: " + domain);
        } else {
            bot.getApi().sendMessage(message->chat->id, "Invalid url: " + message->text);
        }
    });
    
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
