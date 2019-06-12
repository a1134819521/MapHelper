#pragma once

#include "stdafx.h"

struct TriggerType
{
	uint32_t flag;//0x0
	const char type[0x8c];//0x4
	const char value[0xc8];//0x90 ������TriggerData�ļ����TriggerTypeDefaults��Ĭ��ֵ
	const char base_type[0x30];//0x158
	uint32_t is_import_path;//0x188
	char unknow[0x14];//0x18c
};//size 0x1a0

struct TriggerConfigData
{
	char unknow[0x1c];
	uint32_t type_count; //0x1c
	TriggerType* array;//0x20
};

struct Parameter
{
	enum Type {
		invalid = -1,
		preset,
		variable,
		function,
		string
	};
	uint32_t table; //0x0
	uint32_t unknow2; //0x4
	uint32_t typeId; //0x8 ���������� -1 ~3
	char type_name[0x40];//0xc �������� �ַ���
	const char value[0x12c]; //0x4c
	struct Action* funcParam;//0x178 ��0ʱ��ʾ �˲�����������
	Parameter* arrayParam;//0x17c ��0ʱ ��ʾ�ò������������ ����ӵ���Ӳ���
};

struct Action
{
	enum Type {
		event,
		condition,
		action,
		none
	};

	struct VritualTable
	{
		uint32_t unknow1;
		uint32_t unknow2;
		uint32_t(__thiscall* getType)(void* pThis);
	};
	VritualTable* table; //0x0
	char unknow1[0x8];	 //0x4
	uint32_t child_count;	//0xc
	Action** child_actions;//0x10
	char unknow2[0xc];	 //0x14
	const char name[0x100]; //0x20
	uint32_t unknow3;//0x120;
	uint32_t unknow32;//0x124;
	uint32_t param_count; // 0x128
	Parameter** parameters;//0x12c
	char unknow4[0xC];//0x130
	uint32_t enable;//0x13c 
	char unknow5[0x14];//0x140
	uint32_t child_flag;//0x154 �������������Ӷ���ʱΪ0 ������-1
};

struct Trigger
{
	char unknow1[0xc];
	uint32_t line_count; //0xc
	Action** actions;	//0x10
	char unknow2[0x4];//0x14
	uint32_t is_comment; //0x18
	uint32_t unknow3; //0x1c
	uint32_t is_enable;  //0x20 
	uint32_t is_disable_init; //0x24
	uint32_t is_custom_srcipt;//0x28
	uint32_t is_initialize;//0x2c Ӧ��Ĭ�϶���1
	uint32_t unknow7;//0x30
	uint32_t custom_jass_size;//0x34
	const char* custom_jass_script;//0x38
	char unknow4[0x10]; //0x3c
	const char name[0x100];//0x4c
	uint32_t unknow5;//0x14c
	struct Categoriy* parent;//0x150 //�ô������ڵ��ļ���
	const char text[0x1000];//0x154 �����ı�ע�� ����δ֪���� ������˸�size

};

struct Categoriy
{
	uint32_t categoriy_id;
	const char categoriy_name[0x10C];
	uint32_t has_change; // 0x110
	uint32_t unknow2; // 0x114
	uint32_t is_comment; // 0x118
	char unknow[0x14];// 0x11c
	uint32_t trigger_count;//0x130 ��ǰ�����еĴ���������
	Trigger** triggers;		//0x134
};

struct VariableData
{
	uint32_t unknow1;	//0x0 δ֪ ����1
	uint32_t is_array;	//0x4
	uint32_t array_size;//0x8
	uint32_t is_init;	//0xc
	const char type[0x1E];//0x10
	const char name[0x64];//0x2e
	const char value[0x12e];//0x92
};

struct Variable
{
	char unknow1[0x8];		//0x0
	uint32_t globals_count;//0x8 ����gg_ ������ ����Ԥ�����ݵ�ȫ�ֱ���
	VariableData* array; //0xc
};

struct TriggerData
{
	uint32_t unknow1;		//0x0
	uint32_t trigger_count; // 0x4	���д���������
	char unknow2[0xC];		// 0x8
	uint32_t categoriy_count; //0x14 
	Categoriy** categories;	  //0x18
	uint32_t unknow3;		 //0x1c
	Variable* variables;    //0x20
	char unknow4[0x10]; // 0x24
	const char global_jass_comment[0x800];//0x34
	uint32_t unknow5; //0x834
	uint32_t globals_jass_size; //0x838
	const char* globals_jass_script;//0x83c
};



