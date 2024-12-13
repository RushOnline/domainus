#include <cstdlib>
#include <fstream>
#include <sstream>

#include <fmt/core.h>
#include <curl/curl.h>

#include <boost/algorithm/string/join.hpp>

#include "Application.h"

#if 0
#if defined(__x86_64__)
#   define DOMAINS_FILENAME "domains.txt"
#   define DNSMASQ_CONFIG "domains-list.conf"
#else
#   define DOMAINS_FILENAME "/opt/etc/domains.txt"
#   define DNSMASQ_CONFIG "/etc/storage/dnsmasq/domains-list.conf"
#endif
#endif

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

Application::Application()
{
}

Application &Application::getInstance()
{
    static Application instance;
    return instance;
}

void Application::init(int argc, char *argv[])
{
    auto dbpath = ::getenv("DOMAINUS_DBPATH");
    if (!dbpath) throw std::runtime_error("DOMAINUS_DBPATH env var not set");
    _dbpath = dbpath;

    auto token = ::getenv("DOMAINUS_TOKEN");
    if (!token) throw std::runtime_error("DOMAINUS_TOKEN env var not set");

    auto cfpath = ::getenv("DOMAINUS_CFPATH");
    if (!cfpath) throw std::runtime_error("DOMAINUS_CFPATH env var not set");
    _cfpath = cfpath;

    auto restart_script = ::getenv("DOMAINUS_DNSMASQ_RESTART_SCRIPT");
    if (!restart_script) throw std::runtime_error("DOMAINUS_DNSMASQ_RESTART_SCRIPT env var not set");
    _restart_script = restart_script;

    std::ifstream ifs(_dbpath);

    if (!ifs)
        throw std::runtime_error("can't open file: " + std::string(_dbpath));

    String line;
    while (std::getline(ifs, line)) {
        ltrim(line);
        rtrim(line);
        _domains.push_back(line);
    }

    _bot.reset(new TgBot::Bot(token));
}

int Application::run()
{
    getBotEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        getBotApi().sendMessage(message->chat->id, "Hi!");
    });

    getBotEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "удали")) {

            std::istringstream ss(message->text.substr(String("удали ").size()));
            StringList domains;
            String domain;

            while (ss >> domain) {
                if (std::find(_domains.begin(), _domains.end(), domain) != _domains.end()) {
                    domains.push_back(domain);
                }
            }

            for (const auto &domain : domains) {
                _domains.remove(domain);
            }

            sync();

            String response = "Удалены домены:\n\n" + boost::algorithm::join(domains, "\n");

            getBotApi().sendMessage(message->chat->id, response);
            return;
        } else if (StringTools::startsWith(message->text, "ищи")) {
            size_t n = String("ищи ").size();
            String mask = (message->text.size() <= n) ? "" : message->text.substr(String("ищи ").size());

            auto domains = findDomains(mask);

            String response = "Найдены домены:\n\n" + boost::algorithm::join(domains, "\n");
            getBotApi().sendMessage(message->chat->id, response);
        } else {
            std::istringstream ss(message->text);
            StringList domains;
            String url;

            while (ss >> url) {
                String domain;
                if (get_domain(url, domain))
                {
                    auto start = domain.rfind(".");
                    if ((start != String::npos) && (start > 0))
                    {
                        start = domain.rfind(".", start - 1);
                        if (start != String::npos)
                        {
                            domain = domain.substr(start + 1);
                        }
                    }
                    domains.push_back(domain);
                }
            }

            if (domains.empty())
            {
                getBotApi().sendMessage(message->chat->id, "Доступные команды:\n\nдобавь <список доменов>\nудали <список доменов>\nищи <подстрока>\nМожно просто отправить сообщение со ссылками.\n");
            } else {
                addDomains(domains);
                getBotApi().sendMessage(message->chat->id, "Добавлены домены:\n" + boost::algorithm::join(domains, "\n"));
            }
        }
    });

    try {
        printf("Bot username: %s\n", _bot->getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(*_bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}

bool Application::addDomains(const StringList &domains)
{
    for (const auto &domain : domains) {
        if (std::find(_domains.begin(), _domains.end(), domain) == _domains.end()) {
            _domains.push_back(domain);
        }
    }

    sync();

    return true;
}

void Application::sync()
{
    std::ofstream dbf(_dbpath), cfg(_cfpath);

    if (!dbf) throw std::runtime_error("can't open file: " + _dbpath);

    if (!cfg) throw std::runtime_error("can't open file: " + _cfpath);

    for (const auto &domain : _domains) {
        dbf << domain << std::endl;
        cfg << fmt::format("ipset=/{}/rkn", domain) << std::endl;
    }

    system(_restart_script.c_str());
}

StringList Application::findDomains(const String &mask)
{
    if (mask.empty()) return _domains;

    StringList result;

    for (const auto &domain : _domains) {
        if (domain.find(mask) != String::npos) {
            result.push_back(domain);
        }
    }

    return result;
}

void Application::dumpDomains()
{
    printf("Domains:\n");
    for (const auto &domain : _domains) {
        printf("%s\n", domain.c_str());
    }
}
