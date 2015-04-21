#include <stdio.h>
#include <curl/curl.h>
#include "cJSON.h"
#include<libnotify/notify.h>



struct MemoryStruct {
    char *memory;
    size_t size;
};
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


int main(void)
{
    CURL *curl;
    CURLcode res;
    cJSON *json,*json_value;
    NotifyNotification *notify_p;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl,CURLOPT_URL,"http://api2.sinaapp.com/search/stock/?appkey=0020130430&appsecert=fa6095e113cd28fd&reqtype=text&keyword=000063");
        /* send all data to this function  */
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
          /* we pass our 'chunk' struct to the callback function */
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        res =curl_easy_perform(curl);
        /* check for errors */
         if(res != CURLE_OK) {
           fprintf(stderr, "curl_easy_perform() failed: %s\n",
                   curl_easy_strerror(res));
         }
         else {
           /*
            * Now, our chunk.memory points to a memory block that is chunk.size
            * bytes big and contains the remote file.
            *
            * Do something nice with it!
            */

           printf("%lu bytes retrieved\n", (long)chunk.size);
           printf("%s\n",chunk.memory);
           cJSON *json =cJSON_Parse(chunk.memory);
           cJSON *text =cJSON_GetObjectItem(json,"text");
           cJSON *content = cJSON_GetObjectItem(text,"content");
           printf("%s\n",content->valuestring);
           notify_init("hello");
           notify_p = notify_notification_new("大盘",content->valuestring,NULL);
           notify_notification_set_timeout(notify_p, 1000); 
           notify_notification_show(notify_p, NULL);

         }
    }
    curl_easy_cleanup(curl);
    free(chunk.memory);

      /* we're done with libcurl, so clean it up */
      curl_global_cleanup();

    return 0;
}

