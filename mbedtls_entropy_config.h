/*
 *  Copyright (C) 2006-2016, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

//#include "select-demo.h"

/* Enable entropy for K64F and K22F. This means entropy is disabled for all other targets. */
/* Do **NOT** deploy this code in production on other targets! */
/* See https://tls.mbed.org/kb/how-to/add-entropy-sources-to-entropy-pool */
#if defined(TARGET_K64F) || defined(TARGET_K22F)
#undef MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES
#undef MBEDTLS_TEST_NULL_ENTROPY
#endif

//#if DEMO == DEMO_HTTPS

#if !defined(MBEDTLS_ENTROPY_HARDWARE_ALT) && \
    !defined(MBEDTLS_ENTROPY_NV_SEED) && !defined(MBEDTLS_TEST_NULL_ENTROPY)
#error "This hardware does not have an entropy source."
#endif /* !MBEDTLS_ENTROPY_HARDWARE_ALT && !MBEDTLS_ENTROPY_NV_SEED &&
        * !MBEDTLS_TEST_NULL_ENTROPY */

#if !defined(MBEDTLS_SHA1_C)
#define MBEDTLS_SHA1_C
#endif /* !MBEDTLS_SHA1_C */

/*
 *  This value is sufficient for handling 2048 bit RSA keys.
 *
 *  Set this value higher to enable handling larger keys, but be aware that this
 *  will increase the stack usage.
 */
#define MBEDTLS_MPI_MAX_SIZE        512 //it was 256 before but cannot satisfy Azure iot hub certificate check

#define MBEDTLS_MPI_WINDOW_SIZE     1

//#endif
