//add here most rarely modified headers to speed up debug build compilation
#include "WorldSocket.h"                                    // must be first to make ACE happy with ACE includes in it
#include "Common.h"

#include "MapManager.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "Database/SQLStorage.h"
#include "Opcodes.h"
#include "SharedDefines.h"

//#ifdef FASTBUILD
//add additional headers here to speed up compilation in release builds even more
#include "ObjectMgr.h"
#include "World.h"
//#endif