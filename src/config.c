#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/config.h"
#include "../includes/utils.h"

static char *read_file(const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    perror("jobs.conf\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END); // me posiciono al final
  long len = ftell(file);   // calculo el tamano
  rewind(file);             // posicion al inicio

  char *data = malloc(len + 1);
  if (!data) {
    perror("data malloc.\n");
    return NULL;
  }

  fread(data, 1, len, file);
  data[len] = '\0';
  fclose(file);
  return data;
}

JobList load_config(const char *path) {
  JobList list = {0};

  char *json_data = read_file(path);
  if (json_data == NULL) {
    exit(1);
  }

  cJSON *root = cJSON_Parse(json_data);
  if (!root) {
    fprintf(stderr, "Invalid Json_data. \n");
    exit(1);
  }

  cJSON *jobs = cJSON_GetObjectItem(root, "jobs");
  if (!cJSON_IsArray(jobs)) {
    fprintf(stderr, "Must be an array. \n");
  }

  size_t count = cJSON_GetArraySize(jobs);
  Job *arr = calloc(count, sizeof(Job));
  if (!arr) {
    fprintf(stderr, "Error get memory system, arr Job");
    exit(1);
  }

  for (size_t i = 0; i < count; i++) {
    cJSON *item = cJSON_GetArrayItem(jobs, i);

    cJSON *name = cJSON_GetObjectItem(item, "name");
    cJSON *command = cJSON_GetObjectItem(item, "command");
    cJSON *path = cJSON_GetObjectItem(item, "path");
    cJSON *timeout = cJSON_GetObjectItem(item, "timeout");

    if (!cJSON_IsString(name) || !cJSON_IsString(command) ||
        !cJSON_IsString(path)) {
      fprintf(stderr, "Error data type item json_data \n");
      exit(1);
    }

    arr[i].name = strdup_custom(name->valuestring);
    arr[i].command = strdup_custom(command->valuestring);
    arr[i].path = strdup_custom(path->valuestring);
    arr[i].timeout = cJSON_IsNumber(timeout) ? timeout->valueint : 0;
  }

  list.jobs = arr;
  list.count = count;

  free(json_data);
  cJSON_Delete(root);

  return list;
}

void clean_job(JobList *list) {
  for (size_t i = 0; i < list->count; i++) {
    free(list->jobs[i].command);
    free(list->jobs[i].name);
    free(list->jobs[i].path);
  }

  free(list->jobs);
}
