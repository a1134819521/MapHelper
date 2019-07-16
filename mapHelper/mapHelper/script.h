#include "stdafx.h"
#include "EditorData.h"
#include "ActionNode.h"

#include "script_handler.hpp"
#include "script_parser.hpp"
#include "script_group.hpp"

namespace script {
	typedef std::shared_ptr<std::map<std::string, std::string>> FuncTablePtr;

	struct ScriptInfo {
		HandlerPtr script;
		ActionDefPtr action_def;
		ActionInoListPtr group;
		FuncTablePtr func_name_table;
		uint32_t group_id;
	};
	
	class Converter {
	public:
		Converter();

		bool init();
		
		bool find_script(ActionNodePtr node, const std::string& name, ScriptInfo& script_info);
		
		bool execute(ActionNodePtr node,std::string& output, std::string& pre_actions, ScriptInfo& info);

		bool call_func(CallPtr call, ScriptInfo& info, ActionNodePtr node,std::string& output, std::string& pre_actions);

		
	private:
		
		bool m_init;

		int m_func_stack;

	public:
		ActionGroup group;

		int& space_stack;
		std::string(&spaces)[200];

		std::map<std::string, std::string> localTable;
	};

	Converter& get_converter();
}

