#include "precompiled.h"
#include "def_hyjal.h"
#include "hyjal_trash.h"

#define SPELL_RAIN_OF_FIRE 31340
#define SPELL_DOOM 31347
#define SPELL_HOWL_OF_AZGALOR 31344
#define SPELL_CLEAVE 31345
#define SPELL_BERSERK 26662

#define SAY_ONDEATH "Your time is almost... up"
#define SOUND_ONDEATH 11002

#define SAY_ONSLAY1 "Reesh, hokta!"
#define SAY_ONSLAY2 "Don't fight it"
#define SAY_ONSLAY3 "No one is going to save you"
#define SOUND_ONSLAY1 11001
#define SOUND_ONSLAY2 11048
#define SOUND_ONSLAY3 11047

#define SAY_DOOM1 "Just a taste... of what awaits you"
#define SAY_DOOM2 "Suffer you despicable insect!"
#define SOUND_DOOM1 11046
#define SOUND_DOOM2 11000

#define SAY_ONAGGRO "Abandon all hope! The legion has returned to finish what was begun so many years ago. This time there will be no escape!"
#define SOUND_ONAGGRO 10999

struct MANGOS_DLL_DECL boss_azgalorAI : public ScriptedAI
{
    boss_azgalorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_HOWL_OF_AZGALOR);
        if(TempSpell)
            TempSpell->EffectRadiusIndex[0] = 12;//100yards instead of 50000?!
        SpellEntry *TempSpell1 = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_DOOM);
        if(TempSpell1)
         {
            TempSpell1->EffectImplicitTargetA[0] = 1;
            TempSpell1->EffectImplicitTargetB[0] = 0;
        }

    }

    uint32 RainTimer;
    uint32 DoomTimer;
    uint32 HowlTimer;
    uint32 CleaveTimer;
    uint32 EnrageTimer;
    bool enraged;
        ScriptedInstance* pInstance;
        bool IsEvent;
        uint32 Delay;
        uint32 LastOverronPos;
        bool IsOverrun;
        bool SetupOverrun;
        uint32 OverrunType;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];

    bool go;
    uint32 pos;

    void Reset()
    {
        RainTimer = 20000;
        DoomTimer = 50000;
        HowlTimer = 30000;
        CleaveTimer = 10000;
        EnrageTimer = 600000;
        enraged = false;

        if(pInstance && IsEvent)
            pInstance->SetData(DATA_AZGALOREVENT, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {    
        if(pInstance && IsEvent)
            pInstance->SetData(DATA_AZGALOREVENT, IN_PROGRESS);
        DoPlaySoundToSet(m_creature, SOUND_ONAGGRO);
        DoYell(SAY_ONAGGRO, LANG_UNIVERSAL, NULL);
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%3)
        {
            case 0:
                DoPlaySoundToSet(m_creature, SOUND_ONSLAY1);
                DoYell(SAY_ONSLAY1, LANG_UNIVERSAL, NULL);
                break;
            case 1:
                DoPlaySoundToSet(m_creature, SOUND_ONSLAY2);
                DoYell(SAY_ONSLAY2, LANG_UNIVERSAL, NULL);
                break;        
            case 2:
                DoPlaySoundToSet(m_creature, SOUND_ONSLAY3);
                DoYell(SAY_ONSLAY3, LANG_UNIVERSAL, NULL);
                break;    
        }        
    }

    void JustDied(Unit *victim)
    {
        if(pInstance && IsEvent)
            {                    
            pInstance->SetData(DATA_AZGALOREVENT, DONE);

            Unit* pCreature =  Unit::GetUnit((*m_creature), pInstance->GetData64(DATA_AZGALOR));
                        if(pCreature)
                        {                    
                            Creature* pUnit = pCreature->SummonCreature(21987,pCreature->GetPositionX(),pCreature->GetPositionY(),pCreature->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,10000);
            
                            Map *map = pCreature->GetMap();
                            if (map->IsDungeon() && pUnit)
                            {
                                pUnit->SetVisibility(VISIBILITY_OFF);
                                Map::PlayerList const &PlayerList = map->GetPlayers();
                                if (PlayerList.isEmpty())
                                     return;
                              
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                {
                                     if (i->getSource())
                                     {
                                        WorldPacket data(SMSG_MESSAGECHAT, 200);                
                                        pUnit->BuildMonsterChat(&data,CHAT_MSG_MONSTER_YELL,"All of your efforts have been in vain, for the draining of the World Tree has already begun. Soon the heart of your world will beat no more.",0,"Archimonde",i->getSource()->GetGUID());
                                        i->getSource()->GetSession()->SendPacket(&data);

                                        WorldPacket data2(SMSG_PLAY_SOUND, 4);
                                        data2 << 10986;
                                        i->getSource()->GetSession()->SendPacket(&data2);
                                     }
                                }
                            }
                        }
                    }
        DoPlaySoundToSet(m_creature, SOUND_ONDEATH);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if(RainTimer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_RAIN_OF_FIRE);
            RainTimer = 20000+rand()%15000;
        }else RainTimer -= diff;

        if(DoomTimer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 1), SPELL_DOOM);//never on tank
            DoomTimer = 45000+rand()%5000;
        }else DoomTimer -= diff;

        if(HowlTimer < diff)
        {
            DoCast(m_creature, SPELL_HOWL_OF_AZGALOR);
            HowlTimer = 30000;
        }else HowlTimer -= diff;

        if(CleaveTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE);
            CleaveTimer = 10000+rand()%5000;    
        }else CleaveTimer -= diff;

        if(EnrageTimer < diff && !enraged)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_BERSERK, true);
            enraged = true;
            EnrageTimer = 600000;
        }else EnrageTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_azgalor(Creature* pCreature)
{
    return new boss_azgalorAI(pCreature);
}

