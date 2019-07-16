#include "stdafx.h"
#include "script.h"
#include "script_group.hpp"

namespace script {
	Converter::Converter()  
		:m_init(0)
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

		if (!m_group.load(current / "group.json")) {
			std::cout << "json¶ÁÈ¡Ê§°Ü\n";
			return false;
		}
		m_init = true;
		return true;
	}

	HandlerPtr Converter::find_script(ActionNodePtr node, const std::string& name)
	{

	}
}