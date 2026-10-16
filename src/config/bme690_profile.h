#pragma once
#include <stdint.h>
// Filename labels 18v/33v mean 1.8 V/3.3 V, not 18 V/33 V.
#ifndef BSEC3_SUPPLY_MV
#define BSEC3_SUPPLY_MV 3300
#endif
#ifndef BSEC3_SAMPLE_PERIOD_S
#define BSEC3_SAMPLE_PERIOD_S 3
#endif
#ifndef BSEC3_HISTORY_DAYS
#define BSEC3_HISTORY_DAYS 4
#endif
#if BSEC3_SUPPLY_MV == 1800 && BSEC3_SAMPLE_PERIOD_S == 3 && BSEC3_HISTORY_DAYS == 4
#include "bme690/bme690_iaq_18v_3s_4d.h"
#elif BSEC3_SUPPLY_MV == 1800 && BSEC3_SAMPLE_PERIOD_S == 3 && BSEC3_HISTORY_DAYS == 28
#include "bme690/bme690_iaq_18v_3s_28d.h"
#elif BSEC3_SUPPLY_MV == 1800 && BSEC3_SAMPLE_PERIOD_S == 300 && BSEC3_HISTORY_DAYS == 4
#include "bme690/bme690_iaq_18v_300s_4d.h"
#elif BSEC3_SUPPLY_MV == 1800 && BSEC3_SAMPLE_PERIOD_S == 300 && BSEC3_HISTORY_DAYS == 28
#include "bme690/bme690_iaq_18v_300s_28d.h"
#elif BSEC3_SUPPLY_MV == 3300 && BSEC3_SAMPLE_PERIOD_S == 3 && BSEC3_HISTORY_DAYS == 4
#include "bme690/bme690_iaq_33v_3s_4d.h"
#elif BSEC3_SUPPLY_MV == 3300 && BSEC3_SAMPLE_PERIOD_S == 3 && BSEC3_HISTORY_DAYS == 28
#include "bme690/bme690_iaq_33v_3s_28d.h"
#elif BSEC3_SUPPLY_MV == 3300 && BSEC3_SAMPLE_PERIOD_S == 300 && BSEC3_HISTORY_DAYS == 4
#include "bme690/bme690_iaq_33v_300s_4d.h"
#elif BSEC3_SUPPLY_MV == 3300 && BSEC3_SAMPLE_PERIOD_S == 300 && BSEC3_HISTORY_DAYS == 28
#include "bme690/bme690_iaq_33v_300s_28d.h"
#else
#error "Use BME690 supply 1800/3300 mV, period 3/300 s, history 4/28 days"
#endif
#if BSEC3_SAMPLE_PERIOD_S == 3
#define BSEC3_PROFILE_SAMPLE_RATE BSEC_SAMPLE_RATE_LP
#else
#define BSEC3_PROFILE_SAMPLE_RATE BSEC_SAMPLE_RATE_ULP
#endif
