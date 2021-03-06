//
// Created by michael on 18.04.16.
//

#pragma once

#include <Story/StoryTemplate.h>

namespace weave {
    class PlanetIntroStoryTemplate : public StoryTemplate {
    public:
        explicit PlanetIntroStoryTemplate(std::string rawText) : StoryTemplate(rawText, {"planet"}) { }

        StoryTemplateResult CreateStory(const EntityMap &requiredEntities, const WeaverGraph &graph,
                                        const WorldModel &worldModel) const override;

        bool IsValid(const EntityMap &requiredEntities, const WeaverGraph &graph,
                     const WorldModel &worldModel) const override;

    private:
        std::string metaDataMarker = "introStoryDone";

        std::vector<std::shared_ptr<WorldEntity>> getValidEntities(const EntityMap &entityMap,
                                                                   const WorldModel &worldModel) const;
    };

}