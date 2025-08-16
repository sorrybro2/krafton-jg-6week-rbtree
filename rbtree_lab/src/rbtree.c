#include "rbtree.h"
#include <stdlib.h>

static void delete_rbtree_recursive(rbtree *t, node_t *node);
static void left_rotate(rbtree *t, node_t *x);
static void right_rotate(rbtree *t, node_t *x);
static void insert_fixup(rbtree *t, node_t *z);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (!p) return NULL;

  p->nil = (node_t *)calloc(1, sizeof(node_t));
  if (!p->nil) {free(p); return NULL;}

  p->nil->color = RBTREE_BLACK;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->nil->parent = p->nil;

  p->root = p->nil;

  return p;
}

// delete_rbtree 함수가 실행되면 안전하게 후위로 노드 메모리 해제
static void delete_rbtree_recursive(rbtree *t, node_t *node){
  if(node==t->nil) return;
  delete_rbtree_recursive(t, node->left);
  delete_rbtree_recursive(t, node->right);
  free(node);
}

// rbtree에 할당한 모든 메모리를 해체한다.
void delete_rbtree(rbtree *t) {
  // 재귀 중에 t->nil을 계속 참조하므로 노드들부터 메모리 해제
  delete_rbtree_recursive(t, t->root);

  //new_rbtree()함수에서 각각 calloc으로 할당해줬으므로, 각각 t->nil, t를 free
  free(t->nil); 
  free(t);
}

static void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right; // x의 오른쪽 자식 y를 생성

  x->right = y->left; // 회전 후 x의 오른쪽 자식은 y의 왼쪽 서브트리 

  if(y->left != t->nil){ //y의 왼쪽 서브트리가 nil이 아니라면
    y->left->parent = x;// 그것의 부모는 x이다
  }

  y->parent = x->parent;//y의 부모는 x의 부모이다.
  
  //어차피 y가 x의 부모로 올라가는건 같아
  if(x->parent == t->nil){ // x가 root일때
    t->root = y;
  }else if(x == x->parent->left){ // x가 root가 아닐 때, x가 왼쪽 자식일 때
    x->parent->left = y; // 그 자리에 y
  }else{ // x가 root가 아닐 때, x가 오른쪽 자식일 때
    x->parent->right = y;
  }

  //y의 왼쪽노드로 꺼지시고
  y->left = x;
  //반대로 x의 부모는 y임
  x->parent = y;
}

static void right_rotate(rbtree *t, node_t *x){
  node_t *y = x->left;
  x->left = y->right;
  if(y->right != t->nil){
    y->right->parent = x;
  }

  y->parent = x->parent;

  if(x->parent == t->nil){
    t->root = y;
  }else if(x == x->parent->left){
    x->parent->left = y;
  }else{
    x->parent->right = y;
  }

  y->right = x;
  x->parent = y;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  if (!z) return NULL;
  
  // 삽입노드 z 초기화
  z->key = key;
  z->color = RBTREE_RED;
  z->left = t->nil;
  z->right = t->nil;
  z->parent = t->nil;
  
  node_t *y = t->nil; // z가 붙을 노드
  node_t *x = t->root;// 내려가며 비교할 커서
  while(x != t->nil){
    y = x; // z가 붙을 노드 이므로 계속 추격
    if(key < x->key) x = x -> left;
    else x = x -> right;
  }
  // 부모-자식 연결
  z->parent = y;

  //z 삽입 조건문
  if(y == t->nil){ //트리에 아무것도 없을 때
    t->root = z;
  }else if(key < y->key){ //부모보다 값이 작으면
    y->left = z;
  }else{
    y->right = z; // 부모보다 값이 크면
  }

  insert_fixup(t, z);
  return z;
}

static void insert_fixup(rbtree *t, node_t *z){

  // 삽입한 노드 바로 위가 빨간색이면 계속 돎
  while(z->parent->color == RBTREE_RED){ 

    // 할아버지 기준 삽입 노드가 왼쪽으로 들어갔을 때
    if(z->parent == z->parent->parent->left){
      node_t *y = z->parent->parent->right; // 삼촌 노드

      // case 1 : 삼촌이 빨간색
      if(y->color == RBTREE_RED){

        // 부모와 자녀 둘의 색깔 바꾸기
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        y->color = RBTREE_BLACK;
        z = z->parent->parent;
      } else { // 삼촌이 검은색인 경우

        // case 2 : red가 꺾여서 삽입됐을 때
        if(z==z->parent->right){
          z = z->parent;
          left_rotate(t,z);// 꺾이는거 피고 case 1 ㄱㄱ
        }

        // case 1 : 한쪽에 red가 몰려 있는 경우
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // 삽입 노드 기준 할아버지랑 부모 색 바꾸고 할아버지 기준 오른쪽회전
        right_rotate(t,z->parent->parent);
      }
    } else { // 할아버지 기준 삽입 노드가 오른쪽으로 들어갔을 때 = 정반대
      node_t *y = z->parent->parent->left;

      // case 1
      if(y->color == RBTREE_RED){ // 삼촌이 red
        y->color = RBTREE_BLACK;
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
      } else { // 삼촌 black

        // case 2
        if(z==z->parent->left){
          z = z->parent;
          right_rotate(t,z);
        }
        // case 1
        z->parent->parent->color = RBTREE_RED;
        z->parent->color = RBTREE_BLACK;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
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

// int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
//   // TODO: implement to_array
//   return 0;
//}
