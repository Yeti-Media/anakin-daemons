#if defined(ANAKIN_OCR_DAEMON)
#  include "configurations/ocr_daemon.h"
#elif defined(ANAKIN_README_UPDATE)
#  include "configurations/readme_update.h"
#elif defined(ANAKIN_ACCEPTANCE)
#  include "configurations/acceptance.h"
#elif defined(ANAKIN_UNIT_TESTING)
#  include "configurations/unit_testing.h"
#else
#  include "configurations/acceptance.h"
#endif
