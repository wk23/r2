#include "precompiled.h"
#include "def_hyjal.h"

#define SPELL_METEOR 33814 //infernal visual
#define SPELL_IMMOLATION 37059
#define SPELL_FLAME_BUFFET 31724
#define NPC_TRIGGER  21987 //World Trigger (Tiny)
#define MODEL_INVIS  11686 //invisible model

struct mob_giant_infernalAI : ScriptedAI
{
    mob_giant_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        meteor = false;//call once!
        CanMove = false;
        Delay = rand()%30000;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, MODEL_INVIS);
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);

    }
    ScriptedInstance* pInstance;
    bool meteor;
    bool CanMove;
    bool WpEnabled;
    bool go;
    uint32 pos;
    uint32 spawnTimer;
    uint32 FlameBuffetTimer;
    bool imol;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {        
        spawnTimer = 2000;
        FlameBuffetTimer= 2000;
        imol = false;
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(Delay<diff)
        {
            Delay=0;
        }else{
            Delay-=diff;
            return;
        }
        if (!meteor)
        {
            float x,y,z;
            m_creature->GetPosition(x,y,z);
            Creature* trigger = m_creature->SummonCreature(NPC_TRIGGER,x+8,y+8,z+25+rand()%10,m_creature->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,1000);
            if(trigger)
            {
                trigger->SetVisibility(VISIBILITY_OFF);
                trigger->setFaction(m_creature->getFaction());
                trigger->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                trigger->CastSpell(m_creature,SPELL_METEOR,true);
            }
            m_creature->GetMotionMaster()->Clear();
            meteor = true;
        }else if (!CanMove){
            if(spawnTimer<diff)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetUInt32Value(UNIT_FIELD_DISPLAYID, m_creature->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                CanMove = true;
                /*if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());*/
                if (pInstance)
                {
                    if (pInstance->GetData(DATA_ANETHERON))
                    {
                        Unit* target = Unit::GetUnit((*m_creature), pInstance->GetData64(DATA_THRALL));
                        if (target && target->isAlive())
                            {m_creature->AddThreat(target,0.0);m_creature->SetActiveObjectState(true);}
                    }
                }        
            }else spawnTimer -= diff;
        }
        if(!CanMove)return;

        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(!imol)
        {
            DoCast(m_creature,SPELL_IMMOLATION);
            imol=true;
        }
        if(FlameBuffetTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_FLAME_BUFFET,true);
            FlameBuffetTimer = 7000;
        }else FlameBuffetTimer -= diff;
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_giant_infernal(Creature* _Creature)
{
    return new mob_giant_infernalAI(_Creature);
}

#define SPELL_DISEASE_CLOUD 31607
#define SPELL_KNOCKDOWN 31610

struct mob_abominationAI : public ScriptedAI
{
    mob_abominationAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);

    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 KnockDownTimer;
    uint32 pos;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        KnockDownTimer = 10000;
    }

void DamageTaken(Unit *done_by, uint32 &damage)
{
    if(done_by->GetTypeId() == TYPEID_PLAYER || (done_by->GetTypeId() == TYPEID_UNIT && ((Creature*)done_by)->isPet()))
    {
        damageTaken += damage;
        if(pInstance)
            pInstance->SetData(DATA_RAIDDAMAGE,damage);//store raid's damage    
    }
}
void JustDied(Unit *victim)
{
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);

    if(pInstance->GetData(DATA_ANETHERON) != 0)//additional check
       if(pInstance->GetData(DATA_RAIDDAMAGE) < MINRAIDDAMAGE && !m_creature->isWorldBoss() )
          m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//no loot
       else m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//loot
}
    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->HasAura(SPELL_DISEASE_CLOUD,0))
            DoCast(m_creature,SPELL_DISEASE_CLOUD);
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(KnockDownTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_KNOCKDOWN);
            KnockDownTimer = 30000+rand()%25000;
        }else KnockDownTimer -= diff;
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_abomination(Creature* _Creature)
{
    return new mob_abominationAI(_Creature);
}

#define SPELL_FRENZY 31540

struct mob_ghoulAI : public ScriptedAI
{
    mob_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);
    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 FrenzyTimer;
    uint32 pos;
    uint32 MoveTimer;
    bool RandomMove;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        FrenzyTimer = 5000+rand()%5000;
        MoveTimer = 2000;
        RandomMove = false;
    }
