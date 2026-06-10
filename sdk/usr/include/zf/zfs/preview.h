#ifndef FILTER_PREVIEW_H
#define FILTER_PREVIEW_H

class ZFScan;
class ZFSHeader;

bool NeedPreview( ZFSHeader* h );
void CreatePreviewJPG( ZFScan* zfs );


#endif //FILTER_PREVIEW_H
