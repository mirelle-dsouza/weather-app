#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* The API key is kept in secrets.h, which is NOT committed to git
   (see .gitignore). Copy secrets.h.example to secrets.h and paste your
   OpenWeatherMap key there. As a fallback the OPENWEATHER_API_KEY
   environment variable is used when secrets.h is missing. */
#if defined(__has_include)
#  if __has_include("secrets.h")
#    include "secrets.h"
#  endif
#endif

#ifndef OPENWEATHER_API_KEY
#  define OPENWEATHER_API_KEY ""
#endif

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("Out of memory!\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

int main(void) {
    CURL *curl;
    CURLcode res;

    const char *apiKey = OPENWEATHER_API_KEY;
    if (apiKey == NULL || apiKey[0] == '\0') {
        apiKey = getenv("OPENWEATHER_API_KEY");
    }

    if (apiKey == NULL || apiKey[0] == '\0') {
        printf("No OpenWeatherMap API key found.\n");
        printf("Copy secrets.h.example to secrets.h and add your key,\n");
        printf("or set the OPENWEATHER_API_KEY environment variable.\n");
        return 1;
    }

#ifdef _WIN32
    /* Make the console show the degree sign correctly. */
    SetConsoleOutputCP(CP_UTF8);
#endif

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        char city[50];
        printf("Enter city name: ");
        scanf("%s", city);

        char url[256];
        snprintf(url, sizeof(url),
                 "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric",
                 city, apiKey);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(res));
        } else {
            cJSON *json = cJSON_Parse(chunk.memory);
            if (json == NULL) {
                printf("Error parsing JSON\n");
            } else {
                cJSON *main = cJSON_GetObjectItem(json, "main");
                cJSON *weatherArr = cJSON_GetObjectItem(json, "weather");

                if (main && weatherArr) {
                    double temp = cJSON_GetObjectItem(main, "temp")->valuedouble;
                    cJSON *weather0 = cJSON_GetArrayItem(weatherArr, 0);
                    const char *desc = cJSON_GetObjectItem(weather0, "description")->valuestring;

                    printf("\n--- Weather Report ---\n");
                    printf("City: %s\n", city);
                    printf("Temperature: %.1f °C\n", temp);
                    printf("Condition: %s\n", desc);
                }

                cJSON_Delete(json);
            }
        }

        curl_easy_cleanup(curl);
        free(chunk.memory);
    }

    curl_global_cleanup();
    return 0;
}
