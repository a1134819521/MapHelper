#pragma once
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "script_handler.hpp"

namespace script {
	enum FLAG : char {
		//value
		PACK_L = '{',
		PACK_R = '}',
		PL = '(',
		PR = ')',
		SPLIT = ',',
		STR = '"',

		//line
		FUNCTION = '#',
	};

	enum class ctype : uint8_t {
		none = 0,
		digit = 1,
		alpha = 2,
		underscode = 4,
		hexdigit = digit | alpha,
	};

	static ctype ctypemap[256] = {
		/*0x00*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x10*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x20*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x30*/ ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::digit, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x40*/ ctype::none, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
		/*0x50*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::underscode,
		/*0x60*/ ctype::none, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::hexdigit, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha,
		/*0x70*/ ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::alpha, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x80*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0x90*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xA0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xB0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xC0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xD0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xE0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
		/*0xF0*/ ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none, ctype::none,
	};

	inline void expect(const char*& p, const char c) {
		assert(*p == c);
		p++;
	}

	inline bool is_flag(const char* c) {
		switch (*c)
		{
		case FLAG::PACK_L:
		case FLAG::PACK_R:
		case FLAG::PL:
		case FLAG::PR:
		case FLAG::SPLIT:
		case FLAG::STR:
		case FLAG::FUNCTION:
			return true;
		}
		return false;
	}

	inline bool is_digit(char c) {
		return ctypemap[(unsigned char)c] == ctype::digit;
	}

	inline bool is_hexdigit(char c) {
		return (uint8_t)ctypemap[(unsigned char)c] & (uint8_t)ctype::digit;
	}
	inline bool is_alnum(char c) {
		uint8_t d = (uint8_t)ctypemap[(unsigned char)c];
		return d & (uint8_t)ctype::digit || d & (uint8_t)ctype::alpha;
	}
	inline bool is_alpha(char c) {
		return (uint8_t)ctypemap[(unsigned char)c] & (uint8_t)ctype::alpha;
	}
	inline bool equal(const char* p, const char c) {
		return *p == c;
	}

	inline bool equal(const char* p, const char c[]) {
		return *p == c[0] || *p == c[1];
	}