struct RandomGroup
{
	uint32_t rate;//0x0 �ٷֱ�
	const char names[10][4]; //һ���������10������
};

struct RandomGroupData
{
	uint32_t unknow1;//0x0
	const char name[0x64];//0x4
	uint32_t param_count;//0x68//��ʾ ����������ֵ�����
	char unknow3[0x2c];//0x6c
	uint32_t group_count;//0x98
	RandomGroup* groups;//0x9c
	uint32_t unknow2;//0xA0
};//size 0xA4

struct ItemTableInfo
{
	const char name[0x4];//��Ʒid
	uint32_t rate;//0x4 ����
};
struct ItemTableSetting
{
	uint32_t info_count;//0x0
	uint32_t info_count2;//0x4
	ItemTableInfo* item_infos;//0x8
	uint32_t unknow;//0xc
};//0x10

struct ItemTable
{
	uint32_t unknow1;//0x0
	const char name[0x64];//0x4;
	uint32_t setting_count;//0x68
	uint32_t setting_count2;//0x6c
	ItemTableSetting* item_setting;//0x70
	uint32_t unknow2;//0x74
};//size 0x78

struct UnitItem
{
	uint32_t slot_id;//0x0
	const char name[0x4];//0x4 ��Ʒid
};//size 0x8

struct UnitSkill
{
	const char name[0x4];//0x0 ����id
	uint32_t is_enable;//0x4 ���� �Ƿ��ʼ������ü���
	uint32_t level;//0x8 ��ʼ���ȼ�
};//size 0xc

struct Unit
{
	uint32_t unknow1;//0x0
	const char name[0x4];//0x4 ��λ���id
	uint32_t variation;//0x8 ��ʽ
	float x;//0xc	//�ڵ����е�����
	float y;//0x10
	float z;//0x14
	float angle;//0x18 ������ Ҫת�ؽǶ� * 180 / pi = �Ƕ���
	float sacle_x;//0x1c 
	float sacle_y;//0x20
	float scale_z;//0x24
	float sacle;//0x28
	char unknow2[0x54];//0x2c
	uint32_t color;//0x80 ��ɫ �����䷶Χ-1ʱ��0xFFFFFFFF ��-2ʱ��0xFF1010FF
	char unknow24[0x3]; //0x84
	uint8_t type;//0x87 ���� ��Ʒ��1
	char unknow22[0x34];//0x88
	uint32_t player_id;//0xbc
	uint32_t unknow13;//0xc0
	uint32_t health;//0xc4	�����ٷֱ� ��С1 ���ڻ����100��Ϊ��1
	uint32_t mana;//0xc8	ħ���ٷֱ� ��С1 ���ڻ����100��Ϊ��1
	uint32_t level;//0xcc	�ȼ�
	uint32_t state_str;//0xd0 ���� ����
	uint32_t state_agi;//0xd4 ���� ����
	uint32_t state_int;//0xd8 ���� ����
	uint32_t item_table_index;//0xdc ��Ʒ��������
	uint32_t item_setting_count;//0xe0
	uint32_t item_setting_count2;//0xe4
	ItemTableSetting* item_setting;//0xe8
	char unknow4[0x8];//0xec
	float warning_range;//0xf4 ���䷶Χ -1 ����ͨ -2 ��Ӫ��
	uint32_t item_count;//0xf8
	uint32_t item_count2;//0xfc
	UnitItem* items;//0x100
	uint32_t unknow14;//0x104
	uint32_t skill_count;//0x108
	uint32_t skill_count2;//0x10c
	UnitSkill* skills;//0x110
	char unknow3[0x4];//0x114

	//0x118 �����Ʒģʽ 0 Ϊ�κ���Ʒ ָ�� �ȼ�������  1 ��������� 2 �������Զ����б�
	uint32_t random_item_mode;//0x118

	uint8_t random_item_level;//0x11c
	char unknow23[0x2];//0x11d
	uint8_t random_item_type;//0x11f

	uint32_t random_group_index;//0x120 ������id �������ƷģʽΪ1ʱȡ��
	uint32_t random_group_child_index; // 0x124����� ����λ�� �����������Ϊ-1

	uint32_t random_item_count;//0x128
	uint32_t random_item_count2;//0x12c
	ItemTableInfo* random_items;//0x130

	char unknow25[0x18];//0x134
	uint32_t doodas_life;// 0x14c ���ƻ��������
	char unknow21[0x8];//0x150
	uint32_t index;//0x158 ȫ��Ԥ�������id
	char unknow28[0x2c];//0x15c
};//size 0x188

