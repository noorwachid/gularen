#pragma once

namespace Gularen {
	enum struct NodeGroup {
		unknown = 0,

		string,
		quote,

		header,
		list,
		item,
		break_,

		link,
		table,
		code,
		tag,

		container,
	};
}