	inline bool consume(const char*& p, const char c) {
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	inline bool consume(const char*& p, const char c[]) {
		if (equal(p, c)) {
			p++;
			return true;
		}
		return false;
	}

	struct Parser {
		bool is_pack = false;
		const char* z;

		Parser(const char* input) : z(input) {
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}
		int reset(const char* input) {
			z = input;
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}

		//不收集空格跟换行信息
		void parse_ignore_whitespace() {
			while (equal(z, " \t") || equal(z, "\n\r")) {
				z++;
			}
		}

		void parse_whitespace(Handler& h) {
			std::string space;
			while (1) {
				if (equal(z, " \t")) {
					space += *z;
				} else {
					if (!space.empty()) {
						h.accept_code(space);
						space.clear();
					}

					if (equal(z, "\n\r")) {
						std::string newline;
						newline += *z;
						h.accept_code(newline);
					} else {
						return;
					}
				}
				z++;
			}
		}

		bool parse_call(Handler& h,CallPtr& call) {
			parse_ignore_whitespace();
			std::string name;
			const char* p = z;
			if (!parse_name(h, name)) {
				return false;
			}
			parse_ignore_whitespace();
			if (!equal(z, FLAG::PL)) {
				z = p;
				return false;
			}
			if (call) {
				call->params.clear();
			} else {
				call = std::make_shared<struct Call>();
			}

			if (!parse_args(h, call->params)) {
				std::cout << name << "读取参数失败\n";
				return false;
			}
			call->name = name;
			call->name_id = hash_(name.c_str());
			return true;
		}
		bool parse_args(Handler& h, std::vector<Value>& params) {
			if (!consume(z, FLAG::PL)) {
				return false;
			}
			std::string name;
			while (!equal(z, '\0')) {
				parse_ignore_whitespace();
				Value param;
				if (parse_uint(h, param.uint)) {
					param.type = Value::TYPE::UINT;
					params.push_back(param);
				} else if (parse_call(h,param.call)) {
					param.type = Value::TYPE::CALL;
					params.push_back(param);
				} else if (parse_name(h,name)) {
					param.type = Value::TYPE::STRING;
					param.string = name;
					params.push_back(param);
				} else if (consume(z,FLAG::SPLIT)) {

				} else if (consume(z, FLAG::PR)) {
					break;
				} else if (equal(z,FLAG::PACK_R)) {
					break;
				}
			}
			return true;
		}


		bool parse_code(Handler& h) {
			std::string code;
			while (!equal(z, '\0') && !equal(z, "\r\n")) {
				if (!is_pack) {
					if (equal(z, FLAG::PACK_L)) {
						break;
					}
				} else if(is_flag(z)) {
					break;
				}
				code += *z;
				z++;
			}
			if (!code.empty()) {
				h.accept_code(code);
				return true;
			}
			return false;
		}

		bool parse_name(Handler& h,std::string& name) {
			if (!is_pack) {
				parse_whitespace(h);
			}
			name.clear();
			while (!equal(z, '\0') && (is_alnum(*z) || equal(z, '_'))) {
				name += *z;
				z++;
			}
			if (!name.empty()) {
				return true;
			}
			return false;
		}

		bool parse_string(Handler& h, std::string& result) {
			if (!consume(z, FLAG::STR)) {
				return false;
			}
			result.clear();

			while (!equal(z, '\0') && !equal(z, FLAG::STR)) {
				result += *z;
				z++;
			}
			if (!consume(z, FLAG::STR)) {
				std::cout << "缺少对称的双引号\n";
				return false;
			}
			return true;
		}
		bool parse_uint(Handler& h,uint32_t& result) {
			std::string number;
			const char* p = z;
			while (!equal(z, '\0') && is_digit(*z)) {
				number += *z;
				z++;
			}
			if (!number.empty()) {
				result = std::atoi(number.c_str());
				return true;
			}
			z = p;
			return false;
		}

		bool parse_function(Handler& h) {
			if (!consume(z, FLAG::FUNCTION)) {
				return false;
			}
			if (!parse_action(h)) {
				std::cout << "读取动作失败\n";
				return false;
			}
			h.accept_line_type(LineInfo::TYPE::FUNCTION);
			return true;
		}

		bool parse_action(Handler& h) {
			while (!equal(z, '\0') && !equal(z, "\r\n")) {
				if (!parse_code(h)) {
					if (equal(z, FLAG::PACK_L)) {
						if (!parse_package(h)) {
							std::cout << "读取包失败 " << "\n";
							return false;
						}
					}
				}
			}
			h.accept_line_type(LineInfo::TYPE::ACTION);
			return true;
		}

		bool parse_package(Handler& h) {
			if (!consume(z, FLAG::PACK_L)) {
				return false;
			}
			is_pack = true;
			CallPtr call = std::make_shared<struct Call>();
			if (!parse_call(h, call)) {
				std::cout << "读取call失败\n";
				return false;
			}
			if (!consume(z, FLAG::PACK_R)) {
				std::cout << "缺少 } 结束符\n";
				return false;
			}
			is_pack = false;
			h.accept_call(call);
			return true;
		}
		

		bool parse_exp(Handler& h) {
			h.accept_newline();
			parse_whitespace(h);
			if (equal(z,FLAG::FUNCTION)) {
				return parse_function(h);
			} else {
				return parse_action(h);
			}
			return true;
		}

		bool parse(Handler& h) {
			while (!equal(z, '\0')) {
				if (!parse_exp(h)) {
					
					return false;
				}
				parse_whitespace(h);
			}
			return true;
		}
	};
}
