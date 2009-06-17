/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Instance_Zulaman
SD%Complete: 25
SDComment:
SDCategory: Zul'Aman
EndScriptData */

#include "precompiled.h"
#include "def_zulaman.h"

struct SHostageInfo
{
    uint32 npc, go;
    float x, y, z, o;
};

static SHostageInfo HostageInfo[] =
{
    {23790, 186648, -57, 1343, 40.77, 3.2}, // bear
    {23999, 187021, 400, 1414, 74.36, 3.3}, // eagle
    {24001, 186672, -35, 1134, 18.71, 1.9}, // dragonhawk
    {24024, 186667, 413, 1117,  6.32, 3.1}  // lynx
    
};

struct MANGOS_DLL_DECL instance_zulaman : public ScriptedInstance
{
    instance_zulaman(Map* pMap) : ScriptedInstance(pMap) {Initialize();}

    std::string strInstData;
    uint32 m_uiEventTimer;
    uint32 m_uiEventMinuteStep;

    uint32 m_uiGongCount;

    uint64 m_uiAkilzonGUID;
    uint64 m_uiNalorakkGUID;
    uint64 m_uiJanalaiGUID;
    uint64 m_uiHalazziGUID;
    uint64 m_uiZuljinGUID;
    uint64 m_uiMalacrassGUID;
    uint64 m_uiHarrisonGUID;

    uint64 m_uiStrangeGongGUID;
    uint64 m_uiMassiveGateGUID;
    uint64 m_uiMalacrassEntranceGUID;

    uint32 m_uiJanalaiEggCntL;
    uint32 m_uiJanalaiEggCntR;

    uint32 m_uiEncounter[ENCOUNTERS];
    uint32 m_uiRandVendor[RAND_VENDOR];

    uint64 HexLordGateGUID;
    uint64 ZulJinGateGUID;
    uint64 AkilzonDoorGUID;
    uint64 ZulJinDoorGUID;
    uint64 HalazziDoorGUID;
 
    uint32 QuestTimer;
    uint16 BossKilled;
    uint16 QuestMinute;
    uint16 ChestLooted;

    void Initialize()
    {
        uint64 HexLordGateGUID = 0;
        uint64 ZulJinGateGUID = 0;
        uint64 AkilzonDoorGUID = 0;
        uint64 HalazziDoorGUID = 0;
        uint64 ZulJinDoorGUID = 0;
 
        QuestTimer = 0;
        QuestMinute = 21;
        BossKilled = 0;
        ChestLooted = 0;

        m_uiEventTimer = MINUTE*IN_MILISECONDS;
        m_uiEventMinuteStep = MINUTE/3;

        m_uiGongCount = 0;

        m_uiAkilzonGUID = 0;
        m_uiNalorakkGUID = 0;
        m_uiJanalaiGUID = 0;
        m_uiHalazziGUID = 0;
        m_uiZuljinGUID = 0;
        m_uiMalacrassGUID = 0;
        m_uiHarrisonGUID = 0;

        m_uiStrangeGongGUID = 0;
        m_uiMassiveGateGUID = 0;
        m_uiMalacrassEntranceGUID = 0;

        m_uiJanalaiEggCntL = 20;
        m_uiJanalaiEggCntR = 20;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            m_uiEncounter[i] = NOT_STARTED;

        for(uint8 i = 0; i < RAND_VENDOR; i++)
            m_uiRandVendor[i] = NOT_STARTED;
    }

