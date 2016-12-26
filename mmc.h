
#ifndef _MMC_H_
#define _MMC_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BLOCK_SIZE 512

int mmc_kill_shwp(const char *);
int mmc_part_read(int, unsigned int, char *, unsigned int);
int mmc_part_write(int, unsigned int, const char *, unsigned int);

#ifdef __cplusplus
}
#endif

#endif

