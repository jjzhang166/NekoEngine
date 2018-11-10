#ifndef __SYSTEM_EVENT_H__
#define __SYSTEM_EVENT_H__

enum System_Event_Type { InvalidEvent, DropEvent };

struct Drop_Event
{
	System_Event_Type type;
	char* path;
};

union System_Event
{
	System_Event_Type type;      /**< Event type, shared with all events */
	Drop_Event drop;             /**< Drag and drop event data */

	/* This is necessary for ABI compatibility between Visual C++ and GCC
	   Visual C++ will respect the push pack pragma and use 52 bytes for
	   this structure, and GCC will use the alignment of the largest datatype
	   within the union, which is 8 bytes.

	   So... we'll add padding to force the size to be 56 bytes for both.

	   padding?
	*/
};

#endif // !System Event
