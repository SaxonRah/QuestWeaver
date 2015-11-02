//
// Created by michael on 12.10.15.
//

#include <fstream>
#include <typeinfo>
#include "../../QuestWeaver.h"
#include "../catch.hpp"
#include "../../Template/Space/SpaceQuestTemplateFactory.h"
#include "../../World/Space/SpaceWorldModel.h"

using namespace weave;
using namespace std;

TEST_CASE("Serialize Quests", "[serialize]") {
    int testSize = 100;
    shared_ptr<RandomStream> rs = make_shared<RandomStream>(42);
    SpaceQuestTemplateFactory factory(rs);

    SECTION("Serialize and deserialize JSON in-memory") {
        WorldModel *worldModel = new SpaceWorldModel(rs);
        for (int i = 0; i < testSize; i++) {
            rs->Seed(i);
            for (string templateKey : factory.GetTemplateKeys()) {
                auto tp = factory.CreateTemplate(templateKey);
                vector<QuestPropertyValue> questValues;
                for (auto property : tp->GetProperties()) {
                    if (!property.IsMandatory()) {
                        continue;
                    }
                    auto candidates = tp->GetPropertyCandidates(property, *worldModel);
                    for (auto candidate : candidates) {
                        questValues.push_back(QuestPropertyValue(property, candidate.GetEntity()));
                    }
                }

                INFO("Template Key: " + templateKey + ", Seed: " + to_string(i));
                auto originalQuest = tp->ToQuest(questValues);

                stringstream ss;
                {
                    cereal::JSONOutputArchive outputArchive(ss);
                    outputArchive(originalQuest);
                }
                string serialized = ss.str();
                REQUIRE(!serialized.empty());

                shared_ptr<Quest> deserializedQuest;
                {
                    cereal::JSONInputArchive inputArchive(ss);
                    inputArchive(deserializedQuest);
                }

                REQUIRE(deserializedQuest.get() != nullptr);
                REQUIRE(deserializedQuest->GetId() == originalQuest->GetId());
                REQUIRE(deserializedQuest->GetDescription() == originalQuest->GetDescription());
                REQUIRE(deserializedQuest->GetState() == originalQuest->GetState());
                REQUIRE(deserializedQuest->GetTitle() == originalQuest->GetTitle());
            }
        }
    }

    SECTION("Serialize and deserialize binary file") {
        WorldModel *worldModel = new SpaceWorldModel(rs);
        const char *fileName = "testSerialization.bin";
        for (int i = 0; i < testSize; i++) {
            rs->Seed(i);
            for (string templateKey : factory.GetTemplateKeys()) {
                auto tp = factory.CreateTemplate(templateKey);
                vector<QuestPropertyValue> questValues;
                for (auto property : tp->GetProperties()) {
                    if (!property.IsMandatory()) {
                        continue;
                    }
                    auto candidates = tp->GetPropertyCandidates(property, *worldModel);
                    for (auto candidate : candidates) {
                        questValues.push_back(QuestPropertyValue(property, candidate.GetEntity()));
                    }
                }

                INFO("Template Key: " + templateKey + ", Seed: " + to_string(i));
                auto originalQuest = tp->ToQuest(questValues);

                {
                    ofstream os(fileName);
                    cereal::PortableBinaryOutputArchive outputArchive(os);
                    outputArchive(originalQuest);
                }

                shared_ptr<Quest> deserializedQuest;
                {
                    ifstream is(fileName);
                    cereal::PortableBinaryInputArchive inputArchive(is);
                    inputArchive(deserializedQuest);
                }

                REQUIRE(deserializedQuest.get() != nullptr);
                REQUIRE(deserializedQuest->GetId() == originalQuest->GetId());
                REQUIRE(deserializedQuest->GetDescription() == originalQuest->GetDescription());
                REQUIRE(deserializedQuest->GetState() == originalQuest->GetState());
                REQUIRE(deserializedQuest->GetTitle() == originalQuest->GetTitle());
            }
        }
        remove(fileName);
    }

    SECTION("Serialize and deserialize quest model") {
        QuestModel questModel;
        WorldModel *worldModel = new SpaceWorldModel(rs);

        for (int i = 0; i < testSize; i++) {
            rs->Seed(i);
            for (string templateKey : factory.GetTemplateKeys()) {
                auto tp = factory.CreateTemplate(templateKey);
                vector<QuestPropertyValue> questValues;
                for (auto property : tp->GetProperties()) {
                    if (!property.IsMandatory()) {
                        continue;
                    }
                    auto candidates = tp->GetPropertyCandidates(property, *worldModel);
                    for (auto candidate : candidates) {
                        questValues.push_back(QuestPropertyValue(property, candidate.GetEntity()));
                    }
                }

                INFO("Template Key: " + templateKey + ", Seed: " + to_string(i));
                try {
                    auto quest = tp->ToQuest(questValues);
                    questModel.RegisterQuest(*quest);
                } catch (ContractFailedException ex) {
                    FAIL(ex.what());
                }
            }
        }

        stringstream ss;
        {
            cereal::JSONOutputArchive outputArchive(ss);
            outputArchive(questModel);
        }
        string serialized = ss.str();
        REQUIRE(!serialized.empty());

        QuestModel deserializedModel;
        {
            cereal::JSONInputArchive inputArchive(ss);
            inputArchive(deserializedModel);
        }

        REQUIRE(deserializedModel.GetQuests().size() == questModel.GetQuests().size());

        for (int i = 0; i < testSize; i++) {
            REQUIRE(deserializedModel.GetQuests()[i]->GetId() == questModel.GetQuests()[i]->GetId());
        }
    }
}