struct UnitData
{
	char unknow[0x5c];//0x0
	uint32_t unit_count;//0x5c
	Unit* array;//0x60
};


struct Sound
{
	const char name[0x64];//0x0 ��gg_snd_ ǰ׺��ȫ�ֱ�����
	const char file[0x104];//0x64
	const char effect[0x34];//0x168
	uint32_t flag;//0x19c  1 | 2 | 4   1���Ƿ�ѭ�� 2 �Ƿ�3D��Ч 4 ������Χֹͣ
	uint32_t fade_out_rate;//0x1a0 ������
	uint32_t fade_in_rate;//0x1a4 ������
	uint32_t volume;//0x1a8 ����
	float pitch;//0x1ac ���� = ����
	char unknow1[0x8];//0x1b0
	uint32_t channel;//0x1b8 ͨ��
	float min_range;//0x1bc ��С˥����Χ
	float max_range;//0x1c0 ���˥����Χ
	float distance_cutoff;//0x1c4 �ضϾ���
	float inside;//0x1c8
	float outside;//0x1cc
	uint32_t outsideVolume;//0x1d0
	float x;//0x1d4
	float y;//0x1d8
	float z;//0x1dc
};//size 0x1E0

struct SoundData
{
	uint32_t unknow1;//0x0
	uint32_t sound_count;//0x4
	Sound* array;//0x8
};


struct MapInfo
{
	char unknow1[0xc4];
	float minY;//0xc4 ��ͼ��Сy
	float minX;//0xc8 ��ͼ��Сx
	float maxY;//0xcc ��ͼ���Y
	float maxX;//0xd0 ��ͼ���X
};

struct Region
{
	uint32_t unknow1;//0x0 
	const char name[0x80];//0x4;
	int bottom;//0x84 �� ��Ҫ��ͼ  * 32 - ��ͼ��С����
	int left;//0x88 ��
	int top;//0x8c��
	int right;//0x90��
	MapInfo* info;//0x94
	char unknow3[0x44];//98
	char weather_id[0x4];//0xdc
	char unknow[0x8];//0xe0
	char sound_name[0x64];//0xe8
	uint32_t color;//0x14c
};

struct RegionData
{
	char unknow[0x60];//0x0
	uint32_t region_count;//0x60
	Region** array; //0x64
};


struct Camera
{
	uint32_t unknow1;//0x0
	float x;//0x4 
	float y;//0x8
	float z_offset;//0xc �߶�ƫ��
	float rotation;//0x10  z����ת
	float angle_of_attack;//0x14 x����תˮƽ
	float target_distance;//0x18 ��ͷ����
	float roll;//0x1c ����(����)
	float of_view;//0x20 �۲�����
	float farz;//0x24 Զ���ض�
	float unknow2;//0x28
	const char name[0x50];//0x2c
};//size 0x7c

struct CameraData
{
	char unknow[0x144]; //0x0
	uint32_t camera_count; //0x144
	Camera* array;//0x148
};

struct PlayerData
{
	uint32_t unknow1;//0x0
	uint32_t controller_id;//0x4 ������id  0�� 1��� 2���� 3���� 4��Ӫ�ȵ�
	uint32_t race;//0x8 ���� 
	uint32_t is_lock;//0xc �Ƿ�������ʼ��
	const char name[0x20];//0x10
	char unknow2[0x4c];//0x30
	uint32_t low_level;//0x7c �����ȼ�  & ���id��2���� �жϸ�����Ƿ��ǵ����ȼ�
	uint32_t height_level;//0x80 �����ȼ�  & ���id��2���� �жϸ�����Ƿ��Ǹ����ȼ�
}; //size 80

struct EditorData
{
	char unknow1[0x3884];// 0x0
	uint32_t player_count;//0x3884
	PlayerData* players;//0x3888
	char unknow11[0x38];//0x388c
	uint32_t random_group_count;//0x38c4���������
	RandomGroupData* random_groups;//0x38c8//�����
	char unknow2[0x8];//0x38cc
	uint32_t item_table_count;//0x38d4 ��Ʒ�б�����
	ItemTable* item_table;//0x38d8		��Ʒ��
	char unknow3[0x4];//0x38dc
	void* terrain;//0x38e0
	UnitData* doodas;//0x38e4
	UnitData* units;//0x38e8
	RegionData* regions;//0x38ec
	struct TriggerData* triggers;//0x38f0 //�����༭������
	CameraData* cameras; //0x38f4
	void* objects;//0x38f8
	SoundData* sounds; //0x38fc
};