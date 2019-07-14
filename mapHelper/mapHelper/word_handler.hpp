#pragma once

#include <vector>
#include <string>
#include <map>

namespace word {

	struct ValueInfo {
		enum Type {
			args,
			args_type,
			args_type2,
			param,
			group,
			code
		};
		Type type;
		std::string data;
		int args_index = -1;
	};

	struct LineInfo {
		enum Type {
			function,
			local,
			action
		};
		Type type; 
		std::vector<ValueInfo> values;
	};

	struct Handler {
		std::vector<LineInfo> lines;
		int pos = -1;
		Handler() {
		}
		void accept_newline() {
			if (++pos == lines.size()) {
				lines.resize(lines.size() + 1);
			}
			lines[pos].type = LineInfo::Type::action;
		}

		void accept_line_type(LineInfo::Type type) {
			lines[pos].type = type;
		}

		void accept_value(const ValueInfo& info) {
			lines[pos].values.push_back(info);
		}

		void accept_value(ValueInfo::Type type, int index) {
			accept_value({ type,std::string(),index });
		}

		void accept_value(ValueInfo::Type type, const std::string& key) {
			accept_value({ type,key,-1 });
		}

		void accept_args(int index) {
			accept_value(ValueInfo::Type::args, index);
		}

		void accept_args_type(int index) {
			accept_value(ValueInfo::Type::args_type, index);
		}

		void accept_param(const std::string& key,int index) {
			accept_value({ ValueInfo::Type::param,key,index });
		}
		
		void accept_args_type2(int index) {
			accept_value(ValueInfo::Type::args_type2, index);
		}

		void accept_group(int index) {
			accept_value(ValueInfo::Type::group, index);
		}

		void accept_code(const std::string& str) {
			accept_value(ValueInfo::Type::code, str);
		}
	};
}