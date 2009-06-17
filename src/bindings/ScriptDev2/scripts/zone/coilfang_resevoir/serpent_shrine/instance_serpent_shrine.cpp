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
SDName: Instance_Serpent_Shrine
SD%Complete: 100%
SDComment:
SDCategory: Coilfang Resevoir, Serpent Shrine Cavern
EndScriptData */

#include "precompiled.h"
#include "def_serpent_shrine.h"

/* Serpentshrine cavern encounters:
0 - Hydross The Unstable event
1 - Leotheras The Blind Event
2 - The Lurker Below Event
3 - Fathom-Lord Karathress Event
4 - Morogrim Tidewalker Event
5 - Lady Vashj Event
*/
const int ENCOUNTERS = 6;

struct MANGOS_DLL_DECL instance_serpentshrine_cavern : public ScriptedInstance
{
    instance_serpentshrine_cavern(Map* pMap) : ScriptedInstance(pMap) { Initialize(); };

    uint64 m_uiSharkkis;
    uint64 m_uiTidalvess;
    uint64 m_uiCaribdis;
    uint64 m_uiLadyVashj;
    uint64 m_uiKarathress;
    uint64 m_uiKarathressEvent_Starter;

    bool m_bShieldGeneratorDeactivated[4];

    bool m_bEncounters[ENCOUNTERS];

    void Initialize()
    {
        m_uiSharkkis = 0;
        m_uiTidalvess = 0;
        m_uiCaribdis = 0;
        m_uiLadyVashj = 0;
        m_uiKarathress = 0;
        m_uiKarathressEvent_Starter = 0;

        memset(&m_bShieldGeneratorDeactivated, 0, sizeof(m_bShieldGeneratorDeactivated));

        memset(&m_bEncounters, 0, sizeof(m_bEncounters));
    }

    bool IsEncounterInProgress()
    {
        for(uint8 i = 0; i < ENCOUNTERS; ++i)
            if(m_bEncounters[i]) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, uint32 uiCreatureEntry)
    {
        switch(pCreature->GetEntry())
        {
            case 21212: m_uiLadyVashj  = pCreature->GetGUID(); break;
            case 21214: m_uiKarathress = pCreature->GetGUID(); break;
            case 21966: m_uiSharkkis   = pCreature->GetGUID(); break;
            case 21965: m_uiTidalvess  = pCreature->GetGUID(); break;
            case 21964: m_uiCaribdis   = pCreature->GetGUID(); break;
        }
    }

    void SetData64(uint32 uiType, uint64 uiData)
    {
        if(uiType == DATA_KARATHRESSEVENT_STARTER)
            m_uiKarathressEvent_Starter = uiData;
    }

    uint64 GetData64(uint32 uiIdentifier)
    {
        switch(uiIdentifier)
        {
            case DATA_SPELLBINDER_1:
                return 93770;
            case DATA_SPELLBINDER_2:
                return 93771;
            case DATA_SPELLBINDER_3:
                return 93772;
            case DATA_SHARKKIS:
                return m_uiSharkkis;
            case DATA_TIDALVESS:
                return m_uiTidalvess;
            case DATA_CARIBDIS:
                return m_uiCaribdis;
            case DATA_LADYVASHJ:
                return m_uiLadyVashj;
            case DATA_KARATHRESS:
                return m_uiKarathress;
            case DATA_KARATHRESSEVENT_STARTER:
                return m_uiKarathressEvent_Starter;
        }
        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case DATA_HYDROSSTHEUNSTABLEEVENT:
                m_bEncounters[0] = uiData;
                break;

            case DATA_LEOTHERASTHEBLINDEVENT:
                m_bEncounters[1] = uiData;
                break;

            case DATA_THELURKERBELOWEVENT:
                m_bEncounters[2] = uiData;
                break;

            case DATA_KARATHRESSEVENT:
                m_bEncounters[3] = uiData;
                break;

            case DATA_MOROGRIMTIDEWALKEREVENT:
                m_bEncounters[4] = uiData;
                break;
                //Lady Vashj
            case DATA_LADYVASHJEVENT:
                if(uiData == 0)
                    memset(&m_bShieldGeneratorDeactivated, 0, sizeof(m_bShieldGeneratorDeactivated));
                m_bEncounters[5] = uiData;
                break;

            case DATA_SHIELDGENERATOR1:
                m_bShieldGeneratorDeactivated[0] = uiData;
                break;

            case DATA_SHIELDGENERATOR2:
                m_bShieldGeneratorDeactivated[1] = uiData;
                break;

            case DATA_SHIELDGENERATOR3:
                m_bShieldGeneratorDeactivated[2] = uiData;
                break;

            case DATA_SHIELDGENERATOR4:
                m_bShieldGeneratorDeactivated[3] = uiData;
                break;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case DATA_HYDROSSTHEUNSTABLEEVENT:
                return m_bEncounters[0];

            case DATA_LEOTHERASTHEBLINDEVENT:
                return m_bEncounters[1];

            case DATA_THELURKERBELOWEVENT:
                return m_bEncounters[2];

            case DATA_KARATHRESSEVENT:
                return m_bEncounters[3];

            case DATA_MOROGRIMTIDEWALKEREVENT:
                return m_bEncounters[4];

                //Lady Vashj
            case DATA_LADYVASHJEVENT:
                return m_bEncounters[5];

            case DATA_SHIELDGENERATOR1:
                return m_bShieldGeneratorDeactivated[0];

            case DATA_SHIELDGENERATOR2:
                return m_bShieldGeneratorDeactivated[1];

            case DATA_SHIELDGENERATOR3:
                return m_bShieldGeneratorDeactivated[2];

            case DATA_SHIELDGENERATOR4:
                return m_bShieldGeneratorDeactivated[3];

            case DATA_CANSTARTPHASE3:
                if(m_bShieldGeneratorDeactivated[0] && m_bShieldGeneratorDeactivated[1] && m_bShieldGeneratorDeactivated[2] && m_bShieldGeneratorDeactivated[3])
                    return 1;
                break;
        }

        return 0;
    }
};

InstanceData* GetInstanceData_instance_serpentshrine_cavern(Map* pMap)
{
    return new instance_serpentshrine_cavern(pMap);
}

void AddSC_instance_serpentshrine_cavern()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_serpent_shrine";
    newscript->GetInstanceData = &GetInstanceData_instance_serpentshrine_cavern;
    newscript->RegisterSelf();
}
