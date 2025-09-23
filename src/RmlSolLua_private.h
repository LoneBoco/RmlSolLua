#pragma once

#ifdef RMLSOLLUA_HAS_SOL_PATH_OVERRIDE
#define SOLHPP <sol.hpp>
#else
#define SOLHPP <sol/sol.hpp>
#endif
