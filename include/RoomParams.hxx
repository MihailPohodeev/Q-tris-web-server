#ifndef ROOMS_PARAMS_HXX
#define ROOMS_PARAMS_HXX

enum class QueueMode { Different, Same };

// parameters for room creation.
struct RoomParams
{
	QueueMode 	queueMode;
	int		playersCount;
	int		startLevel;
};

#endif