#define SPELL_THRASH 12787
#define SPELL_CRIPPLE 31406
#define SPELL_WARSTOMP 31408

struct MANGOS_DLL_DECL mob_lesser_doomguardAI : public ScriptedAI
{
    mob_lesser_doomguardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        if(pInstance)
            AzgalorGUID = pInstance->GetData64(DATA_AZGALOR);
    }

    uint32 CrippleTimer;
    uint32 WarstompTimer;
    uint32 CheckTimer;
    uint64 AzgalorGUID;
        ScriptedInstance* pInstance;
        bool IsEvent;
        uint32 Delay;
        uint32 LastOverronPos;
        bool IsOverrun;
        bool SetupOverrun;
        uint32 OverrunType;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];


    void Reset()
    {
        CrippleTimer = 50000;
        WarstompTimer = 10000;
        DoCast(m_creature, SPELL_THRASH);
        CheckTimer = 5000;
    }

    void Aggro(Unit *who)
    {    
    }

    void KilledUnit(Unit *victim)
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (m_creature->GetDistance(who) <= 50 && !m_creature->isInCombat() && m_creature->IsHostileTo(who))
        {
            m_creature->AddThreat(who,0.0);
            m_creature->SetActiveObjectState(true);
            m_creature->Attack(who,false);
        }
    }

    void JustDied(Unit *victim)
    {      
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if(WarstompTimer < diff)
        {
            DoCast(m_creature, SPELL_WARSTOMP);
            WarstompTimer = 10000+rand()%5000;
        }else WarstompTimer -= diff;

        if(CrippleTimer < diff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_CRIPPLE);
            CrippleTimer = 25000+rand()%5000;
        }else CrippleTimer -= diff;        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_lesser_doomguard(Creature* pCreature)
{
    return new mob_lesser_doomguardAI(pCreature);
}

void AddSC_boss_azgalor()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_azgalor";
    newscript->GetAI = &GetAI_boss_azgalor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_lesser_doomguard";
    newscript->GetAI = &GetAI_mob_lesser_doomguard;
    newscript->RegisterSelf();
}
