#include "normalize.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

/* Remove Dot Algorithm outlined in RFC3986 section 5.2.4 */
int
remove_dot_segments(const char *path, char *ret_buffer, int buff_ct){
    
    //TODO: Ensure buffer is large enough (at least the size of the path) 

    /* Create an input buffer that we can change*/ 
    int path_len = strlen(path);
    char  inBuff[path_len];
    memset(inBuff,0,path_len); 
    strcpy(inBuff,path);


    const char *path_end = inBuff + path_len; 
    char *seg_start = inBuff;
    char *seg_end;
    char *write_buffer = ret_buffer;
    int seg_len;
    
    for(;;){
        if(seg_start == path_end){
            break;
        }
        seg_end = seg_start + 1;

        /*Parse such that Seg start and end hold indices the next full path segment*/ 
        while(seg_end!=path_end && *seg_end != '/'){
            seg_end++;
        }

        seg_len = seg_end - seg_start + 1; 

        /*Remove starting ../ or ./ from input buffer*/
        if(!strncmp(seg_start,"../",seg_len) || !strncmp(seg_start, "./", seg_len)){
            if(seg_end != path_end){
                seg_end ++;
            }
        }

        /*Remove starting /./ or /. from input buffer and replace with '/' in output buffer*/
        else if (!strncmp(seg_start, "/./", seg_len) || !strncmp(seg_start, "/.", seg_len)){
            *write_buffer = '/';
            write_buffer ++;
            if(seg_end != path_end){
                seg_end ++;
            }
        }

        // TODO THIS PART is writing in places it shouldn't sometimes if path passed is empty!!
        /* Replace '/../' or '/..' in input buffer with '/' in output buffer and remove previous path segment from output buffer*/ 
        else if(!strncmp(seg_start,"/../",seg_len) || !strncmp(seg_start,"/..",seg_len)){

            int prev_len = 0;
            while(*write_buffer != '/' && write_buffer != ret_buffer){
                prev_len ++;
                write_buffer --; 
            }
            memset(write_buffer,0,prev_len);
            if(seg_end != path_end){
                seg_start[seg_len-1] = '/';
            }
            else{
                seg_start[seg_len-2] = '/';
                seg_end --;
            }
        }
        /*Remove starting '.' or '..' from input buffer*/
        else if(!strncmp(seg_start,".",seg_len) || !strncmp(seg_start,"..", seg_len)){
            if(seg_end != path_end){
                seg_end ++;
            }
        }
        /* Place the current path segment to the output buffer including initial '/' but not the next '/'*/
        else{
            //Write first char to buffer 
            *write_buffer = seg_start[0];
            write_buffer ++;
            seg_start++;
            seg_len--;

            //and for subsequent characters:
            while(*seg_start != '/' && *seg_start != 0){
                *write_buffer = *seg_start;
                seg_start ++;
                write_buffer++;
            }
        }
        seg_start = seg_end;
    }

    fprintf(stderr,"Normalized Path: %s\n", ret_buffer);
    return strlen(ret_buffer);

}

/* Function returns length of the decoded string or -1 if there was a parsing error */
int
percent_decode(const char *uri, char *decoded_uri, int uri_ct, bool lower){

    static const char *reserved_string  = ":/?#[]@!$&\'()*+,;=";

    if(uri_ct <= 0){
       return 0; 
    }

    int offset = 0;
    int i;
    for(i=0; i < uri_ct; i++){
        if(uri[i] == '%'){
            /* Must be exactly two characters as part of the hex encoded value */
            if(i + 2 >= uri_ct){
                goto decode_failure;
            }
            char encodedVal[4] = {0};
            int j;
            for(j=0; j < 2; j++){
                if(isxdigit(uri[i+j+1])){
                    encodedVal[j] = uri[i+j+1];
                }
                else{
                    goto decode_failure;
                }
            }
            int hexVal;
            char decodeChar;
            sscanf(encodedVal, "%2x", &hexVal);
            decodeChar = (char)hexVal;
            /* If it is a reserved char, leave encoded*/
            if(strchr(reserved_string, decodeChar)){
                decoded_uri[i-offset] = uri[i];
                decoded_uri[i + 1 - offset] = toupper(uri[i+1]);
                decoded_uri[i + 2 - offset] = toupper(uri[i+2]);
            }
            else{
                if(lower){
                    decoded_uri[i - offset] = tolower(decodeChar);
                }
                else{
                    decoded_uri[i - offset] = decodeChar;
                }
                offset = offset + 2; 
            }
            i =i+2;
        }
        else{
            if(lower){
                decoded_uri[i-offset] = tolower(uri[i]);
            }
            else{
                decoded_uri[i - offset] = uri[i];
            }
        }
    }

    return uri_ct - offset;

decode_failure:
    fprintf(stderr, "ERROR Decoding String\n");
    return -1;
}

