#include "stdafx.h"
#include "script.h"
#include "TriggerEditor.h"
#include "YDTrigger.h"
#include "singleton.h"

namespace script {
	Converter::Converter()
		:m_init(0),
		space_stack(get_trigger_editor().space_stack),
		spaces(get_trigger_editor().spaces)
	{ }

	bool Converter::init() {
		char buffer[0x400];
		HMODULE h = GetModuleHandleA("MapHelper.asi");
		if (h == NULL) {
			h = GetModuleHandleA("MapHelper.dll");
		}
		GetModuleFileNameA(h, buffer, 0x400);

		fs::path current = buffer;
		current.remove_filename();

		current = "E:\\MapHelper_git";

		if (!group.load(current / "group.json")) {
			std::cout << "json读取失败\n";
			return false;
		}
		m_init = true;
		return true;
	}

	bool Converter::find_script(ActionNodePtr node, const std::string& name, ScriptInfo& script_info) {

		ActionDefPtr action_def = group.get_action_def(name);
		if (!action_def) {
			return false;
		}

		uint32_t group_id = 0;
		ActionInoListPtr group_ptr;
		HandlerPtr script;
		
		auto def_type = action_def->get_type();

		//如果是动作组 则返回动作组的脚本模板
		if (def_type == ActionDef::TYPE::GROUP) {

			group_ptr = action_def->get_group();

			script = action_def->get_group_script();

			if (!script || !group_ptr) {
				return false;
			}

			script_info = { action_def,script ,group_ptr , group_id };
			return true;

		} else {
			//否则是 动作或者值 则根据位置 来决定使用的脚本模板

			ActionNodePtr branch = node->getBranchNode();
			ActionNodePtr parent = branch->getParentNode();

			const char* parent_name = "default";

			if (action_def->is_auto_param()) {
				if (!parent || parent->isRootNode()) {//如果是在触发中
					parent_name = "trigger";
				}
				else {
					ActionNodePtr ptr = branch;

					//否则搜素上一层带传参的父节点
					while (!ptr->isRootNode()) {
						ActionNodePtr parent_ptr = ptr->getParentNode();
						auto parent_def_ptr = group.get_action_def(*parent_ptr->getName());
						if (parent_def_ptr && parent_def_ptr->is_auto_param()) {
							group_id = ptr->getActionId();
							group_ptr = parent_def_ptr->get_group();
							parent_name = parent_ptr->getName()->c_str();
							break;
						}
						ptr = parent_ptr;
					}
				}
			}
			script = action_def->get_script(parent_name);

			script_info = { action_def, script, group_ptr, group_id };
			return true;
		}

		return false;
	}

	bool Converter::execute(ActionNodePtr node, std::string& output, std::string& pre_actions, ScriptInfo& info)
	{

		if (!info.script)
			return false;

		auto ydtrigger = YDTrigger::getInstance();

		std::string func;

		auto& lines = info.script->lines;

		for (size_t l = 0; l < lines.size(); l++) {
			const auto& line = lines[l];

			auto& values = line.values;

			std::string strline;

			for (size_t p = 0; p < values.size(); p++) {
				const auto& item = values[p];

				if (item.type == script::ValueInfo::TYPE::CODE) {
					//除了第一行之外，在每行开头处增加缩进
					if (!output.empty() && p == 0) {
						strline += spaces[space_stack];
					}
					const auto& code = *item.code;
					//处理多余换行
					if (code[0] == '\n') {
						if (strline.size() > 0) {
							if (strline[strline.size() - 1] != '\n')
								strline += code;
							continue;
						} else {
							continue;
						}
					}
					strline += code;
				} else {
					call_func(item.call, info, node,strline, pre_actions);
				}

			}

			switch (line.type)
			{
			case script::LineInfo::TYPE::FUNCTION:
			{
				if (values.size() > 0)
				{
					const auto& value = values[0];
					const auto& code = *(values[0].code);
					if (value.type == script::ValueInfo::TYPE::CODE)
					{
						if (code.find("endfunction") != std::string::npos)
						{
							ydtrigger->onActionsToFuncEnd(func, node);
							func += strline;
							m_func_stack--;
							break;
						}
						else if (code.find("function") != std::string::npos)
						{
							func += strline;
							ydtrigger->onActionsToFuncBegin(func, node);
							m_func_stack++;
							break;
						}
					}
				}

				func += strline;
				break;
			}
			case script::LineInfo::TYPE::ACTION:
				output += strline;
			}
		}
		m_func_name.clear();

	
		pre_actions += func;
		return true;
	}


