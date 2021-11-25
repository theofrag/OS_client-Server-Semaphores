#ifndef _COMMON_H_
#define _COMMON_H_

#define SEM_KEY_FILE ("sem.key")

#endif /* _COMMON_H_ */


union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};