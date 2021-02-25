#pragma once

#include "stdafx.h"
#include "mapHelper.h"
#include "EditorData.h"
#include "ActionNode.h"

class TriggerEditor
{
public:
	TriggerEditor();
	~TriggerEditor();

	//static TriggerEditor* getInstance();

	void loadTriggers(TriggerData* data);
	void loadTriggerConfig(TriggerConfigData* data);

	void saveTriggers(const char* path); //����wtg
	void saveScriptTriggers(const char* path);//���� wct
	void saveSctipt(const char* path); //����j

	std::string WriteRandomDisItem(const char* id); //������������������Ʒ ����� ����id
	

	std::string convertTrigger(Trigger* trigger);
	std::string convertAction(ActionNodePtr node, std::string& pre_actions, bool nested);

	std::string convertParameter(Parameter* parameter, ActionNodePtr node, std::string& pre_actions, bool add_call = false);
	std::string convertCall(ActionNodePtr node, std::string& pre_actions, bool add_call);
	
	std::string getBaseType(const std::string& type) const;
	std::string getBaseName(ActionNodePtr node);

	std::string generate_function_name(std::shared_ptr<std::string> trigger_name) const;


	//�������༭��ת����������Ϊ�Զ���ű���ʱ��
	bool onConvertTrigger(Trigger* trigger);

private:
	void writeCategoriy(BinaryWriter& writer);
	void writeVariable(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer);
	void writeTrigger(BinaryWriter& writer,Trigger* trigger);
	void writeAction(BinaryWriter& writer, Action* action);
	void writeParameter(BinaryWriter& writer, Parameter* param);

protected:
	TriggerConfigData* m_configData;
	TriggerData* m_editorData;

	class YDTrigger* m_ydweTrigger;

	uint32_t m_version;

	const std::string seperator = "//===========================================================================\n";

	//��������Ĭ�ϵ�ֵ
	std::unordered_map<std::string, TriggerType*> m_typesTable;

	std::unordered_map<std::string, bool> m_initFuncTable;

	std::unordered_map<Trigger*, bool> m_initTriggerTable;

public:

	std::map<std::string, Variable*> variableTable;

	std::string spaces[200];
	int space_stack;
};
extern TriggerEditor g_trigger_editor;
TriggerEditor& get_trigger_editor();
//������Ʒ���͵�jass������
std::string randomItemTypes[];