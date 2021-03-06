//
// Created by michael on 15.08.15.
//

#include <Template/Space/SpaceQuestTemplateFactory.h>
#include <Template/Space/ExploreRegionTemplate.h>
#include <Template/Space/ScanPlanetTemplate.h>
#include <Template/Space/HuntAndKillTemplate.h>

using namespace std;
using namespace Json;
using namespace weave;

std::shared_ptr<QuestTemplate> SpaceQuestTemplateFactory::createFromJsonValues(const Json::Value &root) const {
    if (root["parent"].asString() != "Space") {
        string errorMessage = string("Template file has incompatible parent: ") + root["parent"].asString() + "\n";
        Logger::Fatal(errorMessage);
    }

    string title = extractTitle(root, randomStream);
    vector<TemplateQuestProperty> properties = extractProperties(root);
    vector<TemplateQuestDescription> descriptions = extractDescriptions(root);
    int rarity = extractRarity(root);

    const string &templateKey = root["key"].asString();
    if (templateKey == "ExploreRegionQuest") {
        return make_shared<ExploreRegionTemplate>(title, properties, descriptions, formatterType, rarity);
    } else if (templateKey == "ScanPlanetQuest") {
        return make_shared<ScanPlanetTemplate>(title, properties, descriptions, formatterType, rarity);
    } else if (templateKey == "HuntAndKillQuest") {
        return make_shared<HuntAndKillTemplate>(title, properties, descriptions, formatterType, rarity);
    } else {
        auto ex = ContractFailedException("Unknown Space template key " + templateKey);
        Logger::Fatal(ex);
        throw ex;
    }
}

std::vector<const char *> SpaceQuestTemplateFactory::getTemplateFiles() const {
    return {"Space/ExploreRegionTemplate.qt", "Space/ScanPlanetTemplate.qt", "Space/HuntAndKillTemplate.qt"};
}




