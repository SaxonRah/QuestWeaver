//
// Created by michael on 26.10.15.
//

#pragma once

#include <cereal.h>
#include "../Core/WeaverTypes.h"
#include "../Core/WeaverUtils.h"

namespace weave {

    /*!
     * Holds information about a world entity.
     * World entities are considered stateless by the quest system, so any information about their involvement in
     * quests is saved as MetaData.
     *
     * @ingroup worldApi
     */
    class MetaData {
    public:
        /*!
         * Returns true if this container holds a value for the given key.
         */
        bool HasValue(const std::string &name) const;

        /*!
         * Returns the value associated with the given key.
         * If this container does not have a value associated with the key then 0 is returned.
         */
        int GetValue(const std::string &name) const;

        /*!
         * Saves a value for the given key in this container.
         */
        MetaData &SetValue(const std::string &name, int value);

        /*!
         * Returns the names of all keys this container has values for.
         */
        std::vector<std::string> GetValueNames() const;

    private:
        std::unordered_map<std::string, int> data;

        // serialization
        friend class cereal::access;

        template<class Archive>
        void serialize(Archive &archive) {
            archive(CEREAL_NVP(data));
        }
    };
}
