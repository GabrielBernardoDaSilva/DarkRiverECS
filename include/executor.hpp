#pragma once

namespace darkriver
{
	enum class ExecutorType
	{
		Startup,
		Update,
		End
	};

	enum class ExecutorBehaviour
	{
		Once,
		Always
	};

}