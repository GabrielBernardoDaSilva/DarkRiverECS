#pragma once

namespace darkriver
{
	enum class ArchetypeError
	{
		EntityNotFound,
		ComponentNotFound,
		ComponentTypeDoesNotMatch,
		EntityAlreadyHasComponent,
		EntityDoesNotHaveComponent,
	};

	enum class SchedulerError
	{
		TaskNotFound
	};

	struct Success
	{
	};
}