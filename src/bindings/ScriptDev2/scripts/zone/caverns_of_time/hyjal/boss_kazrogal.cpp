#include "precompiled.h"
#include "def_hyjal.h"
#include "hyjal_trash.h"

#define SPELL_CLEAVE 31436
#define SPELL_WARSTOMP 31480
#define SPELL_MARK 31447

#define SOUND_ONDEATH 11018

#define SAY_ONSLAY1 "Shaza-Kiel!"
#define SAY_ONSLAY2 "You... are nothing!"
#define SAY_ONSLAY3 "Miserable nuisance!"
#define SOUND_ONSLAY1 11017
#define SOUND_ONSLAY2 11053
#define SOUND_ONSLAY3 11054

#define SAY_MARK1 "Your death will be a painful one."
#define SAY_MARK2 "You... are marked."
#define SOUND_MARK1 11016
#define SOUND_MARK2 11052

#define SAY_ONAGGRO "Cry for mercy! Your meaningless lives will soon be forfeit."
#define SOUND_ONAGGRO 11015

struct MANGOS_DLL_DECL boss_kazrogalAI : public ScriptedAI
{
    boss_kazrogalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_MARK);
        if(TempSpell && TempSpell->EffectImplicitTargetA[0] != 1)
        {
            TempSpell->EffectImplicitTargetA[0] = 1;
            TempSpell->EffectImplicitTargetB[0] = 0;
        }
        Reset();
    }

    uint32 CleaveTimer;
    uint32 WarStompTimer;
    uint32 MarkTimer;
    uint32 MarkTimerBase;
    bool go;
    uint32 pos;
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
        damageTaken = 0;
        CleaveTimer = 5000;
        WarStompTimer = 15000;
        MarkTimer = 45000;
        MarkTimerBase = 45000;

        if(pInstance && IsEvent)
            pInstance->SetData(DATA_KAZROGALEVENT, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {    
        if(pInstance && IsEvent)
            pInstance->SetData(DATA_KAZROGALEVENT, IN_PROGRESS);
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
            pInstance->SetData(DATA_KAZROGALEVENT, DONE);
        DoPlaySoundToSet(m_creature, SOUND_ONDEATH);
    }

    void UpdateAI(const uint32 diff)
    {

        //Return since we have no target
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if(CleaveTimer < diff)
        {
            DoCast(m_creature, SPELL_CLEAVE);
            CleaveTimer = 6000+rand()%15000;    
        }else CleaveTimer -= diff;

        if(WarStompTimer < diff)
        {
            DoCast(m_creature, SPELL_WARSTOMP);
            WarStompTimer = 60000;
        }else WarStompTimer -= diff;

        if(m_creature->HasAura(SPELL_MARK,0))
            m_creature->RemoveAurasDueToSpell(SPELL_MARK);
        if(MarkTimer < diff)
        {
            //cast dummy, useful for bos addons
            m_creature->CastCustomSpell(m_creature, SPELL_MARK, NULL, NULL, NULL, false, NULL, NULL, m_creature->GetGUID());            
            
            std::list<HostilReference *> t_list = m_creature->getThreatManager().getThreatList();
            for(std::list<HostilReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                if (target && target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA)
                {
                    target->CastSpell(target, SPELL_MARK,true);//only cast on mana users
                }
            }            
            MarkTimerBase -= 5000;
            if(MarkTimerBase < 5500)
                MarkTimerBase = 5500;
            MarkTimer = MarkTimerBase;            
            switch(rand()%3)
            {
                case 0:
                    DoPlaySoundToSet(m_creature, SOUND_MARK1);
                    DoYell(SAY_MARK1, LANG_UNIVERSAL, NULL);
                    break;
                case 1:
                    DoPlaySoundToSet(m_creature, SOUND_MARK2);
                    DoYell(SAY_MARK2, LANG_UNIVERSAL, NULL);
                    break;    
            }
        }else MarkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kazrogal(Creature* pCreature)
{
    return new boss_kazrogalAI(pCreature);
}

void AddSC_boss_kazrogal()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_kazrogal";
    newscript->GetAI = &GetAI_boss_kazrogal;
    newscript->RegisterSelf();
}