void DamageTaken(Unit *done_by, uint32 &damage)
{
    if(done_by->GetTypeId() == TYPEID_PLAYER || (done_by->GetTypeId() == TYPEID_UNIT && ((Creature*)done_by)->isPet()))
    {
        damageTaken += damage;
        if(pInstance)
            pInstance->SetData(DATA_RAIDDAMAGE,damage);//store raid's damage    
    }
}
void JustDied(Unit *victim)
{
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);

    if(pInstance->GetData(DATA_ANETHERON) != 0)//additional check
       if(pInstance->GetData(DATA_RAIDDAMAGE) < MINRAIDDAMAGE && !m_creature->isWorldBoss() )
          m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//no loot
       else m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//loot
}
void Aggro(Unit* who) {}

void UpdateAI(const uint32 diff)
{
        //Return since we have no target
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(FrenzyTimer<diff)
        {
            DoCast(m_creature,SPELL_FRENZY);
            FrenzyTimer = 15000+rand()%15000;
        }else FrenzyTimer -= diff;        
        DoMeleeAttackIfReady();
}
};

CreatureAI* GetAI_mob_ghoul(Creature* _Creature)
{
    return new mob_ghoulAI(_Creature);
}

#define SPELL_RAISE_DEAD_1 31617
#define SPELL_RAISE_DEAD_2 31624
#define SPELL_RAISE_DEAD_3 31625
#define SPELL_SHADOW_BOLT 31627

struct mob_necromancerAI : public ScriptedAI
{
    mob_necromancerAI(Creature* pCreature) : ScriptedAI(pCreature), summons(m_creature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);

    }
    ScriptedInstance* pInstance;
    SummonList summons;
    bool go;
    uint32 ShadowBoltTimer;
    uint32 pos;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        ShadowBoltTimer = 1000+rand()%5000;
        summons.DespawnAll();
    }

void DamageTaken(Unit *done_by, uint32 &damage)
{
    if(done_by->GetTypeId() == TYPEID_PLAYER || (done_by->GetTypeId() == TYPEID_UNIT && ((Creature*)done_by)->isPet()))
    {
        damageTaken += damage;
        if(pInstance)
            pInstance->SetData(DATA_RAIDDAMAGE,damage);//store raid's damage    
    }
}
void JustDied(Unit *victim)
{
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);

    if(pInstance->GetData(DATA_ANETHERON) != 0)//additional check
       if(pInstance->GetData(DATA_RAIDDAMAGE) < MINRAIDDAMAGE && !m_creature->isWorldBoss() )
          m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//no loot
       else m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//loot
}
    void JustSummoned(Creature* summon)
    {
        Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
        if(target && summon)
            summon->Attack(target,false);
        summons.Summon(summon);
    }
    void SummonedCreatureDespawn(Creature *summon) {summons.Despawn(summon);}
    void KilledUnit(Unit* victim)
    {
        switch (rand()%3)
        {
            case 0:
                DoSpawnCreature(17902,3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                DoSpawnCreature(17902,-3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                break;
            case 1:
                DoSpawnCreature(17903,3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                DoSpawnCreature(17903,-3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                break;
            case 2:
                if(rand()%2)
                    DoSpawnCreature(17902,3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                else
                    DoSpawnCreature(17903,3,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 60000);
                break;
        }
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(ShadowBoltTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_SHADOW_BOLT);
            ShadowBoltTimer = 20000+rand()%10000;
        }else ShadowBoltTimer -= diff;        

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_necromancer(Creature* pCreature)
{
    return new mob_necromancerAI(pCreature);
}

#define SPELL_BANSHEE_CURSE 31651
#define SPELL_BANSHEE_WAIL 38183
#define SPELL_ANTI_MAGIC_SHELL 31662

struct mob_bansheeAI : public ScriptedAI
{
    mob_bansheeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);

    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 CourseTimer;
    uint32 WailTimer;
    uint32 ShellTimer;
    uint32 pos;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        CourseTimer = 20000+rand()%5000;
        WailTimer = 15000+rand()%5000;
        ShellTimer = 50000+rand()%10000;
    }

void DamageTaken(Unit *done_by, uint32 &damage)
{
    if(done_by->GetTypeId() == TYPEID_PLAYER || (done_by->GetTypeId() == TYPEID_UNIT && ((Creature*)done_by)->isPet()))
    {
        damageTaken += damage;
        if(pInstance)
            pInstance->SetData(DATA_RAIDDAMAGE,damage);//store raid's damage    
    }
}
void JustDied(Unit *victim)
{
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);

    if(pInstance->GetData(DATA_ANETHERON) != 0)//additional check
       if(pInstance->GetData(DATA_RAIDDAMAGE) < MINRAIDDAMAGE && !m_creature->isWorldBoss() )
          m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//no loot
       else m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//loot
}

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(CourseTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_BANSHEE_CURSE);
            CourseTimer = 20000+rand()%5000;
        }else CourseTimer -= diff;
        if(WailTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_BANSHEE_WAIL);
            WailTimer = 15000+rand()%5000;
        }else WailTimer -= diff;
        if(ShellTimer<diff)
        {
            DoCast(m_creature,SPELL_ANTI_MAGIC_SHELL);
            ShellTimer = 50000+rand()%10000;
        }else ShellTimer -= diff;
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_banshee(Creature* pCreature)
{
    return new mob_bansheeAI(pCreature);
}

#define SPELL_WEB 28991

struct mob_crypt_fiendAI : public ScriptedAI
{
    mob_crypt_fiendAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
        //pCreature->SetActiveObjectState(true);

    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 WebTimer;
    uint32 pos;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        WebTimer = 20000+rand()%5000;
    }

void DamageTaken(Unit *done_by, uint32 &damage)
{
    if(done_by->GetTypeId() == TYPEID_PLAYER || (done_by->GetTypeId() == TYPEID_UNIT && ((Creature*)done_by)->isPet()))
    {
        damageTaken += damage;
        if(pInstance)
            pInstance->SetData(DATA_RAIDDAMAGE,damage);//store raid's damage    
    }
}
void JustDied(Unit *victim)
{
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);

    if(pInstance->GetData(DATA_ANETHERON) != 0)//additional check
       if(pInstance->GetData(DATA_RAIDDAMAGE) < MINRAIDDAMAGE && !m_creature->isWorldBoss() )
          m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//no loot
       else m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);//loot
}

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(WebTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_WEB);
            WebTimer = 20000+rand()%5000;
        }else WebTimer -= diff;
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_crypt_fiend(Creature* pCreature)
{
    return new mob_crypt_fiendAI(pCreature);
}

