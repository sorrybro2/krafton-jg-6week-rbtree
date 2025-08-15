#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (!p) return NULL;

  p->nil = (node_t *)calloc(1, sizeof(node_t));
  if (!p->nil) free(p); return NULL;

  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->nil->parent = p->nil;

  p->root = p->nil;

  return p;
}

// rbtree에 할당한 모든 메모리를 해체한다.
void delete_rbtree(rbtree *t) {
  // 재귀 중에 t->nil을 계속 참조하므로 노드들부터 메모리 해제
  delete_rbtree_recursive(t, t->root);

  //new_rbtree()함수에서 각각 calloc으로 할당해줬으므로, 각각 t->nil, t를 free
  free(t->nil); 
  free(t);
}

// delete_rbtree 함수가 실행되면 안전하게 후위로 노드 메모리 해제
void delete_rbtree_recursive(rbtree *t, node_t *node){
  if(node==t->nil) return;
  delete_rbtree_recursive(t, node->left);
  delete_rbtree_recursive(t, node->right);
  free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
