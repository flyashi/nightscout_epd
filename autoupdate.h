#ifndef __AUTOUPDATE_H__
#define __AUTOUPDATE_H__

const int FW_VERSION = 2023102201;  // YYYYMMDDRR where R = release of the day

void check_for_remote_update(char* macAddr);

#endif /* __AUTOUPDATE_H__ */