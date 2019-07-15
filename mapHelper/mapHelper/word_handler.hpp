#pragma once

#include <vector>
#include <string>
#include <map>

namespace word {
	
	typedef std::shared_ptr<struct Call> CallPtr;

	struct Param {
		enum TYPE {
			UINT,
			STRING,
			CALL,
		};
		TYPE type;
		uint32_t uint;
		std::string string;

		CallPtr call;
	};
	struct Call {
		std::string name;
		std::vector<Param> params;
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
			accept_value(value);
		}

	};
}