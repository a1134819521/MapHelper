#include "stdafx.h"
#include "EditorData.h"
#include "ActionNode.h"

#include "script_handler.hpp"
#include "script_parser.hpp"
#include "script_group.hpp"

namespace script {

	struct ScriptInfo {
		ActionDefPtr action_def;
		HandlerPtr script;
		ActionInoListPtr group;
		uint32_t group_id;
	};
	
	class Converter {
	public:
		Converter();

		bool init();
		
		bool find_script(ActionNodePtr node, const std::string& name, ScriptInfo& script_info);
		
		std::string execute(ActionNodePtr node, std::string& pre_actions, ScriptInfo& info);

		std::string call_func(CallPtr call, ScriptInfo& info, ActionNodePtr node, std::string& pre_actions);

		ActionGroup& get_group();
		
	private:
		
		bool m_init;

		int m_func_stack;

		std::map<std::string, std::string> m_func_name;

	public:
		ActionGroup group;

		int& space_stack;
		std::string(&spaces)[200];

		std::map<std::string, std::string> localTable;
	};

	Converter& get_converter();
}

