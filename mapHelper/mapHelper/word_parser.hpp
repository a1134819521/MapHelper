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

#include "word_handler.hpp"

namespace word {
	enum cflag : char {
		//value
		args = '$',
		args_type = '~',
		args_type2 = '^',
		group = '%',

		//line
		local = '@',
		function = '#',
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
		case cflag::args:
		case cflag::args_type:
		case cflag::args_type2:
		case cflag::group:
		case cflag::function:
		case cflag::local:
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

		bool parse_args(Handler& h) {
			expect(z, cflag::args);
			consume(z, cflag::args);
			const char* p = z;
			int index;
			if (parse_index(h,index)) {
				h.accept_args(index);
				return true;
			}
			std::string name;
			z = p;
			if (parse_name(h,name)) {
				index = -1;
				parse_index(h, index);
				h.accept_param(name,index);
				return true;
			}
			return false;
		}

		bool parse_args_type(Handler& h) {
			expect(z, cflag::args_type);
			consume(z, cflag::args_type);
			int index;
			if (parse_index(h,index)) {
				h.accept_args_type(index);
				return true;
			}
			return false;
		}



		bool parse_args_type2(Handler& h) {
			expect(z, cflag::args_type2);
			consume(z, cflag::args_type2);
			int index;
			if (parse_index(h,index)) {
				h.accept_args_type2(index);
				return true;
			}
			return false;
		}

		bool parse_group(Handler& h) {
			expect(z, cflag::group);
			consume(z, cflag::group);
			int index;
			if (parse_index(h,index)) {
				h.accept_group(index);
				return true;
			}
			return false;
		}

		bool parse_code(Handler& h) {
			std::string code;
			while (!equal(z, '\0') && !is_flag(z) && !equal(z, "\r\n")) {
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
			parse_whitespace(h);
			name.clear();
			while (!equal(z, '\0') && is_alpha(*z)) {
				name += *z;
				z++;
			}
			if (!name.empty()) {
				return true;
			}
			return false;
		}


		bool parse_index(Handler& h,int& index) {
			parse_whitespace(h);

			std::string integer;

			while (!equal(z, '\0') && is_digit(*z)) {
				integer += *z;
				z++;
			}
			if (!integer.empty()) {
				index = std::atoi(integer.c_str());
				return true;
			}
			return false;
		}

		

		bool parse_function(Handler& h) {
			expect(z, cflag::function);
			consume(z, cflag::function);
			if (parse_action(h)) {
				h.accept_line_type(LineInfo::Type::function);
				return true;
			}
			return false;
		}

		bool parse_local(Handler& h) {
			expect(z, cflag::local);
			consume(z, cflag::local);
			if (parse_action(h)) {
				h.accept_line_type(LineInfo::Type::local);
				return true;
			}
			return false;
		}

		bool parse_action(Handler& h) {

			while (!equal(z, '\0') && !equal(z,"\r\n")) {
				switch (*z)
				{
				case cflag::args: parse_args(h); break;
				case cflag::args_type: parse_args_type(h); break;
				case cflag::args_type2: parse_args_type2(h); break;
				case cflag::group: parse_group(h); break;
				default:
					parse_code(h); 
					break;
				}
			}
			h.accept_line_type(LineInfo::Type::action);
			return true;
		}
		

		bool parse_exp(Handler& h) {
			h.accept_newline();
			parse_whitespace(h);
			switch (*z) 
			{
			case cflag::function:
				return parse_function(h);
			case cflag::local:
				return parse_local(h);
			default:
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
