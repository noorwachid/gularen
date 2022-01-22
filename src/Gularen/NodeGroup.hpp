#pragma once

namespace Gularen
{
	enum struct NodeGroup
	{
		unknown = 0,

		text,

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
