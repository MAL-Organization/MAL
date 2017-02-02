/*
 * uCUnit-v1.0.c
 *
 *  Created on: Jan 24, 2017
 *      Author: Olivier
 */

#include "uCUnit-v1.0.h"

/* Variables for simple statistics */
int ucunit_checks_failed = 0; /* Numer of failed checks */
int ucunit_checks_passed = 0; /* Number of passed checks */

int ucunit_testcases_failed = 0; /* Number of failed test cases */
int ucunit_testcases_passed = 0; /* Number of passed test cases */
int ucunit_testcases_failed_checks = 0; /* Number of failed checks in a testcase */
int ucunit_checklist_failed_checks = 0; /* Number of failed checks in a checklist */
int ucunit_action = UCUNIT_ACTION_WARNING; /* Action to take if a check fails */
int ucunit_checkpoints[UCUNIT_MAX_TRACEPOINTS]; /* Max. number of tracepoints */
int ucunit_index = 0; /* Tracepoint index */