TEST_CASE("Serialize Entities", "[serialize]") {
    int testSize = 100;
    shared_ptr<RandomStream> rs = make_shared<RandomStream>(42);
    shared_ptr<RandomStream> rx = make_shared<RandomStream>(7);
    SpaceWorldModel testModel(rs);

    SECTION("Serialize and deserialize empty model") {
        stringstream ss;
        {
            cereal::JSONOutputArchive outputArchive(ss);
            outputArchive(testModel);
        }
        string serialized = ss.str();
        REQUIRE(!serialized.empty());

        SpaceWorldModel deserializedModel(rx);
        {
            cereal::JSONInputArchive inputArchive(ss);
            inputArchive(deserializedModel);
        }

        REQUIRE(testModel.GetEntities().size() == deserializedModel.GetEntities().size());
    }

    SECTION("Serialize and deserialize a list of entities") {
        vector<shared_ptr<WorldEntity>> entities;

        entities.push_back(testModel.CreateAgent());
        entities.push_back(testModel.CreateLocation());
        entities.push_back(testModel.CreateSolarSystem());

        stringstream ss;
        {
            cereal::JSONOutputArchive outputArchive(ss);
            outputArchive(entities);
        }
        string serialized = ss.str();
        REQUIRE(!serialized.empty());

        vector<shared_ptr<WorldEntity>> deserialized;
        {
            cereal::JSONInputArchive inputArchive(ss);
            inputArchive(deserialized);
        }
        REQUIRE(entities.size() == deserialized.size());

        for (int i = 0; i < entities.size(); i++) {
            REQUIRE(typeid(*(deserialized[i])) == typeid(*(deserialized[i])));
        }
    }

    SECTION("Serialize and deserialize a full world model") {
        vector<ModelAction> actions;

        actions.push_back(ModelAction(ActionType::CREATE, testModel.CreateAgent()));
        actions.push_back(ModelAction(ActionType::CREATE, testModel.CreateLocation()));
        actions.push_back(ModelAction(ActionType::CREATE, testModel.CreateSolarSystem()));

        testModel.Execute(actions);
        REQUIRE(testModel.GetEntities().size() == actions.size());

        testModel.GetMetaData(1).SetValue("Size", 7);
        testModel.GetMetaData(1).SetValue("Age", 42);
        testModel.GetMetaData(2).SetValue("Age", 43);

        stringstream ss;
        {
            cereal::JSONOutputArchive outputArchive(ss);
            outputArchive(testModel);
        }
        string serialized = ss.str();
        REQUIRE(!serialized.empty());

        SpaceWorldModel deserializedModel(rx);
        {
            cereal::JSONInputArchive inputArchive(ss);
            inputArchive(deserializedModel);
        }

        REQUIRE(testModel.GetEntities().size() == deserializedModel.GetEntities().size());

        for (int i = 0; i < testModel.GetEntities().size(); i++) {
            REQUIRE(typeid(*(testModel.GetEntities()[i])) == typeid(*(deserializedModel.GetEntities()[i])));
        }
        REQUIRE(deserializedModel.GetMetaData(1).GetValue("Size") == 7);
        REQUIRE(deserializedModel.GetMetaData(1).GetValue("Age") == 42);
        REQUIRE(deserializedModel.GetMetaData(2).GetValue("Age") == 43);
    }
}