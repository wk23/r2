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
SDName: Boss_Shade_of_Akama
SD%Complete: 90
SDComment: Seems to be complete.
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

#define SAY_DEATH                   -1564013
#define SAY_LOW_HEALTH              -1564014
// Ending cinematic text
#define SAY_FREE                    -1564015
#define SAY_BROKEN_FREE_01          -1564016
#define SAY_BROKEN_FREE_02          -1564017

#define GOSSIP_ITEM                 "We are ready to fight alongside you, Akama"

struct Location
{
    float x, y, o, z;
};

static Location SpawnLocations[]=
{
    {498.652740, 461.728119, 0},
    {498.505003, 339.619324, 0}
};

static Location BrokenCoords[]=
{
    {541.375916, 401.439575, M_PI, 112.783997},             // The place where Akama channels
    {534.130005, 352.394531, 2.164150, 112.783737},         // Behind a 'pillar' which is behind the east alcove
    {499.621185, 341.534729, 1.652856, 112.783730},         // East Alcove
    {499.151093, 461.036438, 4.770888, 112.78370},          // West Alcove
};

static Location BrokenWP[]=
{
    {492.491638, 400.744690, 3.122336, 112.783737},
    {494.335724, 382.221771, 2.676230, 112.783737},
    {489.555939, 373.507202, 2.416263, 112.783737},
    {491.136353, 427.868774, 3.519748, 112.783737},
};

// Locations
#define Z1              118.543144
#define Z2              120.783768
#define Z_SPAWN         113.537949
#define AGGRO_X         482.793182
#define AGGRO_Y         401.270172
#define AGGRO_Z         112.783928
#define AKAMA_X         514.583984
#define AKAMA_Y         400.601013
#define AKAMA_Z         112.783997

// Spells
#define SPELL_VERTEX_SHADE_BLACK    39833
#define SPELL_SHADE_SOUL_CHANNEL    40401
#define SPELL_DESTRUCTIVE_POISON    40874
#define SPELL_LIGHTNING_BOLT        42024
#define SPELL_AKAMA_SOUL_CHANNEL    40447
#define SPELL_AKAMA_SOUL_RETRIEVE   40902
#define AKAMA_SOUL_EXPEL            40855
#define SPELL_SHADE_SOUL_CHANNEL_2  40520

// Channeler entry
#define CREATURE_CHANNELER          23421
#define CREATURE_SORCERER           23215
#define CREATURE_DEFENDER           23216
#define CREATURE_BROKEN             23319

const uint32 spawnEntries[3]= { 23523, 23318, 23524 };
struct Locations
{
    float x, y, z;
    uint32 id;
    float o;
};

static Locations AkamaWP[]=
{
    { 540.53, 401.11, 112.78 },
    { 534.25, 400.82, 112.78 },
    { 527.55, 400.91, 112.78 },
    { 522.66, 400.67, 112.78 },
    { 516.18, 400.83, 112.78 },
    { 510.08, 400.93, 112.78 },
    { 504.00, 400.97, 112.78 },
    { 499.75, 400.94, 112.78 },
    { 491.89, 400.90, 112.78 },
    { 489.05, 401.09, 112.78 },
    { 479.69, 401.12, 113.85 },
    { 477.67, 401.08, 114.08 },
    { 475.41, 401.03, 116.03 },
    { 472.70, 400.97, 117.38 },
    { 470.63, 400.93, 118.42 },
    { 465.45, 400.82, 118.53 }
};
// 755.762, 304.0747, 312.1769 -- This is where Akama should be spawned
static Locations SpiritSpawns[]=
{
{443.495, 413.973, 118.621, 5.13127, CREATURE_CHANNELER},
{456.694, 414.117, 118.621, 4.2586, CREATURE_CHANNELER},
{457.763, 390.117, 118.621, 2.25148, CREATURE_CHANNELER},
{444.153, 389.214, 118.621, 1.0821, CREATURE_CHANNELER},
{438.279, 401.258, 118.621, 0.017453, CREATURE_CHANNELER},
{463.186, 401.64, 118.621, 3.19395, CREATURE_CHANNELER}
};

struct WayPoints
{
    WayPoints(uint32 _id, float _x, float _y, float _z)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
    }
    uint32 id;
    float x, y, z;
};


