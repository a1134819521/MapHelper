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

		current = "D:\\MapHelper_git";

		if (!group.load(current / "group.json")) {
			std::cout << "json读取失败\n";
			return false;
		}
		m_init = true;
		return true;
	}

	bool Converter::find_script(ActionNodePtr node, const std::string& name, ScriptInfo& info) {

		ActionDefPtr action_def = group.get_action_def(name);
		if (!action_def) {
			return false;
		}

		uint32_t group_id = 0;
		ActionInoListPtr group_ptr;
		HandlerPtr script;
		FuncTablePtr func_table = std::make_shared<std::map<std::string, std::string>>();
		
		auto def_type = action_def->get_type();

		//如果是动作组 则返回动作组的脚本模板
		if (def_type == ActionDef::TYPE::GROUP) {

			auto table = node->getVarTable();

			group_ptr = action_def->get_group();

			script = action_def->get_group_script();

			if (!script || !group_ptr) {
				return false;
			}
			ActionNodePtr branch = node->getBranchNode();
			ActionNodePtr parent = branch->getParentNode();


			info = { script, action_def ,group_ptr ,(int)branch->getActionId(), func_table,nullptr,parent };
			return true;

		} else {
			//否则是 动作或者值 则根据位置 来决定使用的脚本模板

			ActionNodePtr branch = node->getBranchNode();

			ActionNodePtr parent = branch->getParentNode();

			bool is_trigger = true;

			const char* parent_name = "default";

			std::function<void(ActionNodePtr)> find_parent = [&](ActionNodePtr ptr) {
				parent = ptr->getParentNode();
				if (!parent || parent->isRootNode()) {
					if (is_trigger) {
						parent_name = "trigger";
					} else {
						parent_name = "default";
					}
				} else {
					auto parent_def_ptr = group.get_action_def(*parent->getName());

					if (parent_def_ptr && (parent_def_ptr->have_func() || (parent_def_ptr->is_auto_param() && parent_def_ptr->is_group()) )) {
						//找到所在的逆天计时器动作组
						if (parent_def_ptr->is_auto_param()) {

							group_id = ptr->getActionId();
							group_ptr = parent_def_ptr->get_group();
							parent_name = parent->getName()->c_str();

							//如果在参数区 并且是 值类型 则再往上一层
							if (group_ptr && !(*group_ptr)[group_id].is_child && def_type == ActionDef::TYPE::VALUE) {
								find_parent(parent);
							}
						} else {
							//如果拥有函数 且不是自动传参的 则继续搜索上一层 并且标记非触发根部位置
							is_trigger = false;
							find_parent(parent);
						}
					} else {
						//如果不是带函数的动作组 则往上一层搜索
						find_parent(parent);
					}
				}
			};
			find_parent(branch);

			script = action_def->get_script(parent_name);

			info = { script, action_def ,group_ptr ,(int)group_id, func_table,nullptr,parent };
			return true;
		}

		return false;
	}

	

	bool Converter::execute(ScriptInfo& info, Value& result)
	{
		ActionNodePtr& node = info.node;
		std::string& pre_actions = *info.pre_actions;
		std::string& output = result.string;
		if (!info.script)
			return false;

		auto ydtrigger = YDTrigger::getInstance();

		std::string func;
		auto script = info.script;

		ActionDefPtr action_def = info.action_def;

		//如果该值 是自动传参的值 则判断条件后 执行传参脚本
		if (action_def->is_auto_param() && action_def->get_type() == ActionDef::TYPE::VALUE) {
			bool is_child = false;
			//bool is_root = !info.parent || info.parent->isRootNode();

			if (info.parent && info.group) {
				auto parent_def = (*info.group)[info.group_id];
				is_child = parent_def.is_child;
			}
			auto auto_script = action_def->get_auto_script();
			//当父节点是 根触发器 或者 是自动传参的子动作 则开始执行传参脚本
			if (is_child && auto_script) {
				info.script = auto_script;
				Value value;
				const auto& lines = auto_script->lines;
				for (size_t l = 0; l < lines.size(); l++) {
					const auto& line = lines[l];
					const auto& values = line.values;
					for (size_t p = 0; p < values.size(); p++) {
						const auto& item = values[p];
						if (item.type == ValueInfo::CALL) {
							call_func(item.call, info, value);
						}
					}
				}
			}
			
		}


		const auto& lines = script->lines;

		for (size_t l = 0; l < lines.size(); l++) {
			const auto& line = lines[l];

			const auto& values = line.values;

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
					Value ret;
					if (call_func(item.call, info, ret)) {
						strline += ret.string;
					}
				}

			}

			switch (line.type) 
			{
			case script::LineInfo::TYPE::FUNCTION: 
			{
				if (values.size() > 0) {
					const auto& value = values[0];
					const auto& code = *(values[0].code);
					if (value.type == script::ValueInfo::TYPE::CODE) {
						if (code.find("endfunction") != std::string::npos) {
							ydtrigger->onActionsToFuncEnd(func, node);
							func += strline;
							m_func_stack--;
							break;
						} else if (code.find("function") != std::string::npos) {
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

		
		pre_actions += func;
		return true;
	}


	bool Converter::call_func(CallPtr call,ScriptInfo& info, Value& result) {

		ActionNodePtr& node = info.node;
		std::string& pre_actions = *info.pre_actions;

		if (!call) {
			return false;
		}

		std::function <uint32_t(script::Value&)> param2uint = [&](script::Value& param) {
			if (param.type == script::Value::TYPE::CALL) {
				Value result;
				if (call_func(param.call, info, result)) {
					return result.uint;
				}
			}
			return param.uint;
		};

		std::function <std::string(script::Value&)> param2string = [&](script::Value& param) {
			if (param.type == script::Value::TYPE::CALL) {
				Value result;
				if (call_func(param.call, info, result)) {
					return result.string;
				}
			}
			return param.string;
		};

		std::function <BindPtr(script::Value&)> param2bind = [&](script::Value& param) {
			if (param.type == script::Value::TYPE::CALL) {
				Value result;
				if (call_func(param.call, info, result)) {
					return result.bind;
				}
			}
			return param.bind;
		};


		Action* action = node->getAction();

		Parameter** parammeters = action->parameters;

		auto& editor = get_trigger_editor();

		std::vector<script::Value>& params = call->params;

		ActionInoListPtr& group_ptr = info.group;

		int& group_id = info.group_id;

		auto ydtrigger = YDTrigger::getInstance();

		switch (call->name_id) {
			
			//加上双引号后返回
			case "to_string"s_hash: {
				result.string = "\"" + param2string(params[0]) + "\"";
				result.type = Value::STRING;
				return true;
			}

			//获取参数
			case "get"s_hash: {
				if (params.size() == 0)
					return false;

				uint32_t index = min(param2uint(params[0]), action->param_count);
				result.string = editor.convertParameter(parammeters[index], node, pre_actions);
				result.type = Value::STRING;

				return true;
			}

			//获取参数类型
			case "get_type"s_hash: {
				if (params.size() == 0)
					return false;

				uint32_t index = min(param2uint(params[0]), action->param_count);
				result.string = editor.convertParameter(parammeters[index], node, pre_actions);
				result.type = Value::STRING;

				return true;;
			}
		
			//取动作组中配置好的值
			case "get_value"s_hash: {
				if (params.size() == 0)
					return false;
			
				if (group_ptr) {
					const std::string& key = param2string(params[0]);
					auto& info = (*group_ptr)[max(0,group_id)];
					std::string value;
					if (group_id < group_ptr->size() && info.get_value(key, value)) {
						result.string = value;
						result.type = Value::STRING;

						return true;
					}
				}
				return false;
			}

			//生成函数名
			case "func_name"s_hash: {
				if (params.size() == 0)
					return false;

				//生成 或取一个函数名
				std::string func_index = std::to_string(param2uint(params[0]));
				auto it = info.func_name_table->find(func_index);
				if (it == info.func_name_table->end()) {
					std::string name = editor.generate_function_name(node->getTriggerNamePtr());
					info.func_name_table->emplace(func_index, name);
					result.string = name;
				} else {
					result.string = it->second;
				}
				result.type = Value::STRING;
				return true;
			}

			//转换参数值为循环遍历名
			case "loop_name"s_hash: {
				if (params.size() == 0)
					return false;
				//如果是取参数转换为循环变量名的话
				uint32_t index = param2uint(params[0]);
				if (index == -1) break;
				index = min(index, action->param_count);
				std::string name = "ydul_" + editor.convertParameter(parammeters[index], node, pre_actions);
				convert_loop_var_name(name);

				result.string = name;
				result.type = Value::STRING;

				return true;
			}

			//获取返回类型
			case "get_return_type"s_hash: {
				if (!info.parameter)
					return false;
				result.string = info.parameter->type_name;
				result.type = Value::STRING;

				return true;
			}

			//注册局部变量
			case "add_local"s_hash: {
				if (params.size() < 2)
					return false;
				localTable.emplace(params[0].string, params[1].string);

				return true;
			}

			//获取逆天类型值 原值+11
			case "get_ydtype"s_hash: {
				if (params.size() == 0)
					return false;
				uint32_t index = min(param2uint(params[0]), (int)action->param_count);

				//typename_01_integer + 11 = integer
				result.string = std::string(parammeters[index]->value + 11);
				result.type = Value::STRING;

				return true;
			}

			//解包指定子id的动作
			case "get_group"s_hash: {
				if (params.size() == 0)
					return false;

				//只有动作组可以解包
				if (!info.action_def->is_group())
					return false;

				uint32_t index = param2uint(params[0]);
				if (index >= group_ptr->size())
					return false;

				auto& action_info = (*group_ptr)[index];

				std::vector<ActionNodePtr> list;
				node->getChildNodeList(list);

				bool firstBoolexper = true;
				int s = 0;

				space_stack++;
				if (m_func_stack > 0) {
					s = space_stack;
					space_stack = 1;
				}

				//
				if (info.action_def->is_auto_param()) {
					auto last_table = node->getLastVarTable();
				}
			
				std::string str;

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
							str += spaces[space_stack];
						else
							str += spaces[max(1,space_stack - 1)];
					}

					//事件需要默认一个参数
					if (type == Action::Type::event) {
						if (child->getNameId() == "MapInitializationEvent"s_hash) {
							continue;
						}
						ydtrigger->onRegisterEvent(str, child);

						str += "call " + editor.getBaseName(child) + "(";

						std::string value;
						auto& info = (*group_ptr)[index];
						if (!info.get_value("handle", value)) {
							value = "null";
						}
						str += value;

						for (size_t k = 0; k < childAction->param_count; k++) {
							str += ", ";
							str += editor.convertParameter(childAction->parameters[k], child, pre_actions);
						}
						str += ")\n";
						ydtrigger->onRegisterEvent2(str, child);

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
						str += value;
						str += "(" + editor.convertAction(child, pre_actions, true) + ")";
					} else {
						str += editor.convertAction(child, pre_actions, false);
						if (str.size() > 0 && str[str.size() - 1] != '\n') {
							str += "\n";
						}
					}
				}
				if (action_info.type_id == Action::Type::condition && firstBoolexper) {
					str += "true";
				}
				
				if (s > 0) {
					space_stack = s;
				}
				space_stack--;

				result.string = str;
				result.type = Value::STRING;
				return true;
			}
			 
			//bind(name, args...) 参数捆绑 将动作名 跟参数打包成对象",
			case "bind"s_hash: {
				if (params.size() == 0)
					return false;
				if (params[0].type != Value::TYPE::STRING)
					return false;
				
				result.bind = std::make_shared<struct Bind>();
				result.type = Value::TYPE::BIND;
				result.bind->name = param2string(params[0]);
				for (size_t i = 1; i < params.size(); i++) {
					Value& param = params[i];
					if (param.type == Value::TYPE::CALL) {
						Value value;
						if (call_func(param.call, info, value)) {
							result.bind->params.push_back(value);
						}
					}
					else {
						result.bind->params.push_back(param);
					}
				}
				return true;
			}

			case "map_set"s_hash: {
				if (params.size() != 2)
					return false;

				std::string key = param2string(params[0]);

				auto last_table = node->getLastVarTable();


				BindPtr bind = param2bind(params[1]);
				if (bind) {
					Value value;
					value.type = Value::TYPE::BIND;
					value.bind = bind;
					last_table->emplace(key, value);
				}
			
				return true;
			}

			case "map_each"s_hash: {
				if (params.size() == 0)
					return false;

				uint32_t index = param2uint(params[0]);

				auto last_table = node->getLastVarTable();
				auto table = node->getVarTable();
				std::string& str = result.string;
				info.group_id = index;
				for (auto&[k, value] : *table) {
					str += spaces[space_stack];

					if (value.type == Value::TYPE::BIND) {
						Value ret;
						if (call_bind(value.bind, info, ret)) {
							str += ret.string;
						}
					} else if (value.type == Value::TYPE::STRING) {
						str += value.string;
					} else if (value.type == Value::TYPE::UINT) {
						str += std::to_string(value.uint);
					} else if (value.type == Value::TYPE::CALL) {
						str += param2string(value);
					}
					str += "\n";
					last_table->emplace(k, value);
				}
				if (!str.empty()) {
					str.pop_back();
				}
				result.type = Value::STRING;
				return true;
			}
		}
		return false;
	}


	bool Converter::call_bind(BindPtr bind,ScriptInfo& info, Value& result) {
		if (!bind) {
			return false;
		}

		auto action_def = group.get_action_def(bind->name);
		if (!action_def) {
			return false;
		}
		auto type = action_def->get_type();
		if (type == ActionDef::TYPE::GROUP) {
			return false;
		}
		
		ScriptInfo* script_info = &info;
		std::shared_ptr<std::string> name_ptr;
		const char* parent_name;

		if (type == ActionDef::TYPE::VALUE) {

			ScriptInfo* script_info = &ScriptInfo(info);
			int group_id = 0;
			ActionInoListPtr group_ptr;
			
			ActionNodePtr branch = info.node->getBranchNode();

			ActionNodePtr parent = branch->getParentNode();

			bool is_trigger = true;
			parent_name = "default";
	
			std::function<void(ActionNodePtr)> find_parent = [&](ActionNodePtr ptr) {
				parent = ptr->getParentNode();
				if (!parent || parent->isRootNode()) {
					if (is_trigger) {
						parent_name = "trigger";
					}
					else {
						parent_name = "default";
					}
				}
				else {
					auto parent_def_ptr = group.get_action_def(*parent->getName());

					if (parent_def_ptr && (parent_def_ptr->have_func() || (parent_def_ptr->is_auto_param() && parent_def_ptr->is_group()))) {
						//找到所在的逆天计时器动作组
						if (parent_def_ptr->is_auto_param()) {

							group_id = (int)ptr->getActionId();
							group_ptr = parent_def_ptr->get_group();
							parent_name = parent->getName()->c_str();
							info.group = group_ptr;
							info.group_id = group_id;

							//如果在参数区 并且是 值类型 则再往上一层
							if (group_ptr && !(*group_ptr)[group_id].is_child && type == ActionDef::TYPE::VALUE) {
								find_parent(parent);
							}
						}
						else {
							//如果拥有函数 且不是自动传参的 则继续搜索上一层 并且标记非触发根部位置
							is_trigger = false;
							find_parent(parent);
						}
					}
					else {
						//如果不是带函数的动作组 则往上一层搜索
						find_parent(parent);
					}
				}
			};
			find_parent(branch);

			name_ptr = std::make_shared<std::string>(parent_name);
		
		} else {
			name_ptr = info.node->getName();
		}
		
		
		auto script = action_def->get_script(*name_ptr);
		if (!script) {
			return false;
		}

		const auto& lines = script->lines;

		std::string& output = result.string;

		const auto& params = bind->params;
		uint32_t count = 0;

		for (size_t i = 0; i < lines.size(); i++) {
			const auto& line = lines[i];
			const auto& values = line.values;

			for (size_t n = 0; n < values.size(); n++) {
				const auto& value = values[n];
				if (value.type == ValueInfo::CODE) {
					output += *value.code;
				} else {
					if (value.type == ValueInfo::TYPE::CALL) {
						if (value.call->name_id == "get_value"s_hash) {
							Value ret;
							if (call_func(value.call, *script_info, ret)) {
								output += ret.string;
								continue;;
							}
						}
					}
					if (count < params.size()) {
						const Value& value = params[count];
						if (value.type == Value::TYPE::BIND) {
							Value ret;
							if (call_bind(value.bind, *script_info, ret)) {
								output += ret.string;
							}
						} else {
							output += value.string;
						}
						count++;
					}
				}
			}
		}
		result.type = Value::TYPE::STRING;
		return true;
	}

	Converter& get_converter()
	{
		return base::singleton<Converter>::instance();
	}
}


