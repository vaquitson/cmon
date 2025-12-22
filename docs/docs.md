#/lib/inotify 
This in this directory we can find all the related functions to 
the inotify API. The function start with the prefix ino_... to indentify
inotify fcuntions.


## int ino_init( void )
This function returns a flidescriptor related to the inotify instance.
In general, this function is a wrrapper around inotify_init() function.