	bool Converter::call_func(CallPtr call,ScriptInfo& info,ActionNodePtr node, std::string& output, std::string& pre_actions) {

		if (!call) {
			return false;
		}

		std::function <uint32_t(script::Param&)> param2uint = [&](script::Param& param) {
			if (param.type == script::Param::TYPE::CALL) {
				std::string result;
				if (call_func(param.call, info, node, result, pre_actions)) {
					return (uint32_t)std::stoi(result);
				}
			}
			return param.uint;
		};

		std::function <std::string(script::Param&)> param2string = [&](script::Param& param) {
			if (param.type == script::Param::TYPE::CALL) {
				std::string result;
				if (call_func(param.call, info, node, result, pre_actions)) {
					return result;
				}
			}
			return param.string;
		};

		Action* action = node->getAction();

		Parameter** parammeters = action->parameters;

		auto editor = get_trigger_editor();

		std::vector<script::Param>& params = call->params;

		ActionInoListPtr& group_ptr = info.group;

		uint32_t& group_id = info.group_id;

		auto ydtrigger = YDTrigger::getInstance();

		switch (call->name_id) 
		{
		case "get"s_hash: //获取参数
		{
			if (params.size() == 0)
				return false;

			uint32_t index = min(param2uint(params[0]), action->param_count);
			output+=editor.convertParameter(parammeters[index], node, pre_actions);
			return true;
		}
		case "get_type"s_hash: //获取参数类型
		{
			if (params.size() == 0)
				return false;

			uint32_t index = min(param2uint(params[0]), action->param_count);
			output +=editor.convertParameter(parammeters[index], node, pre_actions);
			return true;;
		}

		case "get_value"s_hash: //取动作组中配置好的值
		{
			if (params.size() == 0)
				return false;

			if (group_ptr) {
				const std::string& key = param2string(params[0]);
				auto& info = (*group_ptr)[group_id];
				std::string value;
				if (group_id < group_ptr->size() && info.get_value(key, value)) {
					output += value;
					return true;
				}
			}
			return false;
		}
		case "func_name"s_hash: //生成函数名
		{
			if (params.size() == 0)
				return false;

			//生成 或取一个函数名
			std::string func_index = std::to_string(param2uint(params[0]));
			auto it = m_func_name.find(func_index);
			if (it == m_func_name.end()) {
				std::string name = editor.generate_function_name(node->getTriggerNamePtr());
				m_func_name[func_index] = name;
				output += name;
				return true;
			} else {
				output += it->second;
				return true;
			}
			return false;
		}
		case "loop_name"s_hash: //转换参数值为循环遍历名
		{
			if (params.size() == 0)
				return false;
			//如果是取参数转换为循环变量名的话
			uint32_t index = param2uint(params[0]);
			if (index == -1) break;
			index = min(index, action->param_count);
			std::string name = "ydul_" + editor.convertParameter(parammeters[index], node, pre_actions);
			convert_loop_var_name(name);
			output +=name;
			return false;
		}
		case "get_return_type"s_hash: //获取返回类型
		{
			return false;
		}
		case "add_local"s_hash: //注册局部变量
		{
			if (params.size() < 2)
				return false;
			localTable.emplace(params[0].string, params[1].string);
			return true;;
		}
		case "get_ydtype"s_hash: //获取逆天类型值 原值+11
		{
			if (params.size() == 0)
				return false;
			uint32_t index = min(param2uint(params[0]), (int)action->param_count);
			output +=std::string(parammeters[index]->value + 11); //typename_01_integer + 11 = integer
			return true;
		}

		case "get_group"s_hash: //解包指定子id的动作
		{
			if (params.size() == 0)
				return false;

			//只有动作组可以解包
			if (!info.action_def->is_group())
				return false;

			int s = 0;

			uint32_t index = param2uint(params[0]);
			if (index >= group_ptr->size())
				return false;

			bool firstBoolexper = true;
			auto& action_info = (*group_ptr)[index];

			std::vector<ActionNodePtr> list;

			node->getChildNodeList(list);

			space_stack++;
			if (m_func_stack > 0) {
				s = space_stack;
				space_stack = 1;
			}
			std::string result;
			for (size_t i = 0; i < list.size(); i++) {
				auto& child = list[i];
				Action* childAction = child->getAction();

				uint32_t child_id = child->getActionId();

				auto type = child->getActionType();

				//生成动作
				if (index != child_id || child_id >= group_ptr->size()) {
					continue;
				}
				if (type != Action::Type::condition) {
					if (action_info.is_child)
						result += spaces[space_stack];
					else
						result += spaces[space_stack - 1];
				}

				//事件需要默认一个参数
				if (type == Action::Type::event) {
					if (child->getNameId() == "MapInitializationEvent"s_hash) {
						continue;
					}
					ydtrigger->onRegisterEvent(result, child);

					result += "call " + editor.getBaseName(child) + "(";

					std::string value;
					auto& info = (*group_ptr)[index];
					if (!info.get_value("handle", value)) {
						value = "null";
					}
					result += value;

					for (size_t k = 0; k < childAction->param_count; k++) {
						result += ", ";
						result += editor.convertParameter(childAction->parameters[k], child, pre_actions);
					}
					result += ")\n";
					ydtrigger->onRegisterEvent2(result, child);

				} else if (type == Action::Type::condition) {
					std::string value;

					if (firstBoolexper) {
						firstBoolexper = false;
					} else {
						if (!action_info.get_value("Compare", value)) {
							value = "and";
						}
						value = " " + value + " ";
					}
					result += value;
					result += "(" + editor.convertAction(child, pre_actions, true) + ")";
				} else {
					result += editor.convertAction(child, pre_actions, false);
					if (result.size() > 0 && result[result.size() - 1] != '\n') {
						result += "\n";
					}
				}
			}
			if (action_info.type_id == Action::Type::condition && firstBoolexper) {
				result += "true";
			}

			if (s > 0) {
				space_stack = s;
			}
			space_stack--;

			//如果是自动传参的动作组 并且解包的是 参数代码
			if (info.action_def->is_auto_param() && !action_info.is_child) {

			}
			output +=result;
			return true;
		}
		}

		return false;
	}

	Converter& get_converter()
	{
		return base::singleton<Converter>::instance();
	}
}


