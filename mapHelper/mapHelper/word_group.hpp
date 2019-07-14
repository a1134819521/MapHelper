#pragma once
#include "stdafx.h"

#include "json.hpp"
#include "word_handler.hpp"
#include "word_parser.hpp"


namespace word {

	using json::Json;

	struct ActionInfo
	{
		int type_id;

		std::string name;

		bool is_child = false;


		std::map<std::string, std::string> attrs;

		bool equal(const std::string& key, const std::string& value) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return false;
			}
			return it->second.compare(value) == 0;
		}

		std::string get_value(const std::string& key) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return std::string();
			}
			return it->second;
		}

		bool get_value(const std::string& key, std::string& result) {
			auto it = attrs.find(key);
			if (it == attrs.end()) {
				return false;
			}
			result = it->second;
			return true;
		}
	};

	struct ActionDef
	{
		bool is_auto_param = false;

		bool is_group = true;

		std::vector<ActionInfo> actions;

		//脚本模板语法树
		word::Handler script_handler;
	};

	class ActionGroup {

	public:
		typedef std::map<std::string, ActionDef> GroupDef;
		ActionGroup()
		{ }

		bool load(const fs::path& path)
		{
			std::ifstream file(path, std::ios::binary);
			if (!file.is_open()) {
				std::cout << path << " 文件不存在\n";
				return false;
			}
			std::stringstream stream;
			std::string text;
			std::string error;

			stream << file.rdbuf();

			text = stream.str();

			file.close();

			return load(text);
		}

		bool load(const std::string& text) {
			std::string error;
			Json json = Json::parse(text, error);

			if (!error.empty()) {
				std::cout << "json 读取错误\n" << error;
				return false;
			}
			return load(json);
		}

		bool load(const Json& json) {
			auto defs = json.object_items();
			if (defs.empty()) {
				std::cout << "json是空的\n";
				return 0;
			}

			for (const auto&[def_name, child] : defs) {
				Json jactions = child["Actions"];
				Json jscript = child["Script"];

				if (!jactions.is_array() || !jscript.is_array()) {
					continue;
				}

				ActionDef action_def;

				Json jbool = child["AutoParam"];
				if (jbool.is_bool()) {
					action_def.is_auto_param = jbool.bool_value();
				}
				for (const auto& action : jactions.array_items()) {
					Json jtype = action["Action"];
					int type_id = -1;

					if (jtype.is_string()) {
						type_id = 2;
					}
					else if (jtype = action["Condition"], jtype.is_string()) {
						type_id = 1;
					}
					else if (jtype = action["Event"], jtype.is_string()) {
						type_id = 0;
					}
					if (type_id < 0) {
						continue;
					}
					ActionInfo info;
					info.type_id = type_id;
					info.name = jtype.string_value();
					Json jchild = action["IsChild"];
					if (jchild.is_bool()) {
						info.is_child = jchild.bool_value();
					}

					for (const auto&[key, value] : action.object_items()) {
						if (value.is_string()) {
							info.attrs[key] = value.string_value();
						}
					}
					action_def.actions.push_back(info);
				}

				std::string script;
				auto array = jscript.array_items();

				for (size_t i = 0; i < array.size(); i++) {
					const auto& line = array[i];
					script += line.string_value();

					if (i + 1 < array.size()) {
						script += "\n";
					}
				}

				if (!script.empty()) {
					word::Handler& handler = action_def.script_handler;
					word::Parser parser(script.c_str());
					parser.parse(handler);

				}

				m_group_def.emplace(def_name, action_def);
			}
			std::cout << "读取成功\n";
			return true;
		}

		GroupDef& get_group_def() {
			return m_group_def;
		}


		ActionDef get_action_def(const std::string& name) {
			auto it = m_group_def.find(name);
			if (it == m_group_def.end()) {
				return ActionDef();
			}
			return it->second;
		}

	private:
		GroupDef m_group_def;
	};
}