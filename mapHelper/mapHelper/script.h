#include "stdafx.h"
#include "EditorData.h"
#include "ActionNode.h"

#include "script_handler.hpp"
#include "script_parser.hpp"
#include "script_group.hpp"

namespace script {

	class Converter {
	public:
		Converter();

		bool init();
		
		HandlerPtr find_script(ActionNodePtr node, const std::string& name);
		
		
		

	private:
		bool m_init;

		ActionGroup m_group;
	};



}

