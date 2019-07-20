#include "stdafx.h"
#include "EditorData.h"
#include "ActionNode.h"

#include "script_handler.hpp"
#include "script_parser.hpp"
#include "script_group.hpp"

namespace script {
	typedef std::shared_ptr<std::map<std::string, std::string>> FuncTablePtr;

	struct ScriptInfo {
		//Script脚本的内存信息
		HandlerPtr script;

		//整个动作模板 对象
		ActionDefPtr action_def;

		//如果当前动作 是 动作组 则 group 是动作组中的信息 如果是 动作或者值的话 group是所在传参的动作组
		ActionInoListPtr group;

		//如果当前动作 是 动作组 则是0 否则是 动作或者值 时 group_id是 所在动作组中的子id 
		//比如动作在逆天触发器的事件中是 0 参数是 1 动作是2
		int group_id;


		//当前动作模板的函数名的表 实现func_name(1) 时记录名字
		FuncTablePtr func_name_table;

		//当前动作所在的节点
		ActionNodePtr node;

		//带传参的父节点
		ActionNodePtr parent;

		//如果是值 那么就会有个参数器指针
		Parameter* parameter = nullptr;

		//函数外输出脚本地址
		std::string* pre_actions;
		
	};
	
	class Converter {
	public:
		Converter();

		bool init();
		
		bool find_script(ActionNodePtr node, const std::string& name, ScriptInfo& script_info);
		
		bool execute(ScriptInfo& info, Value& result);

		bool call_func(CallPtr call, ScriptInfo& info, Value& result);

		bool call_bind(BindPtr bind, ScriptInfo& info, Value& result);
		
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