struct MANGOS_DLL_DECL mob_ashtongue_channelerAI : public ScriptedAI
{
    mob_ashtongue_channelerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
      pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
      CheckTimer = 2000;
      ShadeGUID = pInstance ? pInstance->GetData64(DATA_SHADEOFAKAMA) : 0;
      SpellEntry *TempSpell1 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHADE_SOUL_CHANNEL_2);
      if(TempSpell1)
        TempSpell1->EffectImplicitTargetA[0] =18;//TARGET_EFFECT_SELECT
    }

    ScriptedInstance* pInstance;
    uint32 CheckTimer;
    uint64 ShadeGUID;

    void Reset() { }

    void SpellHitTarget(Unit* target,const SpellEntry* spell)
    {
        Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
        if (Shade)
        switch(spell->Id)
        {
        case SPELL_SHADE_SOUL_CHANNEL:
        case SPELL_SHADE_SOUL_CHANNEL_2: 
            if (Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
               Shade->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        default:        
            if (Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
               Shade->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTimer < diff)
        {
            Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
            if (Shade && Shade->isAlive() && m_creature->isAlive())
            {
                DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);
            }
            CheckTimer = 2000;
        }else CheckTimer -= diff;
    }

    void JustDied(Unit* killer) 
    {
       uint32 Ch;
       Ch = pInstance->GetData(DATA_AKAMACHAN);
       Ch--;
       pInstance->SetData(DATA_AKAMACHAN,Ch);
//error_log("ostalos sors-chans:  %u  ",Ch);
       m_creature->RemoveCorpse();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (pInstance)
            //if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT)==IN_PROGRESS)
        return;
    }

    void AttackStart(Unit* who)
    {
        if (pInstance)
            //if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT)==IN_PROGRESS)
        return;
    }

    void Aggro(Unit* who)  {}
};