    void UpdateInstanceWorldState(uint32 uiId, uint32 uiState)
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* pPlayer = itr->getSource())
                {
                    pPlayer->SendUpdateWorldState(uiId, uiState);
                    debug_log("SD2: Instance Zulaman: UpdateInstanceWorldState for id %u with state %u",uiId,uiState);
                }
            }
        }
        else
            debug_log("SD2: Instance Zulaman: UpdateInstanceWorldState, but PlayerList is empty.");
    }
    void UpdateWorldState(uint32 field, uint32 value)
    {
        WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);
        data << field << value;
        ((InstanceMap*)instance)->SendToPlayers(&data);
    }
    void SummonHostage(uint8 num)
    {
        if(!QuestMinute)
            return;

        Map::PlayerList const &PlayerList = instance->GetPlayers();
        if (PlayerList.isEmpty())
            return;

        Map::PlayerList::const_iterator i = PlayerList.begin();
        if(Player* i_pl = i->getSource())
         {
            if(Unit* Hostage = i_pl->SummonCreature(HostageInfo[num].npc, HostageInfo[num].x, HostageInfo[num].y, HostageInfo[num].z, HostageInfo[num].o, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                Hostage->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                Hostage->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        } 
     }
    void OnCreatureCreate(Creature* pCreature, uint32 uiCreatureEntry)
    {
        switch(pCreature->GetEntry())
        {
            case 23574: m_uiAkilzonGUID     = pCreature->GetGUID(); break;
            case 23576: m_uiNalorakkGUID    = pCreature->GetGUID(); break;
            case 23578: m_uiJanalaiGUID     = pCreature->GetGUID(); break;
            case 23577: m_uiHalazziGUID     = pCreature->GetGUID(); break;
            case 23863: m_uiZuljinGUID      = pCreature->GetGUID(); break;
            case 24239: m_uiMalacrassGUID   = pCreature->GetGUID(); break;
            case 24358: m_uiHarrisonGUID    = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case 187359:
                m_uiStrangeGongGUID = pGo->GetGUID();
                break;
            case 186728:
                m_uiMassiveGateGUID = pGo->GetGUID();
                m_uiStrangeGongGUID = pGo->GetGUID();
                break;
            case 186303:
                HalazziDoorGUID = pGo->GetGUID();
                break;
            case 186304:
                ZulJinGateGUID = pGo->GetGUID();
                break;
            case 186305:
                m_uiMalacrassEntranceGUID = pGo->GetGUID();
                HexLordGateGUID = pGo->GetGUID();
                break;
            case 186858:
                AkilzonDoorGUID = pGo->GetGUID();
                break;
            case 186859:
                ZulJinDoorGUID = pGo->GetGUID();
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Zulaman: SetData received for type %u with data %u",uiType,uiData);

        switch(uiType)
        {
            case TYPE_EVENT_RUN:
                if (uiData == SPECIAL)
                {
                    ++m_uiGongCount;
                    if (m_uiGongCount == 5)
                        m_uiEncounter[0] = uiData;
                }
                if (uiData == IN_PROGRESS)
                {
                    DoUseDoorOrButton(m_uiMassiveGateGUID);
                    UpdateInstanceWorldState(WORLD_STATE_COUNTER,m_uiEventMinuteStep);
                    UpdateInstanceWorldState(WORLD_STATE_ID,1);
                    m_uiEncounter[0] = uiData;
                }
                break;
            case TYPE_AKILZON:
                if (uiData == IN_PROGRESS)
                {
                DoUseDoorOrButton(AkilzonDoorGUID);
                }
                if (uiData == DONE)
                {
                DoUseDoorOrButton(AkilzonDoorGUID);
                if(QuestMinute)
                {
                    QuestMinute += 10;
                    UpdateWorldState(3106, QuestMinute);
                }
                SummonHostage(1);
                    if (m_uiEncounter[0] == IN_PROGRESS)
                    {
                        m_uiEventMinuteStep += MINUTE/6;    //add 10 minutes
                        UpdateInstanceWorldState(WORLD_STATE_COUNTER,m_uiEventMinuteStep);
                    }
                }
                m_uiEncounter[1] = uiData;
                break;
            case TYPE_NALORAKK:
                if (uiData == DONE)
                {
                if(QuestMinute)
                 {
                    QuestMinute += 15;
                    UpdateWorldState(3106, QuestMinute);
                 }
                SummonHostage(0);
                    if (m_uiEncounter[0] == IN_PROGRESS)
                    {
                        m_uiEventMinuteStep += MINUTE/4;    //add 15 minutes
                        UpdateInstanceWorldState(WORLD_STATE_COUNTER,m_uiEventMinuteStep);
                    }
                }
                m_uiEncounter[2] = uiData;
                break;
            case TYPE_JANALAI:
                if (uiData == NOT_STARTED)
                {
                    m_uiJanalaiEggCntL = 20;
                    m_uiJanalaiEggCntR = 20;
                }
                m_uiEncounter[3] = uiData;
                if (uiData == DONE)
                {
                SummonHostage(2);
                }
                break;
            case TYPE_HALAZZI:
                m_uiEncounter[4] = uiData;
                if (uiData == IN_PROGRESS)
                {
                DoUseDoorOrButton(HalazziDoorGUID);
                }
                if (uiData == DONE)
                {
                DoUseDoorOrButton(HalazziDoorGUID);
                SummonHostage(3);
                }
                break;
            case TYPE_ZULJIN:
                m_uiEncounter[5] = uiData;
                if (uiData == IN_PROGRESS)
                {
                DoUseDoorOrButton(ZulJinDoorGUID);
                }
                if (uiData == DONE)
                {
                DoUseDoorOrButton(ZulJinDoorGUID);
                }
                break;
            case TYPE_MALACRASS:
                m_uiEncounter[6] = uiData;
                if (uiData == IN_PROGRESS)
                {
                DoUseDoorOrButton(HexLordGateGUID);
                }
                if (uiData == DONE)
                {
                DoUseDoorOrButton(HexLordGateGUID);
                DoUseDoorOrButton(ZulJinDoorGUID);
                }
                break;
            case DATA_J_HATCHLEFT:
                m_uiJanalaiEggCntL -= uiData;
                break;
            case DATA_J_HATCHRIGHT:
                m_uiJanalaiEggCntR -= uiData;
                break;
            case DATA_CHESTLOOTED:
                ChestLooted++;
                SaveToDB();
                break;
            case TYPE_RAND_VENDOR_1:
                m_uiRandVendor[0] = uiData;
                break;
            case TYPE_RAND_VENDOR_2:
                m_uiRandVendor[1] = uiData;
                break;
            default:
                error_log("SD2: Instance Zulaman: ERROR SetData = %u for type %u does not exist/not implemented.",uiType,uiData);
                break;
        }

        if (uiData == DONE || (uiType == TYPE_EVENT_RUN && uiData == IN_PROGRESS))
        {
            OUT_SAVE_INST_DATA;
        if (uiData == DONE)
        {
            BossKilled++;
            if(QuestMinute && BossKilled >= 4)
            {
                QuestMinute = 0;
                UpdateWorldState(3104, 0);
            }
                 if(BossKilled = 4)
                DoUseDoorOrButton(HexLordGateGUID);

        }
            std::ostringstream saveStream;
            saveStream << "S " << BossKilled << " " << ChestLooted << " " << QuestMinute << " "  << m_uiEncounter[0] << " " << m_uiEncounter[1] << " " << m_uiEncounter[2] << " "
                << m_uiEncounter[3] << " " << m_uiEncounter[4] << " " << m_uiEncounter[5] << " " << m_uiEncounter[6];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);
        char dataHead; // S
        uint16 data1, data2, data3;
        std::istringstream loadStream(chrIn);
        loadStream >> dataHead >> data1 >> data2 >> data3 >> m_uiEncounter[0] >> m_uiEncounter[1] >> m_uiEncounter[2] >> m_uiEncounter[3]
            >> m_uiEncounter[4] >> m_uiEncounter[5] >> m_uiEncounter[6] >> ChestLooted;
        if(dataHead == 'S')
        {
            BossKilled = data1;
            ChestLooted = data2;
            QuestMinute = data3;
        }else error_log("SD2: Zul'aman: corrupted save data.");
        //not changing m_uiEncounter[0], TYPE_EVENT_RUN must not reset to NOT_STARTED
        for(uint8 i = 1; i < ENCOUNTERS; ++i)
        {
            if (m_uiEncounter[i] == IN_PROGRESS)
                m_uiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_EVENT_RUN:
                return m_uiEncounter[0];
            case TYPE_AKILZON:
                return m_uiEncounter[1];
            case TYPE_NALORAKK:
                return m_uiEncounter[2];
            case TYPE_JANALAI:
                return m_uiEncounter[3];
            case TYPE_HALAZZI:
                return m_uiEncounter[4];
            case TYPE_ZULJIN:
                return m_uiEncounter[5];
            case TYPE_MALACRASS:
                return m_uiEncounter[6];
            case DATA_CHESTLOOTED:   return ChestLooted;
            case DATA_J_EGGSLEFT:
                return m_uiJanalaiEggCntL;
            case DATA_J_EGGSRIGHT:
                return m_uiJanalaiEggCntR;

            case TYPE_RAND_VENDOR_1:
                return m_uiRandVendor[0];
            case TYPE_RAND_VENDOR_2:
                return m_uiRandVendor[1];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_AKILZON:
                return m_uiAkilzonGUID;
            case DATA_NALORAKK:
                return m_uiNalorakkGUID;
            case DATA_JANALAI:
                return m_uiJanalaiGUID;
            case DATA_HALAZZI:
                return m_uiHalazziGUID;
            case DATA_ZULJIN:
                return m_uiZuljinGUID;
            case DATA_MALACRASS:
                return m_uiMalacrassGUID;
            case DATA_HARRISON:
                return m_uiHarrisonGUID;
            case DATA_GO_GONG:
                return m_uiStrangeGongGUID;
            case DATA_GO_ENTRANCE:
                return m_uiMassiveGateGUID;
            case DATA_GO_MALACRASS_GATE:
                return m_uiMalacrassEntranceGUID;
        }
        return 0;
    }

    void Update(uint32 uiDiff)
    {
        if(QuestMinute)
        {
            if(QuestTimer < uiDiff)
            {
                QuestMinute--;
                SaveToDB();
                QuestTimer += 60000;
                if(QuestMinute)
                {
                    UpdateWorldState(3104, 1);
                    UpdateWorldState(3106, QuestMinute);
                }else UpdateWorldState(3104, 0);
            }
            QuestTimer -= uiDiff;
         }

        if (GetData(TYPE_EVENT_RUN) == IN_PROGRESS)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                if (m_uiEventMinuteStep == 0)
                {
                    debug_log("SD2: Instance Zulaman: event time reach end, event failed.");
                    m_uiEncounter[0] = FAIL;
                    return;
                }

                --m_uiEventMinuteStep;
                UpdateInstanceWorldState(WORLD_STATE_COUNTER, m_uiEventMinuteStep);
                debug_log("SD2: Instance Zulaman: minute decrease to %u.",m_uiEventMinuteStep);

                m_uiEventTimer = MINUTE*IN_MILISECONDS;
            }
            else
                m_uiEventTimer -= uiDiff;
        }
    }
};

InstanceData* GetInstanceData_instance_zulaman(Map* pMap)
{
    return new instance_zulaman(pMap);
}

void AddSC_instance_zulaman()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_zulaman";
    pNewScript->GetInstanceData = &GetInstanceData_instance_zulaman;
    pNewScript->RegisterSelf();
}