/* This function takes a uri and an initialized buffer to populate with the normalized uri.
 *
 * The buffer provided must be at least the length of the uri + 1 as the normalized uri will
 * potentially be one char larger than the original uri if a backslash is added to the path. 
 * 
 *   The normalization function returns a string with the following modifications
 *   1. Lowecase protocol/domain
 *   2. Path segments .. and . are removed from path 
 *   3. Alphabetical percent encoded octet values are toupper
 *   4. Non-reserved percent encoded octet values are decoded
 *   5. The Port is removed if it is default
 *   6. Defaults to a single backslash for the path segment if path segment is empty
*/
int
normalize_uri(const char *uri, char *normal_uri, int normal_ct){

    fprintf(stderr, "Normalizing URI: %s\n", uri);

    /* Buffer provided must be large enough to store the uri plus one additional char */
    //IF WE USE STRLEN(URI) again put it in a variable
    int uri_ct = strlen(uri);
    const char *uri_end = uri + uri_ct;

    if (normal_ct < uri_ct + 2 && normal_uri){
        fprintf(stderr, "Buffer to Normalize URI not large enough.\n");
        return -1;
    }

    char path_buffer[normal_ct];
    memset(path_buffer, 0, normal_ct);

    /* comp variables store starting/ending indexes for each uri component as it is parsed */
    const char *comp_start = uri;
    const char *comp_end = uri;
    char *write_buffer = normal_uri;
    bool https = false;

    /* Parse the protocol which will end with a colon */
    while(*comp_end != ':' && comp_end != uri_end){
        *write_buffer = tolower(*comp_end); 
        comp_end ++;
        write_buffer++;
    }

    if(comp_end == uri_end){
        fprintf(stderr, "Reached End of String prematurely\n");
        goto normalize_failure;
    }

    /* Copy the colon */
    *write_buffer = *comp_end; 
    comp_end ++;
    write_buffer++;

    /* Ensure the protocol is either http or https */
    if(strcmp("https:", normal_uri) == 0){
        https = true;
    }
    else if(strcmp("http:", normal_uri)){
        fprintf(stderr, "String is neither http or https\n");
        goto normalize_failure;
    }

    /* Protocol will be separated by double whacks */
    int i;
    for(i = 0; i < 2; i++){
        if(comp_end == uri_end || *comp_end != '/'){
            goto normalize_failure;
        }
        *write_buffer = *comp_end;
        comp_end++;
        write_buffer++;
    }

    if(comp_end == uri_end){
        goto normalize_failure;
    }

    /* Comp_start is index of start of authority component*/
    int comp_ct;
    comp_start = comp_end;

    /* Parse and decode the authority component and normalize userinfo if needed */
    bool userInfo = false;
    while(*comp_end != '/' && *comp_end != '?' && *comp_end != '#' && comp_end != uri_end){
        if(*comp_end == '@' && userInfo == false){
            comp_ct = comp_end - comp_start;
            comp_ct = percent_decode(comp_start, write_buffer, comp_ct, false);
            if(comp_ct < 0){
                goto normalize_failure;
            }
            comp_start = comp_end;
            userInfo = true;
            write_buffer = write_buffer + comp_ct;
        }
        comp_end ++;
    }

    if(userInfo == true && comp_end == uri_end){
        goto normalize_failure;
    }

    //COMP_END could be the end of the URI at this point.. IS this handled correclty???!!??!?!??!??!
    //IF so that is totally fine. Just keep that in mind when reviewing the code as we go
    //TODO Think we must add if comp_end == uri_end && userInfo == true, Then throw an error
    //What about http://dylanSouza:pewpoy@
 
    comp_ct = comp_end - comp_start;

    //TODO Decide if we want a if comp_ct == 0 return because we are done (could be http://??stooobabeebop)

    /* - comp start/end variables hold indices in original uri of hostname/port
     * - comp_buffer holds pointer to start of hostname/port in buffer 
     * - comp_ct holds size of hostname/port in original uri*/

    /* Parse and decode the hostname and port */
    comp_ct = percent_decode(comp_start, write_buffer, comp_ct, true);

    if(comp_ct < 0){
        goto normalize_failure;
    }

    ///* Remove the port from the buffer if default*/
    while(*write_buffer != 0){
        if(*write_buffer == ':'){
            if(https == true && !strncmp(write_buffer, ":443", 5)){
                memset(write_buffer,0,4);
                break;
            }
            else if(https == false && !strncmp(write_buffer, ":80", 4)){
                memset(write_buffer,0,3);
                break;
            }
        }
        write_buffer++;
    }
    
    comp_start = comp_end;

    /* We have reached the end of the authority section with an empty path component. Add trailing backslash */   
    if(*comp_end == 0 || *comp_end == '?' || *comp_end == '#'){
        *write_buffer = '/';
        write_buffer++;
    }

    //comp_end here either has the start of path component (if above not true) or remainder of the uri after the path
    //IF empty path component, then we still run this code anyway. Not optimal perhaps can decide not to!
    else{
        while(*comp_end != '?' && *comp_end != '#' && *comp_end != 0){
            comp_end ++;
        }
        /* Decode the path component */
        comp_ct = comp_end - comp_start;
        comp_ct = percent_decode(comp_start, path_buffer, comp_ct, false); 

        if(comp_ct < 0){
            goto normalize_failure;
        }

        /* Remove the . and .. segments from the path and place in the buffer that will be returned */
        fprintf(stderr,"Removing Dot Segments\n");
        comp_ct = remove_dot_segments(path_buffer, write_buffer, comp_ct);

        if(comp_ct < 0){
            fprintf(stderr,"Failure removing dot segments from path\n");
            goto normalize_failure;
        }
        write_buffer = write_buffer + comp_ct;

    }

    //If there is a remaining uri case normalize and decode it
    if(comp_end != uri_end){
        comp_start = comp_end;
        comp_ct = uri_end - comp_start;
        comp_ct = percent_decode(comp_start, write_buffer, comp_ct, false);
        if(comp_ct < 0){
            goto normalize_failure;
        }

    }

    fprintf(stderr, "Normalized URI:  %s\n", normal_uri);
    return 0;

normalize_failure:
    fprintf(stderr, "URI Normalization Failure. URI does not fit http or https schemes.\n");
    return -1;
}
