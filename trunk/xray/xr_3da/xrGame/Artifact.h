#pragma once

#include "hud_item_object.h"
#include "hit_immunity.h"
#include "PHObject.h"
#include "script_export_space.h"

struct SArtefactActivation;

class CArtefact :	public CHudItemObject, 
					public CPHUpdateObject {
private:
	typedef			CHudItemObject	inherited;

public:
									CArtefact						();
	virtual							~CArtefact						();

	virtual void					Load							(LPCSTR section);
	
	virtual BOOL					net_Spawn						(CSE_Abstract* DC);
	virtual void					net_Destroy						();

	virtual void					OnH_A_Chield					();
	virtual void					OnH_B_Independent				(bool just_before_destroy);
	
	virtual void					UpdateCL						();
	virtual void					shedule_Update					(u32 dt);	
			void					UpdateWorkload					(u32 dt);

	
	virtual bool					CanTake							() const;

	//virtual void					renderable_Render				();
	virtual BOOL					renderable_ShadowGenerate		()		{ return FALSE;	}
	virtual BOOL					renderable_ShadowReceive		()		{ return TRUE;	}
	virtual void					create_physic_shell();

	//for smart_cast
	virtual CArtefact*				cast_artefact						()		{return this;}

protected:
	virtual void					UpdateCLChild						()		{};

	u16								m_CarringBoneID;
	shared_str						m_sParticlesName;
protected:
	SArtefactActivation*			m_activationObj;
	//////////////////////////////////////////////////////////////////////////
	//	Lights
	//////////////////////////////////////////////////////////////////////////
	//����, ��� ��������� ����� ���� ��������
	bool							m_bLightsEnabled;

	//��������� �� ����� ������ � ������ ���������
	ref_light						m_pTrailLight;
	Fcolor							m_TrailLightColor;
	float							m_fTrailLightRange;

	u8								m_af_rank;

protected:
	virtual void					UpdateLights();
	
public:
	IC u8							GetAfRank						() const		{return m_af_rank;}
	
	virtual void					StartLights();
	virtual void					StopLights();
	void							ActivateArtefact					();
	bool							CanBeActivated						()				{return m_bCanSpawnZone;};// does artefact can spawn anomaly zone

	virtual void					PhDataUpdate						(dReal step);
	virtual void					PhTune								(dReal step)	{};

	bool							m_bCanSpawnZone;
	
	float							m_fHealthRestoreSpeed;
#ifndef OBJECTS_RADIOACTIVE
	float 							m_fRadiationRestoreSpeed;
#endif
	float 							m_fSatietyRestoreSpeed;
	float 							m_fThirstRestoreSpeed;
	float							m_fPowerRestoreSpeed;
	float							m_fBleedingRestoreSpeed;
	float							m_fPowerLifeRestoreSpeed;
	float							m_additional_weight;
	float							m_fConditionLoss;

	CHitImmunity 					m_ArtefactHitImmunities;

	// Kondr48: ���������� ���������
	

	float							GetHealthRestoreSpeed           () {return m_fHealthRestoreSpeed    * m_fCondition;}
	float 							GetSatietyRestoreSpeed          () {return m_fSatietyRestoreSpeed   * m_fCondition;}
	float 							GetThirstRestoreSpeed           () {return m_fThirstRestoreSpeed    * m_fCondition;}
	float 							GetPowerRestoreSpeed            () {return m_fPowerRestoreSpeed     * m_fCondition;}
	float 							GetBleedingRestoreSpeed         () {return m_fBleedingRestoreSpeed  * m_fCondition;}
	float 							GetPowerLifeRestoreSpeed        () {return m_fPowerLifeRestoreSpeed * m_fCondition;}
	float							GetAdditionalInventoryWeight	() {return m_additional_weight      * m_fCondition;}

#ifndef OBJECTS_RADIOACTIVE
	float							GetRadiationRestoreSpeed	    () {return m_fRadiationRestoreSpeed * m_fCondition;}
#endif
	                     
protected:
	MotionSVec						m_anim_idle;
	MotionSVec						m_anim_idle_sprint;
	MotionSVec						m_anim_hide;
	MotionSVec						m_anim_show;
	MotionSVec						m_anim_activate;
public:
	enum EAFHudStates {
		eIdle		= 0,
		eShowing,
		eHiding,
		eHidden,
		eActivating,
	};
	virtual	void					PlayAnimIdle		();
public:
	virtual void					Hide				();
	virtual void					Show				();
	virtual	void					UpdateXForm			();
	virtual bool					Action				(s32 cmd, u32 flags);
	virtual void					onMovementChanged	(ACTOR_DEFS::EMoveCommand cmd);
	virtual void					OnStateSwitch		(u32 S);
	virtual void					OnAnimationEnd		(u32 state);
	virtual bool					IsHidden			()	const	{return GetState()==eHidden;}
	virtual u16						bone_count_to_synchronize	() const;

	void				ChangeCondition		(float fDeltaCondition);

	// optimization FAST/SLOW mode
public:						
	u32						o_render_frame				;
	BOOL					o_fastmode					;
	IC void					o_switch_2_fast				()	{
		if (o_fastmode)		return	;
		o_fastmode			= TRUE	;
		//processing_activate		();
	}
	IC void					o_switch_2_slow				()	{
		if (!o_fastmode)	return	;
		o_fastmode			= FALSE	;
		//processing_deactivate		();
	}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CArtefact)
#undef script_type_list
#define script_type_list save_type_list(CArtefact)

