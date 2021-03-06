#include "stdafx.h"
#include "UIHackWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIDragDropListEx.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "../level.h"
#include "../game_base_space.h"
#include "UIHelper.h"
#include "../Actor.h"
#include "../ActorCondition.h"
#include "UIInventoryUtilities.h"
#include "../inventory.h"
#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UI3tButton.h"
#include "../string_table.h"
#include "../pch_script.h"
#include "../script_game_object.h"
#include "../script_game_object_impl.h"

CUIHackWnd::CUIHackWnd()
{
	hack_time = 0;
}

CUIHackWnd::~CUIHackWnd()
{
  UiActorDevivesList->ClearAll					(true);
}

void CUIHackWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "pda_hack.xml");
	R_ASSERT3(xml_result, "xml file not found", "pda_hack.xml");
	CUIXmlInit xml_init;

	xml_init.InitWindow				      (uiXml, "main_wnd", 0, this);

	UILeftFrame						      = xr_new<CUIFrameWindow>();UILeftFrame->SetAutoDelete(true);
	AttachChild							  (UILeftFrame);
	xml_init.InitFrameWindow			  (uiXml, "left_frame_window", 0, UILeftFrame);

	UIRightFrame						  = xr_new<CUIFrameWindow>();UIRightFrame->SetAutoDelete(true);
	AttachChild							  (UIRightFrame);
	xml_init.InitFrameWindow			  (uiXml, "right_frame_window", 0, UIRightFrame);

	UIDeviceListHeader				      = xr_new<CUIFrameLineWnd>();UIDeviceListHeader->SetAutoDelete(true);
	UILeftFrame->AttachChild		      (UIDeviceListHeader);
	xml_init.InitFrameLine				  (uiXml, "devices_list_header", 0, UIDeviceListHeader);

	UIDeviceListAnimation				  = xr_new<CUIAnimatedStatic>();UIDeviceListAnimation->SetAutoDelete(true);
	UIDeviceListHeader->AttachChild		  (UIDeviceListAnimation);
	xml_init.InitAnimatedStatic			  (uiXml, "devices_list_a_static", 0, UIDeviceListAnimation);

	UiActorDevivesList                    = UIHelper::CreateDragDropListEx(uiXml, "dragdrop_devices_list", this);
	BindDragDropListEnents				  (UiActorDevivesList);

	m_device_soft_version                 = UIHelper::CreateStatic(uiXml, "device_soft_version", this);
	m_device_name		                  = UIHelper::CreateStatic(uiXml, "device_name", this);
	m_device_icon                         = UIHelper::CreateStatic(uiXml, "device_icon", this);
	m_device_encryption_level             = UIHelper::CreateStatic(uiXml, "device_encryption_level", this);
	m_device_hack_time                    = UIHelper::CreateStatic(uiXml, "device_hack_time", this);
	m_hacked_start_button                 = UIHelper::Create3tButton(uiXml, "hacked_start_button", this);
	m_hacked_stop_button                  = UIHelper::Create3tButton(uiXml, "hacked_stop_button", this);
	m_hack_console                        = UIHelper::CreateStatic(uiXml, "hack_console", this);

	ClearRightWindow();
}

void CUIHackWnd::Draw()
{
	CInventoryOwner *pInvOwner	= smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	CActor*	m_pActor = smart_cast<CActor*>(Level().CurrentViewEntity());

	if(!pInvOwner || !m_pActor)				return;

	m_pRuck						= &pInvOwner->inventory();
	
	UiActorDevivesList->ClearAll					(true);
	
	TIItemContainer::iterator itb = m_pRuck->m_ruck.begin();
    TIItemContainer::iterator ite = m_pRuck->m_ruck.end();

	for (; itb != ite; ++itb)
	{
		if ((*itb)->m_can_be_hacked)
		{
		   CUICellItem* itm = create_cell_item(*itb);
		   if((*itb)->m_quest_device) itm->SetColorAll(color_argb(255,205,227,11)); // ������� ����� �����-�������
		   UiActorDevivesList->SetItem(itm);
		}
	}

	luabind::functor<float> get_hack_time;
	if (ai().script_engine().functor("hack_module.get_hack_time", get_hack_time))
	hack_time = get_hack_time();

	if (hack_time > 0) // �����-�� ������ ��� ������������. ������� ���� � ������
	{			
		LPCSTR header_hack_time = *CStringTable().translate("hckd_time");
		int hours   = iFloor(hack_time/3600);
		int minutes = iFloor(hack_time/60 - hours*60);
		sprintf_s(text_hack_time, "%s: %02i:%02i", header_hack_time, hours, minutes);
		m_device_hack_time->SetText(text_hack_time);
		m_device_hack_time->Show(true);
		m_hack_console->Show(true);
		m_hacked_start_button->Enable(false);
	    m_hacked_stop_button->Enable(true);

        luabind::functor<LPCSTR> get_hacked_device;
		if (ai().script_engine().functor("hack_module.get_hacked_device", get_hacked_device))
		{
		   hacked_device = get_hacked_device();
		   ShowRightWindow(hacked_device);
		}
	} 
	else 
	{
		m_hacked_start_button->Enable(true);
	    m_hacked_stop_button->Enable(false);
		m_device_hack_time->Show(false);
		m_hack_console->Show(false);
	}

	inherited::Draw();
}

