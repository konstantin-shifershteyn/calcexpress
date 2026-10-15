#ifndef	CIN_IS_REENTRANT
#define	CIN_IS_REENTRANT

CIN MgErr CINProperties( int32 mode, void *data)
{
	switch (mode) {
		case kCINIsReentrant :
			*(Bool32 *)data = TRUE;
			return	noErr;
			break;
	}
	return mgNotSupported;
}

#endif