struct MANGOS_DLL_DECL mob_ashtongue_sorcererAI : public ScriptedAI
{
    mob_ashtongue_sorcererAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
      pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
      CheckTimer = 2000;
      ShadeGUID = pInstance ? pInstance->GetData64(DATA_SHADEOFAKAMA) : 0;
      SpellEntry *TempSpell1 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHADE_SOUL_CHANNEL_2);
      if(TempSpell1)
        TempSpell1->EffectImplicitTargetA[0] =18;//TARGET_EFFECT_SELECT
    }

    ScriptedInstance* pInstance;
    uint32 CheckTimer;
    uint64 ShadeGUID;

    void Reset() { }

    void SpellHitTarget(Unit* target,const SpellEntry* spell)
    {
        Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
        if (Shade)
        switch(spell->Id)
        {
        case SPELL_SHADE_SOUL_CHANNEL:
        case SPELL_SHADE_SOUL_CHANNEL_2: 
            if (Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
               Shade->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        default:        
            if (Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
               Shade->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTimer < diff)
        {
            Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
            if (Shade && Shade->isAlive() && m_creature->isAlive())
            {
                m_creature->SetActiveObjectState(true);
                DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                DoCast(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);
            }
            CheckTimer = 2000;
        }else CheckTimer -= diff;
    }

    void JustDied(Unit* killer) 
    {
       uint32 Ch;
       Ch = pInstance->GetData(DATA_AKAMACHAN);
       Ch--;
       pInstance->SetData(DATA_AKAMACHAN,Ch);
//error_log("ostalos sors-chans:  %u ",Ch);
       m_creature->RemoveCorpse();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (pInstance)
            //if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT)==IN_PROGRESS)
        return;
        //ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (pInstance)
            //if (pInstance->GetData(DATA_SHADEOFAKAMAEVENT)==IN_PROGRESS)
        return;
        //ScriptedAI::AttackStart(who);
    }

    void Aggro(Unit* who)
    {
        //m_creature->CombatStop();
     }
};

struct MANGOS_DLL_DECL boss_shade_of_akamaAI : public ScriptedAI
{
    boss_shade_of_akamaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        AkamaGUID = pInstance ? pInstance->GetData64(DATA_AKAMA_SHADE) : 0;
        Reset();
        OpenMotherDoor();
    }

    ScriptedInstance* pInstance;

    uint64 AkamaGUID;

    uint32 ReduceHealthTimer;
    uint32 SummonTimer;
    uint32 ResetTimer;
    uint32 DefenderTimer;                                   // They are on a flat 15 second timer, independant of the other summon creature timer.

    bool IsBanished;
    bool HasKilledAkama;

    void Reset()
    {
        if (Unit* Akama = Unit::GetUnit(*m_creature, AkamaGUID))
           {
               Akama->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
               //Akama-AI()->EnterEvadeMode();
            }

        SummonTimer = 10000;
        ReduceHealthTimer = 0;
        ResetTimer = 60000;
        DefenderTimer = 15000;

        IsBanished = true;
        HasKilledAkama = false;
        m_creature->SetActiveObjectState(true);
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);

        if (pInstance)
        {
            if (m_creature->isAlive())
            {
                pInstance->SetData(DATA_SHADEOFAKAMAEVENT, NOT_STARTED);
            } else OpenMotherDoor();
        }
    }

    void JustSummoned(Creature* summon)
    {
        Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
        if(target && summon)
            summon->Attack(target,false);
    }

    void OpenMotherDoor()
    {
        if (GameObject* pDoor = pInstance->instance->GetGameObject(pInstance->GetData64(DATA_GO_PRE_SHAHRAZ_DOOR)))
            pDoor->SetGoState(GO_STATE_ACTIVE);
    }

    void AttackStart(Unit* who)
    {
        if (!who || IsBanished || who == m_creature)
            return;

        ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (IsBanished)
            return;
        ScriptedAI::MoveInLineOfSight(who);
    }

    void SummonCreature()
    {
        uint32 random = rand()%2;
        float X = SpawnLocations[random].x;
        float Y = SpawnLocations[random].y;

        for(uint8 i = 0; i < 3; ++i)
            {
                Creature* Spawn = m_creature->SummonCreature(spawnEntries[i], X, Y, Z_SPAWN, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 25000);
                if (Spawn)
                {
                    Spawn->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
                    Spawn->SetActiveObjectState(true);
                    //Spawn->GetMotionMaster()->MovePoint(0, AGGRO_X, AGGRO_Y, AGGRO_Z);
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        Spawn->AddThreat(target, 0.0f);
                }
            }
    }

    void SetAkamaGUID(uint64 guid)
    {
        AkamaGUID = guid;
    }

    void JustDied(Unit *killer)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_SHADEOFAKAMAEVENT, DONE);
            OpenMotherDoor();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isInCombat())
            return;
        if (pInstance)
           if (pInstance->GetData(DATA_AKAMACHAN)==0)
              IsBanished=false;
        if (IsBanished)
        {
            // Akama is set in the threatlist so when we reset, we make sure that he is not included in our check
            if (m_creature->getThreatManager().getThreatList().size() < 2)
                ScriptedAI::EnterEvadeMode();

            if (DefenderTimer < diff)
            {
                uint32 ran = rand()%2;
                Creature* Defender = m_creature->SummonCreature(CREATURE_DEFENDER, SpawnLocations[ran].x, SpawnLocations[ran].y, Z_SPAWN, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 25000);
                if (Defender)
                {
                    Defender->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
                    Defender->SetActiveObjectState(true);
                    //Defender->GetMotionMaster()->MovePoint(0, AGGRO_X, AGGRO_Y, AGGRO_Z);
                    if(Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        Defender->AddThreat(target, 0.0f);
                }
                DefenderTimer = 15000;
            }else DefenderTimer -= diff;

            if (SummonTimer < diff)
            {
                SummonCreature();
                SummonTimer = 35000;
            }else SummonTimer -= diff;

        }
        else                                                // No longer banished, let's fight Akama now
        {
            if (!IsBanished)
            {
                if (AkamaGUID)
                {
                    Creature* Akama = (Creature*)Unit::GetUnit((*m_creature), AkamaGUID);
                    if (Akama && Akama->isAlive() && m_creature->isAlive())
                    {
                        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                           m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->AddThreat(Akama, 10000000.0f);
                        Akama->AddThreat(m_creature, 10000000.0f);
                        m_creature->AI()->AttackStart(Akama);
                        //((Creature*)Akama)->AI()->AttackStart(m_creature);
                    }
                }
            }
            if (ReduceHealthTimer < diff)
            {
                if (AkamaGUID)
                {
                    Unit* Akama = Unit::GetUnit((*m_creature), AkamaGUID);
                    if (Akama && Akama->isAlive())
                    {
                        //10 % less health every few seconds.
                        m_creature->DealDamage(Akama, Akama->GetMaxHealth()/10, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        ReduceHealthTimer = 12000;
                    }
                    else
                    {
                        HasKilledAkama = true;              // Akama is dead or missing, we stop fighting and disappear
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        m_creature->SetHealth(m_creature->GetMaxHealth());
                        m_creature->RemoveAllAuras();
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                }
            }else ReduceHealthTimer -= diff;

            if (HasKilledAkama)
            {
                if (ResetTimer < diff)
                {
                    EnterEvadeMode();                       // Reset a little while after killing Akama
                }
                else ResetTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

struct MANGOS_DLL_DECL npc_akamaAI : public ScriptedAI
{
    npc_akamaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        ShadeHasDied = false;
        StartCombat = false;
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        ShadeGUID = pInstance ? pInstance->GetData64(DATA_SHADEOFAKAMA) : 0;
        SpellEntry *TempSpell1 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_DESTRUCTIVE_POISON);
        if(TempSpell1)
            TempSpell1->EffectImplicitTargetA[0] =18;//TARGET_EFFECT_SELECT
        //SpellEntry *TempSpell2 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_LIGHTNING_BOLT);
        //if(TempSpell2)
            //TempSpell2->EffectImplicitTargetA[0] =18;//TARGET_EFFECT_SELECT
        SpellEntry *TempSpell3 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_AKAMA_SOUL_CHANNEL);
        if(TempSpell3)
            TempSpell3->EffectImplicitTargetA[1] =18;//TARGET_EFFECT_SELECT
        SpellEntry *TempSpell4 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_AKAMA_SOUL_RETRIEVE);
        if(TempSpell4)
            {
            TempSpell4->EffectImplicitTargetA[1] =18;//TARGET_EFFECT_SELECT
            TempSpell4->EffectImplicitTargetA[2] =18;//TARGET_EFFECT_SELECT
            }
        SpellEntry *TempSpell5 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_SHADE_SOUL_CHANNEL_2);
        if(TempSpell5)
            TempSpell5->EffectImplicitTargetA[1] =18;//TARGET_EFFECT_SELECT
        WayPointList.clear();
        Reset();
    }

    /* Instance Data */
    ScriptedInstance* pInstance;

    /* Timers */
    uint32 ChannelTimer;
    uint32 TalkTimer;
    uint32 WalkTimer;

    uint32 DestructivePoisonTimer;
    uint32 LightningBoltTimer;
    uint32 CheckTimer;
    uint32 CastSoulRetrieveTimer;
    uint32 SoulRetrieveTimer;
    uint32 SummonBrokenTimer;
    uint32 EndingTalkCount;
    uint32 WayPointId;
    uint32 BrokenSummonIndex;

    /* GUIDs */
    uint64 ShadeGUID;
    uint64 PlayerGUID;
    uint64 SpiritGUID[6];
    uint64 ChannelGUID;
    uint32 Chans;

    bool IsTalking;
    bool StartChanneling;

    bool IsWalking;
    bool Walking;
    uint32 TalkCount;
    uint32 ChannelCount;

    std::list<uint64> BrokenList;

    bool EventBegun;
    bool ShadeHasDied;
    bool StartCombat;
    bool HasYelledOnce;

    std::list<WayPoints> WayPointList;
    std::list<WayPoints>::iterator WayPoint;

    void BeginEvent(uint64 PlayerGUID);

    void Reset()
    {
        ShadeGUID = 0;
        PlayerGUID  = 0;
        ChannelGUID = 0;
        Chans = 0;
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        StartChanneling = false;
        for(uint8 i = 0; i < 6; ++i) SpiritGUID[i] = 0;
        if (pInstance)
        {
            pInstance->SetData(DATA_AKAMACHAN,Chans);
            pInstance->SetData(DATA_SHADEOFAKAMAEVENT, NOT_STARTED);
            ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
        }
        Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
        if (!Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
           Shade->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        for(uint8 i = 0; i < 6; ++i)
            {
                 Creature* Spirit = m_creature->SummonCreature(CREATURE_CHANNELER, SpiritSpawns[i].x, SpiritSpawns[i].y, SpiritSpawns[i].z, SpiritSpawns[i].o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 720000);
                 if (Spirit)
                    {
                        if (!Spirit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                           Spirit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
                        if (Shade && Shade->isAlive() && Spirit->isAlive())
                           {
                               Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                               Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);
                               StartChanneling = true;
                           }
                        SpiritGUID[i] = Spirit->GetGUID();
                        Chans++;
//error_log("first summon Chans, i, guid:  %u, %u, %u",Chans,i,SpiritGUID[i]);
                        pInstance->SetData(DATA_AKAMACHAN, Chans);
//error_log("aftet setdata first summon Chans, i, guid:  %u, %u, %u",pInstance->GetData(DATA_AKAMACHAN),i,SpiritGUID[i]);
                    }
            }
        ChannelTimer = 0;
        ChannelCount = 0;

        WalkTimer = 0;
        IsWalking = false;
        Walking = false;

        TalkTimer = 0;
        TalkCount = 0;

        IsTalking = false;
        EventBegun = false;
        // Database sometimes has strange values..
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetVisibility(VISIBILITY_ON);
    }

    void JustSummoned(Creature* summon)
    {
        Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
        if(target && summon)
            summon->Attack(target,false);
    }

    void BeginEvent(Player* pl)
    {
        if (!pInstance)
            return;

        WayPoint = WayPointList.begin();
        m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);
        m_creature->GetMotionMaster()->MovePoint(WayPoint->id, WayPoint->x, WayPoint->y, WayPoint->z);
        IsWalking = true;
        Walking = true;

        ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
        if (!ShadeGUID)
            return;
        Creature* Shade = ((Creature*)Unit::GetUnit((*m_creature), ShadeGUID));
        if (Shade)
        {
            if(pl)
               {  
                   Shade->AddThreat(pl, 1000000.0f);
                   DoZoneInCombat(Shade);
                }
            // Prevent players from trying to restart event
            pInstance->SetData(DATA_SHADEOFAKAMAEVENT, IN_PROGRESS);
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            for(uint8 i = 0; i < 6; ++i)
               if (SpiritGUID[i])
                  {
                      Unit* Spirit = Unit::GetUnit((*m_creature), SpiritGUID[i]);
//error_log("begun ivent remove flags: %u, %u",SpiritGUID[i],i);
                      if (Spirit && (Spirit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)))
                         {
                              Spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                              //Spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                              Spirit->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                          }
                   }
            EventBegun = true;
        }
    }

    // Do not call reset in Akama's evade mode, as this will stop him from summoning minions after he kills the first bit
    void EnterEvadeMode()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
    }

    void AddWaypoint(uint32 id, float x, float y, float z)
    {
        WayPoints AWP(id, x, y, z);
        WayPointList.push_back(AWP);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 15 && !HasYelledOnce)
        {
            DoScriptText(SAY_LOW_HEALTH, m_creature);
            HasYelledOnce = true;
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || !IsWalking)
            return;

        if (WayPoint->id != id)
            return;

        switch(id)
        {
            case 16:
                IsWalking = false;
                if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                   m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetUInt64Value(UNIT_FIELD_TARGET, ShadeGUID);
                break;
        }

        ++WayPoint;
        WalkTimer = 200;
    }

    void DeleteFromThreatList()
    {
        // If we do not have Shade's GUID, do not proceed
        if (!ShadeGUID)
            return;
        // Create a pointer to Shade
        Creature* Shade = ((Creature*)Unit::GetUnit((*m_creature), ShadeGUID));
        // No use to continue if Shade does not exist
        if (!Shade)
            return;

        std::list<HostilReference*>::iterator itr = Shade->getThreatManager().getThreatList().begin();
        for(; itr != Shade->getThreatManager().getThreatList().end(); ++itr)
        {
            // Loop through threatlist till our GUID is found in it.
            if ((*itr)->getUnitGuid() == m_creature->GetGUID())
            {
                (*itr)->removeReference();                  // Delete ourself from his threatlist.
                return;                                     // No need to continue anymore.
            }
        }
        // Now we delete our threatlist to prevent attacking anyone for now
        m_creature->DeleteThreatList();
    }

    void UpdateAI(const uint32 diff)
    {
        if (ShadeGUID)
        {
            Creature* Shade = ((Creature*)Unit::GetUnit((*m_creature), ShadeGUID));
            if (Shade)
            {
                if (!Shade->IsInEvadeMode() && m_creature->IsInEvadeMode())
                    Shade->AI()->EnterEvadeMode();
            }
        }else
        {
            if (pInstance)
                ShadeGUID = pInstance->GetData64(DATA_SHADEOFAKAMA);
        }

        if (IsWalking && WalkTimer)
        {
            if (WalkTimer <= diff)
            {
                if (WayPoint == WayPointList.end())
                    return;

                m_creature->GetMotionMaster()->MovePoint(WayPoint->id, WayPoint->x, WayPoint->y,WayPoint->z);
                WalkTimer = 0;
            }else WalkTimer -= diff;
        }

        if (EventBegun && StartChanneling)
        {
            Chans = pInstance->GetData(DATA_AKAMACHAN);
            if(Chans == 0)
               {
//error_log("EventBegun && StartChanneling - summonChans: %u, %b",Chans,StartChanneling);
                  StartChanneling = false;
                  if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                     m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                  //m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                  Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
                  if (Shade)
                     {
                         if (Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                             Shade->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                         //Shade->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                         Shade->AddThreat(m_creature, 1000000.0f);
                         m_creature->AddThreat(Shade, 1000000.0f);
//error_log("EventBegun && StartChanneling - ShadeGUID chans: %u ",Chans);
                         Shade->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                         DoZoneInCombat(Shade);
                     }
                  return;
               }
            if (ChannelTimer < diff)
            {
                for(uint8 i = 0; i < 6; ++i)
                    {
                     if (SpiritGUID[i])
                        {
                        Unit* Spirit = Unit::GetUnit((*m_creature), SpiritGUID[i]);
                        Creature* Shade = (Creature*)Unit::GetUnit((*m_creature), ShadeGUID);
                        if (Spirit)
                           {if (Shade && Shade->isAlive() && Spirit->isAlive())
                               {
                                   Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                                   Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);
                                   if (Spirit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                                      Spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                }}
                        else
                           {
                                Creature* Spirit = m_creature->SummonCreature(CREATURE_SORCERER, SpiritSpawns[i].x, SpiritSpawns[i].y, SpiritSpawns[i].z, SpiritSpawns[i].o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 720000);
                                if (Spirit)
                                   {
                                       if (Spirit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                                          Spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                       //Spirit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                       Spirit->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
                                       if (Shade && Shade->isAlive() && Spirit->isAlive())
                                          {
                                              Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL, true);
                                              Spirit->CastSpell(Shade, SPELL_SHADE_SOUL_CHANNEL_2, true);
                                           }
                                       SpiritGUID[i] = Spirit->GetGUID();
                                       Chans++;
//error_log("ChannelTimer < diff guid, i, chans: %u, %u, %u",SpiritGUID[i],i,Chans);
                                       pInstance->SetData(DATA_AKAMACHAN, Chans);
                                   }
                           }
                        }
                    }

                ChannelTimer = 5000;
            }else ChannelTimer -= diff;
        }

        if (SummonBrokenTimer && BrokenSummonIndex < 4)
        {
            if (SummonBrokenTimer <= diff)
            {
                for(uint8 i = 0; i < 4; ++i)
                {
                    float x = BrokenCoords[BrokenSummonIndex].x + (i*5);
                    float y = BrokenCoords[BrokenSummonIndex].y + (1*5);
                    float z = BrokenCoords[BrokenSummonIndex].z;
                    float o = BrokenCoords[BrokenSummonIndex].o;

                    Creature* Broken = m_creature->SummonCreature(CREATURE_BROKEN, x, y, z, o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 360000);
                    if (Broken)
                    {
                        float wx = BrokenWP[BrokenSummonIndex].x + (i*5);
                        float wy = BrokenWP[BrokenSummonIndex].y + (i*5);
                        float wz = BrokenWP[BrokenSummonIndex].z;

                        Broken->GetMotionMaster()->MovePoint(0, wx, wy, wz);
                        Broken->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        Broken->SetActiveObjectState(true);
                        BrokenList.push_back(Broken->GetGUID());
                    }
                }
                ++BrokenSummonIndex;
                SummonBrokenTimer = 10000;
            }else SummonBrokenTimer -= diff;
        }

        if (DestructivePoisonTimer < diff)
        {
            Unit* Shade = Unit::GetUnit((*m_creature), ShadeGUID);
            if (Shade &&!Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)&& Shade->isAlive() && (m_creature->IsWithinDist(Shade, 50.0f, true)))
                DoCast(Shade, SPELL_DESTRUCTIVE_POISON);
            DestructivePoisonTimer = 15000;
        }else DestructivePoisonTimer -= diff;

        if (LightningBoltTimer < diff)
        {
            Unit* Shade = Unit::GetUnit((*m_creature), ShadeGUID);
            if (Shade &&!Shade->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)&& Shade->isAlive() && (m_creature->IsWithinDist(Shade, 50.0f, true)))
                DoCast(Shade, SPELL_LIGHTNING_BOLT);
            LightningBoltTimer = 10000;
        }else LightningBoltTimer -= diff;

        if (SoulRetrieveTimer)
        {
            if (SoulRetrieveTimer <= diff)
            {
                switch(EndingTalkCount)
                {
                    case 0:
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        ++EndingTalkCount;
                        SoulRetrieveTimer = 2000;
                        SummonBrokenTimer = 1;
                        break;
                    case 1:
                        DoScriptText(SAY_FREE, m_creature);
                        ++EndingTalkCount;
                        SoulRetrieveTimer = 25000;
                        break;
                    case 2:
                        if (!BrokenList.empty())
                        {
                            bool Yelled = false;
                            for(std::list<uint64>::iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                if (Unit* pUnit = Unit::GetUnit(*m_creature, *itr))
                                {
                                    if (!Yelled)
                                    {
                                        DoScriptText(SAY_BROKEN_FREE_01, pUnit);
                                        Yelled = true;
                                    }
                                    pUnit->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                }
                        }
                        ++EndingTalkCount;
                        SoulRetrieveTimer = 1500;
                        break;
                    case 3:
                        if (!BrokenList.empty())
                        {
                            for(std::list<uint64>::iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                if (Unit* pUnit = Unit::GetUnit(*m_creature, *itr))
                                    {
                                         // This is the incorrect spell, but can't seem to find the right one.
                                         DoCast(pUnit, SPELL_AKAMA_SOUL_RETRIEVE);
                                         DoCast(pUnit, AKAMA_SOUL_EXPEL);
                                         pUnit->CastSpell(pUnit, 39656, true);
                                    }
                        }
                        ++EndingTalkCount;
                        SoulRetrieveTimer = 5000;
                        break;
                    case 4:
                        if (!BrokenList.empty())
                        {
                            for(std::list<uint64>::iterator itr = BrokenList.begin(); itr != BrokenList.end(); ++itr)
                                if (Unit* pUnit = Unit::GetUnit((*m_creature), *itr))
                                    DoScriptText(SAY_BROKEN_FREE_02, pUnit);
                        }
                        SoulRetrieveTimer = 0;
                        break;
                }
            }else SoulRetrieveTimer -= diff;
        }

        // If we don't have a target, or is talking, or has run away, return
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim()) return;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_akama_shade(Creature* pCreature)
{
    npc_akamaAI* Akama_AI = new npc_akamaAI(pCreature);

    for(uint8 i = 0; i < 16; ++i)
        Akama_AI->AddWaypoint(i, AkamaWP[i].x, AkamaWP[i].y, AkamaWP[i].z);

    return ((CreatureAI*)Akama_AI);
}


CreatureAI* GetAI_boss_shade_of_akama(Creature* pCreature)
{
    return new boss_shade_of_akamaAI(pCreature);
}

CreatureAI* GetAI_mob_ashtongue_channeler(Creature* pCreature)
{
    return new mob_ashtongue_channelerAI(pCreature);
}

CreatureAI* GetAI_mob_ashtongue_sorcerer(Creature* pCreature)
{
    return new mob_ashtongue_sorcererAI(pCreature);
}

bool GossipHello_npc_akama(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->isAlive())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_akama(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)               //Fight time
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ((npc_akamaAI*)pCreature->AI())->BeginEvent(pPlayer);
    }

    return true;
}

void AddSC_boss_shade_of_akama()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_shade_of_akama";
    newscript->GetAI = &GetAI_boss_shade_of_akama;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_channeler";
    newscript->GetAI = &GetAI_mob_ashtongue_channeler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ashtongue_sorcerer";
    newscript->GetAI = &GetAI_mob_ashtongue_sorcerer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_akama_shade";
    newscript->GetAI = &GetAI_npc_akama_shade;
    newscript->pGossipHello = &GossipHello_npc_akama;
    newscript->pGossipSelect = &GossipSelect_npc_akama;
    newscript->RegisterSelf();
}
