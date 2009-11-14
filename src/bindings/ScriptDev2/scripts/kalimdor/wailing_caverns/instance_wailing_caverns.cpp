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
SDName: Instance_Wailing_Caverns
SD%Complete: 0
SDComment: Placeholder
SDCategory: Wailing Caverns
EndScriptData */

#include "precompiled.h"

#include "escort_ai.h"

#define DATA_COBRAHN  1
#define DATA_ANACONDRA   2
#define DATA_PYTHAS   3
#define DATA_SERPENTIS   4
#define ENCOUNTERS     4
#define SPELL_MW     5232
#define SPELL_SMD     24624

 struct MANGOS_DLL_DECL instance_wailing_caverns : public ScriptedInstance
 {
     instance_wailing_caverns(Map *map) : ScriptedInstance(map) {Initialize();};
 
    uint16 BossKilled;
    uint32 Encounters[ENCOUNTERS];

    void OnCreatureCreate(Creature *creature, uint32 creature_entry)
    {
        switch(creature->GetEntry())
        {
        case 3669://COBRAHN
        case 3671://ANACONDRA
        case 3670://PYTHAS
        case 3673://SERPENTIS
        default: break;
        }
    }

    void Initialize()
    {
         for(uint8 i = 0; i < ENCOUNTERS; i++)
             Encounters[i] = NOT_STARTED;
    }

    void SetData(uint32 type, uint32 data)
    {
         switch(type)
         {
        case DATA_COBRAHN:
            Encounters[0] = data;
            break;
        case DATA_ANACONDRA:
            Encounters[1] = data;
            break;
        case DATA_PYTHAS:
            Encounters[2] = data;
            break;
        case DATA_SERPENTIS:
            Encounters[3] = data;
            break;
       }
}

     uint32 GetData(uint32 type)
     {
         switch(type)
         {
        case DATA_COBRAHN: return Encounters[0];
        case DATA_ANACONDRA:  return Encounters[1];
        case DATA_PYTHAS:  return Encounters[2];
        case DATA_SERPENTIS:  return Encounters[3];
        default:                 return 0;
        }
    }
};
/*######
## npc_disciple_of_naralex
######*/

struct MANGOS_DLL_DECL npc_disciple_of_naralexAI : public npc_escortAI
{
    npc_disciple_of_naralexAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
    }

    ScriptedInstance *pInstance;
    void StartWP()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        SetEscortPaused(false);
    }

    void JustSummoned(Creature* summoned)
    {
        switch(summoned->GetEntry())
        {
            case 3669:
                break;
            default:
                summoned->AI()->AttackStart(m_creature);
                break;
        }
    }
    void WaypointReached(uint32 i)
    {
        switch(i)
        {
            case 1://
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                break;
            case 7://two elite Deviate Ravagers: npc=3636
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                m_creature->SummonCreature(3636,-44.389, 271.394, -92.7563, 4.87418,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(3636,-44.389, 271.394, -92.7563, 4.87418,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                break;
            case 10://three elite among Deviate Viper: npc=5755 and Deviate Adder: npc=5048.
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                m_creature->SummonCreature(5755, 2.81626, 302.536, -87.2281, 2.5329,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5755, 2.81626, 302.536, -87.2281, 2.5329,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5755, 2.81626, 302.536, -87.2281, 2.5329,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5048, 2.81626, 302.536, -87.2281, 2.5329,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                break;
            case 12://three elite Deviate Moccasin npc=5762
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                m_creature->SummonCreature(5762,61.8763, 204.702, -91.626, 2.41195,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5762,61.8763, 204.702, -91.626, 2.41195,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5762,61.8763, 204.702, -91.626, 2.41195,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                break;
            case 13://seven normal Nightmare Ectoplasm:npc=5763. 
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->SummonCreature(5763, 92.1718, 227.981, -94.7002, 4.19245,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                break;
            case 14:
                m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
                //Summon (Mutanus the Devourer)spell=24624
                DoCast(m_creature,SPELL_SMD);
                break;
        }
    }
    void Reset() {}
    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_disciple_of_naralex(Creature* pCreature)
{
    return new npc_disciple_of_naralexAI(pCreature);
}

bool GossipHello_npc_disciple_of_naralex(Player *player, Creature *_Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if (pInstance && pInstance->GetData(DATA_COBRAHN) == DONE && pInstance->GetData(DATA_ANACONDRA)== DONE && pInstance->GetData(DATA_PYTHAS)== DONE && pInstance->GetData(DATA_SERPENTIS)== DONE)
    {
        player->ADD_GOSSIP_ITEM( 0, "Wake up Naralex and end his nightmare!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(_Creature->GetNpcTextId(), _Creature->GetGUID());
        ((npc_disciple_of_naralexAI*)_Creature->AI())->DoCast(player,SPELL_MW);
        return true;
    }
//Mark of the Wild	Rank 2, spell=5232
    ((npc_disciple_of_naralexAI*)_Creature->AI())->DoCast(player,SPELL_MW);
    return true;
}

bool GossipSelect_npc_disciple_of_naralex(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
     player->CLOSE_GOSSIP_MENU(); 
     //((npc_escortAI*)(_Creature->AI()))->Start(true, true, true, player->GetGUID());       
     ((npc_disciple_of_naralexAI*)_Creature->AI())->StartWP();
    }
    return true;
};

InstanceData* GetInstanceData_instance_wailing_caverns(Map* map)
{
    return new instance_wailing_caverns(map);
}

void AddSC_instance_wailing_caverns()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "instance_wailing_caverns";
    newscript->GetInstanceData = &GetInstanceData_instance_wailing_caverns;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_disciple_of_naralex";
    newscript->pGossipHello =   &GossipHello_npc_disciple_of_naralex;
    newscript->pGossipSelect =  &GossipSelect_npc_disciple_of_naralex;
    newscript->GetAI = &GetAI_npc_disciple_of_naralex;
    newscript->RegisterSelf();
}