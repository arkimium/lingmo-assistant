#include "ProviderManager.h"
#include "AIProvider.h"
#include "OpenAICompatProvider.h"
#include "OllamaProvider.h"
#include "LlamaCppProvider.h"
#include "../core/Settings.h"
#include <QVariantMap>
#include <QVariantList>

ProviderManager::ProviderManager(QObject *parent)
    : QObject(parent)
{
}

void ProviderManager::init(Settings *settings) {
    m_settings = settings;

    auto *deepseek = new OpenAICompatProvider("deepseek", "DeepSeek",
        "DeepSeek V4 ¡ª 1M context, powerful reasoning", true, false, 1048576, this);
    deepseek->setEndpoint(settings->deepseekEndpoint(), "/chat/completions",
                          settings->apiKey("deepseek"));
    deepseek->setDefaultModels({"deepseek-chat", "deepseek-reasoner"});
    m_providers.append(deepseek);

    auto *ollama = new OllamaProvider("ollama", "Ollama (Local)",
        "Local models via Ollama ¡ª free, private", false, true, this);
    ollama->setEndpoint(settings->ollamaEndpoint());
    m_providers.append(ollama);

    auto *ollamaCloud = new OllamaProvider("ollama-cloud", "Ollama Cloud",
        "Cloud-hosted Ollama models", false, false, this);
    ollamaCloud->setEndpoint(settings->ollamaCloudEndpoint());
    m_providers.append(ollamaCloud);

    auto *llamaCpp = new LlamaCppProvider("llama-cpp", "llama.cpp (Local)",
        "Run GGUF models locally via llama.cpp", false, true, this);
    m_providers.append(llamaCpp);

    emit providersChanged();
}

QStringList ProviderManager::providerIds() const {
    QStringList ids;
    for (auto *p : m_providers) ids.append(p->id());
    return ids;
}

AIProvider *ProviderManager::provider(const QString &id) const {
    for (auto *p : m_providers) {
        if (p->id() == id) return p;
    }
    return nullptr;
}

QVariantList ProviderManager::providerList() const {
    QVariantList list;
    for (auto *p : m_providers) {
        QVariantMap m;
        m["id"] = p->id();
        m["name"] = p->displayName();
        m["description"] = p->description();
        m["requiresApiKey"] = p->requiresApiKey();
        m["isLocal"] = p->isLocal();
        m["isAvailable"] = p->isAvailable();
        list.append(m);
    }
    return list;
}

void ProviderManager::setActiveProvider(const QString &id) {
    if (m_activeProvider != id) {
        m_activeProvider = id;
        if (m_settings) {
            m_settings->setActiveProvider(id);
        }
        emit activeProviderChanged();
    }
}

QString ProviderManager::activeProviderId() const {
    return m_activeProvider.isEmpty() ? "deepseek" : m_activeProvider;
}

QStringList ProviderManager::activeModels() const {
    auto *p = provider(m_activeProvider);
    if (p) return p->defaultModels();
    return {};
}
