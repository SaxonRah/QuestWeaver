//
// Created by michael on 11.08.15.
//

#pragma once

#include <cereal/access.hpp>
#include <string>
#include <memory>
#include "../Core/WeaverTypes.h"

namespace weave {
    enum QuestState {
        Proposed, Inactive, Active, Failed, Success
    };

    class Quest {
    public:
        const ID NoID = 0;

        Quest(const std::string &title,
              const std::string &description);

        QuestState GetState() const;

        std::string GetTitle() const;

        std::string GetDescription() const;

        ID GetId() const;

    protected:
        friend class QuestModel;

        Quest(ID id,
              QuestState state,
              const std::string &title,
              const std::string &description);

        virtual std::shared_ptr<Quest> setStateAndId(ID newId, QuestState newState) const = 0;


    private:
        ID id;
        QuestState state;
        std::string title;
        std::string description;

        friend class cereal::access;

        // serialization
        template<class Archive>
        void serialize(Archive &archive) {
            archive(id, state, title, description);
        }
    };
}