#define SPELL_MANA_BURN 31729

struct mob_fel_stalkerAI : public ScriptedAI
{
    mob_fel_stalkerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;    
        pos = 0;
        Reset();
    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 ManaBurnTimer;
    uint32 pos;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        ManaBurnTimer = 9000+rand()%5000;
    }
    void JustDied(Unit *victim)
    {
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(ManaBurnTimer<diff)
        {
            DoCast(m_creature->getVictim(),SPELL_MANA_BURN);
            ManaBurnTimer = 9000+rand()%5000;
        }else ManaBurnTimer -= diff;
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_fel_stalker(Creature* pCreature)
{
    return new mob_fel_stalkerAI(pCreature);
}

#define SPELL_FROST_BREATH 31688

struct mob_frost_wyrmAI : public ScriptedAI
{
    mob_frost_wyrmAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;
        pos = 0;
        Reset();
    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 FrostBreathTimer;
    uint32 pos;
    uint32 MoveTimer;
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        FrostBreathTimer = 5000;
        MoveTimer = 0;        
        m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
    }

    void JustDied(Unit *victim)
    {
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        z = m_creature->GetMap()->GetHeight(x, y, z, true);
        m_creature->GetMotionMaster()->MovePoint(0,x,y,z);
        m_creature->Relocate(x,y,z,0);
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);
    }

    void Aggro(Unit* who) {}

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(m_creature->GetDistance(m_creature->getVictim()) >= 25){
            if(MoveTimer<diff)
            {
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                MoveTimer = 2000;
            }else MoveTimer-=diff;
        }
                
        if(FrostBreathTimer<diff)
        {
            if(m_creature->GetDistance(m_creature->getVictim()) < 25)
            {
                DoCast(m_creature->getVictim(),SPELL_FROST_BREATH);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                FrostBreathTimer = 4000;
            }            
        }else FrostBreathTimer -= diff;
    }
};


CreatureAI* GetAI_mob_frost_wyrm(Creature* pCreature)
{
    return new mob_frost_wyrmAI(pCreature);
}

