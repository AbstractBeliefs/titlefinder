#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>

int get_names(Display *display, Window window, char **wm_name){
    Atom WM_Name = XInternAtom(display, "_NET_WM_NAME", 0);
    Atom ActualType;
    int format;
    unsigned long ItemCount;
    unsigned long BytesRemaining;
    unsigned char *data;
    int result = XGetWindowProperty(
        display,                        // Display
        window,                         // Window
        WM_Name,                        // Property
        0,(~0L),                        // offset, length
        0,                              // Delete prop?
        AnyPropertyType,                // Type
        &ActualType,                    // Returned: actual type
        &format,                        // Returned: the actual format
        &ItemCount,                     // Returned: item count
        &BytesRemaining,                // Returned: number bytes left to read
        &data                           // Returned: the returned data
    );
    
    if (result < Success || ItemCount <= 0){
        XFree(data);
        return 1;
    }

    *wm_name = calloc(ItemCount+1, sizeof(char));
    strcpy(*wm_name, data);
    XFree(data);

    return 0;

}
int main(){
    int retval = 1;

    Display *display = XOpenDisplay(NULL);
    Atom ClientList = XInternAtom(display, "_NET_CLIENT_LIST", 0);
    
    Atom ActualType;
    int format;
    unsigned long ItemCount;
    unsigned long BytesRemaining;
    unsigned char *data;
    int result = XGetWindowProperty(
        display,                        // Display
        XDefaultRootWindow(display),    // Window
        ClientList,                     // Property
        0,(~0L),                        // offset, length
        0,                              // Delete prop?
        AnyPropertyType,                // Type
        &ActualType,                    // Returned: actual type
        &format,                        // Returned: the actual format
        &ItemCount,                     // Returned: item count
        &BytesRemaining,                // Returned: number bytes left to read
        &data                           // Returned: the returned data
    );

    if (result >= Success && ItemCount > 0){
        Window *windows = (Window*)data;
        for (size_t windex = 0; windex < ItemCount; windex++){
            Window w = (Window)windows[windex];

            char *wm_name;
            if (!get_names(display, w, &wm_name)){
                char *chop = strstr(wm_name, " - YouTube - Chromium");
                if (chop == NULL){
                    free(wm_name);
                    continue;
                }
                retval = 0;

                while (*chop) { *chop = '\0'; chop++; }
                printf("%s\n", wm_name);

                free(wm_name);
            }
        }
    }
 
    XFree(data);
    XCloseDisplay(display);
    return retval;
}
