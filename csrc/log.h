#ifndef CSRC_LOG_H
#define CSRC_LOG_H

#include <stdio.h>

/*
#define	LOG_PRINT(fmt, ...) \
			printf("[log]%s(): " fmt "\n", __func__, ## __VA_ARGS__)
*/
#define LOG_PRINT(fmt, ...)

#define	ERR_PRINT(fmt, ...) \
		fprintf(stderr, "[log]%s(): " fmt "\n", __func__, ## __VA_ARGS__)

#endif