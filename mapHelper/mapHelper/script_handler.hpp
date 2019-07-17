#pragma once

#include <vector>
#include <string>
#include <map>

namespace script {
	
	typedef std::shared_ptr<struct Call> CallPtr;
	typedef std::shared_ptr<struct Bind> BindPtr;
	struct Value {
		enum TYPE {
			NONE,
			UINT,
			STRING,
			CALL,
			BIND
		};
		TYPE type = NONE;
		uint32_t uint;
		std::string string;
		CallPtr call;
		BindPtr bind;
	};
	struct Call {
		uint32_t name_id;
		std::string name;
		std::vector<Value> params;
	};

	struct ValueInfo {
		enum TYPE {
			CALL,
			CODE
		};
		TYPE type;
		std::shared_ptr<std::string> code;
		CallPtr call;
	};

	struct LineInfo {
		enum TYPE {
			FUNCTION,
			ACTION
		};
		TYPE type; 
		std::vector<ValueInfo> values;
	};


	struct Bind {
		std::string name;
		std::vector<Value> params;
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
			lines[pos].type = LineInfo::TYPE::ACTION;
		}

		void accept_line_type(LineInfo::TYPE type) {
			lines[pos].type = type;
		}

		void accept_value(const ValueInfo& info) {
			lines[pos].values.push_back(info);
		}


		void accept_code(const std::string& str) {
			ValueInfo value;
			value.type = ValueInfo::CODE;
			value.code = std::make_shared<std::string>(str);
			accept_value(value);
		}

		void accept_call(CallPtr call) {
			ValueInfo value;
			value.type = ValueInfo::CALL;
			value.call = call;
			if (call) {
				call->name_id = hash_(call->name.c_str());
			}
			accept_value(value);
		}

	};
}