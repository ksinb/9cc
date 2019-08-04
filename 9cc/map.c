#include "9cc.h"

Map *new_map() {
  Map *map = malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
}

void map_put(Map *map, char *key, void *val) {
  vec_push(map->keys, key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
  for (int i = map->keys->len - 1; i >= 0; i--) {
    if (!strcmp(map->keys->data[i], key)) {
      return map->vals->data[i];
    }
  }
  return NULL;
}

bool map_exists(Map *map, char *key) {
  for (int i = 0; i < map->keys->len; i++)
    if (!strcmp(map->keys->data[i], key))
      return true;
  return false;
}