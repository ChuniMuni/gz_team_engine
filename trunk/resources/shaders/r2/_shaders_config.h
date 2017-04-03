#ifndef _SHADERS_CONFIG_H
#define _SHADERS_CONFIG_H

/////////////////////////////////////////////////////////////
// improved parallax occlusion mapping
	#define POM_PARALLAX_OFFSET float(0.015) //(0.02)			// ��������. ��� ������, ��� ������ ����� ��������� �������.

/////////////////////////////////////////////////////////////
// ����
	#define SW_USE_FOAM								// �������� "����" ������
	#define SW_FOAM_THICKNESS float (0.035)			// ������� "����"
	#define SW_WATER_INTENSITY float (1.0)			// ������� ����� ����

/////////////////////////////////////////////////////////////
// ����� �� �����
	#define USE_GRASS_WAVE							// �������� "�����" �� ����� �� �����
	#define GRASS_WAVE_POWER float(1.4) //(2.0)				// "�������", ���������� ����
	#define GRASS_WAVE_FREQ float(0.5) //(0.7)				// ������� ��������� ����
	
#endif // _SHADERS_CONFIG_H	