void CUIHackWnd::BindDragDropListEnents(CUIDragDropListEx* lst)
{
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIHackWnd::OnItemSelected);
}

void CUIHackWnd::ShowRightWindow(LPCSTR section)
{
	
	// �������� ��������
	LPCSTR header_name = *CStringTable().translate("hckd_device_name");
	hacked_device_name = pSettings->r_string(section, "inv_name");
	hacked_device_name = *CStringTable().translate(hacked_device_name);
	sprintf_s(text_name, "%s: %s", header_name, hacked_device_name.c_str());
	m_device_name->SetText(text_name);
	m_device_name->Show(true);     

	// ������� ����������
	LPCSTR header_encryption_level = *CStringTable().translate("hckd_encryption_level");
	hacked_encryption_level = pSettings->r_string(section, "encryption_level");
	hacked_encryption_level = *CStringTable().translate(hacked_encryption_level);
	sprintf_s(text_encryption_level, "%s: %s", header_encryption_level, hacked_encryption_level.c_str());
	m_device_encryption_level->SetText(text_encryption_level);
	m_device_encryption_level->Show(true);    
	
	// ������ ��������
	CActor*	m_pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	PIItem pda = m_pActor->inventory().ItemFromSlot(PDA_SLOT);
	soft_ver = pda->m_soft_level;
	LPCSTR header_m_device_soft_version = *CStringTable().translate("hckd_soft_version");
	sprintf_s(text_soft_version, "%s: 1.35.%02d", header_m_device_soft_version, soft_ver);
	m_device_soft_version->SetText(text_soft_version);
	m_device_soft_version->Show(true);

	// ������ ������������� ��������
	m_device_icon->InitTexture("ui\\ui_icon_equipment"); 
	float x = pSettings->r_float(section, "inv_grid_x");
	float y = pSettings->r_float(section, "inv_grid_y");
	float w = pSettings->r_float(section, "inv_grid_width");
	float h = pSettings->r_float(section, "inv_grid_height");
	m_device_icon->SetOriginalRect(x*50, y*50, w*50, h*50);
	m_device_icon->Show(true);

	// ������
	m_hacked_start_button->Show(true);  // �����
	m_hacked_stop_button->Show(true);   // ������
}

bool CUIHackWnd::OnItemSelected(CUICellItem* itm)
{	
	if (hack_time > 0) return false;

	PIItem	   item		     = (PIItem)itm->m_pData;
	item_section = item->item_section.c_str();
	
	ShowRightWindow          (item_section);

	return				      true;
}

void CUIHackWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == m_hacked_start_button && msg == BUTTON_CLICKED)
	{
	  luabind::functor<void>	StartHackDevice;
      if (ai().script_engine().functor("hack_module.StartHackDevice",StartHackDevice))
		StartHackDevice(item_section);
	}
	else if(pWnd == m_hacked_stop_button && msg == BUTTON_CLICKED)
	{
      luabind::functor<void>	StopHackDevice;
      if (ai().script_engine().functor("hack_module.StopHackDevice",StopHackDevice))
	  {
		StopHackDevice();
        ClearRightWindow();
	  }
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}

void CUIHackWnd::ClearRightWindow()
{
	m_device_soft_version->Show(false);
	m_device_name->Show(false);
	m_device_icon->Show(false);
	m_device_encryption_level->Show(false);
	m_device_hack_time->Show(false);
	m_hacked_start_button->Show(false);
	m_hacked_stop_button->Show(false);
	m_hack_console->Show(false);
}