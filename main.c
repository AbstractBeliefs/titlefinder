#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>

// My WM titles for a chromium window with youtube looks like:
// (123) Unknown Mortal Orchestra - Multilove (Video) - YouTube - Chromium
// ^^^^^|                                           |^^^^^^^^^^^^^^^^^^^^^
//      This bit trimmed by title_without_alerts    |
//                                                  This bit trimmed in main

// Get the _NET_WM_NAME for a given window.
// Can be extended to get _NET_WM_VISIBLE_NAME, but not needed.
int get_names(Display *display, Window window, char **wm_name){
    Atom WM_Name = XInternAtom(display, "_NET_WM_NAME", 0);
    Atom ActualType;
    int format;
    unsigned long ItemCount;
    unsigned long BytesRemaining;
    unsigned char *wm_name_text_data;
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
        &wm_name_text_data              // Returned: the returned data
    );
    
    if (result < Success || ItemCount <= 0){
        XFree(wm_name_text_data);
        return 1;
    }

    *wm_name = calloc(ItemCount+1, sizeof(char));
    strcpy(*wm_name, wm_name_text_data);
    XFree(wm_name_text_data);

    return 0;
}

// Trim the leading alert count on a window title
char* title_without_alerts(char *source){
    if (*source != '('){
        return source;
    }

    char *new_index = source+1;  // Get past initial open paren
    char curr_char;
    int all_numbers = 1;
    while (curr_char = *new_index++, curr_char != ')' && curr_char != '\0'){
        if (curr_char < '0' || curr_char > '9'){
            all_numbers = 0;
        }
    }

    if (all_numbers && curr_char) {
        return new_index+1;
    } else {
        return source;
    }
}

int main(){
    int retval = 1;

    Display *display = XOpenDisplay(NULL);
    Atom ClientList = XInternAtom(display, "_NET_CLIENT_LIST", 0);
    
    Atom ActualType;
    int format;
    unsigned long ItemCount;
    unsigned long BytesRemaining;
    unsigned char *window_list;
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
        &window_list                    // Returned: the returned data
    );

    if (result >= Success && ItemCount > 0){
        Window *windows = (Window*)window_list;
        for (size_t windex = 0; windex < ItemCount; windex++){
            Window w = (Window)windows[windex];

            char *wm_name;
            if (!get_names(display, w, &wm_name)){
                char *chop = strstr(wm_name, " - YouTube");
                if (chop == NULL){
                    free(wm_name);
                    continue;
                }
                retval = 0;

                while (*chop) { *chop = '\0'; chop++; }
                char *start_pos = title_without_alerts(wm_name);
                printf("%s\n", start_pos);

                free(wm_name);
            }
        }
    }
 
    XFree(window_list);
    XCloseDisplay(display);
    return retval;
}
