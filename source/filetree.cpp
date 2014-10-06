#include <string.h>

#include "filetree.h"

#include "virtual_file.h"

VirtualFile subfolder_indexhtml("index.html","Test Files\\subfolderindex.html","text","html");

VirtualObject* subfolder_subfiles[1] = {&subfolder_indexhtml};

VirtualFolder subfolder("subfolder", 1, subfolder_subfiles);

VirtualFile root_indexhtml("index.html","Test Files\\index.html","text","html");
VirtualFile root_faviconico("favicon.ico","Test Files\\favicon.ico","image","vnd.microsoft.icon");

VirtualObject* root_subfiles[3] = {&root_indexhtml, &root_faviconico, &subfolder};

//extern VirtualFolder webroot("", 3, root_subfiles);
//PhysicalFolder physical_webroot("", "Test Files", 0, nullptr);
//extern const VirtualFolder* webroot = &physical_webroot;
