//
// Created by michael on 18.04.16.
//

#include <World/Space/Planet.h>
#include "Story/Space/PlanetIntroStoryTemplate.h"


using namespace weave;
using namespace std;

StoryTemplateResult PlanetIntroStoryTemplate::CreateStory(const EntityMap &requiredEntities, const WeaverGraph &,
                                                          const WorldModel &worldModel) const {
    auto entities = getValidEntities(requiredEntities, worldModel);
    if (entities.empty()) {
        Logger::Fatal("Invalid template call!");
    }
    StoryTemplateResult result;
    auto entity = entities[0];

    TokenToEntityMap tokenEntityMap;
    tokenEntityMap["1"] = {entity->GetId()};
    result.tokenMap = createTokenMapping(tokenEntityMap);
    result.rawText = rawText;

    MetaData markedData;
    markedData.SetValue(metaDataMarker, 1);
    result.worldActions.emplace_back(WorldActionType::UPDATE, entity, markedData);
    
    return result;
}

bool PlanetIntroStoryTemplate::IsValid(const EntityMap &requiredEntities, const WeaverGraph &,
                                       const WorldModel &worldModel) const {
    return getValidEntities(requiredEntities, worldModel).size() > 0;
}

vector<shared_ptr<WorldEntity>> PlanetIntroStoryTemplate::getValidEntities(const EntityMap &entityMap,
                                                                           const WorldModel &worldModel) const {
    vector<shared_ptr<WorldEntity>> result;
    auto mapIter = entityMap.find(Planet::Type);
    if (mapIter == entityMap.end()) {
        return result;
    }
    for (shared_ptr<WorldEntity> entity : mapIter->second) {
        ID id = entity->GetId();
        MetaData metaData = worldModel.GetMetaData(id);
        if (metaData.HasValue(metaDataMarker)) {
            continue;
        }
        result.push_back(entity);
    }
    return result;
}