#define SPELL_GARGOYLE_STRIKE 31664

struct mob_gargoyleAI : public ScriptedAI
{
    mob_gargoyleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        go = false;
        pos = 0;
        DummyTarget[0] = 0;DummyTarget[1] = 0;DummyTarget[2] = 0;
        Reset();
    }
    ScriptedInstance* pInstance;
    bool go;
    uint32 StrikeTimer;
    uint32 pos;
    uint32 MoveTimer;
    float Zpos;
    bool forcemove;    
        uint32 Delay;
        uint8 faction;
        bool useFlyPath;
        uint32 damageTaken;
        float DummyTarget[3];
    void Reset()
    {
        forcemove = true;
        Zpos = 10.0;
        StrikeTimer = 2000+rand()%5000;
        MoveTimer = 0;        
        m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);        
    }

    void JustDied(Unit *victim)
    {
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        z = m_creature->GetMap()->GetHeight(x, y, z, true);
        m_creature->GetMotionMaster()->MovePoint(0,x,y,z);
        m_creature->Relocate(x,y,z,0);
        uint32 EnemyCount;
        EnemyCount=pInstance->GetData(DATA_TRASH);
        --EnemyCount;
        pInstance->SetData(DATA_TRASH, EnemyCount);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(m_creature->GetDistance(m_creature->getVictim()) >= 20 || forcemove)
        {
            forcemove = false;
            if(forcemove)
            {
                Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if(target)
                    m_creature->Attack(target,false);
            }
            if(MoveTimer<diff)
            {
                float x,y,z;
                m_creature->getVictim()->GetPosition(x,y,z);
                m_creature->GetMotionMaster()->MovePoint(0,x,y,z+Zpos);
                Zpos-=1.0;
                if(Zpos<=0)Zpos=0;                
                MoveTimer = 2000;
            }else MoveTimer-=diff;
        }                
        if(StrikeTimer<diff)
        {
            if(m_creature->GetDistance(m_creature->getVictim()) < 20)
            {
                DoCast(m_creature->getVictim(),SPELL_GARGOYLE_STRIKE);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                StrikeTimer = 2000+rand()%1000;
            }else StrikeTimer=0;
        }else StrikeTimer -= diff;
    }
};


CreatureAI* GetAI_mob_gargoyle(Creature* pCreature)
{
    return new mob_gargoyleAI(pCreature);
}

#define SPELL_EXPLODING_SHOT 7896

struct MANGOS_DLL_DECL alliance_riflemanAI : public Scripted_NoMovementAI
{
    alliance_riflemanAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 ExplodeTimer;

    void JustDied(Unit*)
    {
    }

    void Reset()
    {
        ExplodeTimer = 5000+rand()%5000;
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, 30))
            {
                AttackStart(who);
            }
        }
    }

    void Aggro(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        //Check if we have a target
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;
        if(ExplodeTimer < diff)
        {
            if (!m_creature->IsWithinDistInMap(m_creature->getVictim(), 30))
            {
                EnterEvadeMode();
                return;
            }
            int dmg = 500+rand()%700;
            m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_EXPLODING_SHOT, &dmg, 0, 0, false);
            ExplodeTimer = 5000+rand()%5000;
        }else ExplodeTimer -= diff;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_alliance_rifleman(Creature* pCreature)
{
    return new alliance_riflemanAI(pCreature);
}

void AddSC_hyjal_trash()
{
    Script *newscript = new Script;
    newscript->Name = "mob_giant_infernal";
    newscript->GetAI = &GetAI_mob_giant_infernal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_abomination";
    newscript->GetAI = &GetAI_mob_abomination;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ghoul";
    newscript->GetAI = &GetAI_mob_ghoul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_necromancer";
    newscript->GetAI = &GetAI_mob_necromancer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_banshee";
    newscript->GetAI = &GetAI_mob_banshee;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_crypt_fiend";
    newscript->GetAI = &GetAI_mob_crypt_fiend;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_fel_stalker";
    newscript->GetAI = &GetAI_mob_fel_stalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frost_wyrm";
    newscript->GetAI = &GetAI_mob_frost_wyrm;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_gargoyle";
    newscript->GetAI = &GetAI_mob_gargoyle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "alliance_rifleman";
    newscript->GetAI = &GetAI_alliance_rifleman;
    newscript->RegisterSelf();
}
