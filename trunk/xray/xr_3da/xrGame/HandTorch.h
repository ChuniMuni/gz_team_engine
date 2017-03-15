////////////////////////////////////////////////////////////////////////////
//	Module 		: HandTorch.h
//	Created 	: 15.02.2017
//  Modified 	: 04.03.2017
//	Author		: Kondr48
//	Description : ������ ������
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "HudItemBase.h"

class CLAItem;

class CHandTorch:	public CHudItemBase {
private:
	typedef			CHudItemBase	inherited;

protected:
	shared_str		light_trace_bone;

public:
									CHandTorch						();
	virtual							~CHandTorch						();

	virtual void					Load							(LPCSTR section);
	
	virtual BOOL					net_Spawn						(CSE_Abstract* DC);
	
	virtual CHandTorch*				cast_hand_torch					()		{return this;}

protected:

	virtual void	                Switch(bool turn);

	ref_light		                light_render;
	ref_light		                light_omni;
	ref_glow		                glow_render;

	CLAItem*		                lanim;
	float			                fBrightness;

	float			                m_delta_h;
	Fvector2		                m_prev_hp;

public:

	virtual void					UpdateCL			();
	virtual void					OnStateSwitch		(u32 S);
	virtual void					OnAnimationEnd		(u32 state);
};
