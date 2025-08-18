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
        z = z->parent->parent; //while 더 돌기 위해서
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

// 해당 값의 주소값을 찾는 함수
node_t *rbtree_find(const rbtree *t, const key_t key) {
  // root부터 천천히 내려갈 생각임
  node_t *x = t->root;

  // nil 만날 때까지 돎
  while(x != t->nil){
    // 만약에 키값 같으면 찾은거니까 return x
    if(key == x->key) { return x; }
    // 이진 탐색
    x = (key < x->key) ? x->left : x->right;
  }

  //없으면 NULL
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {

  if(t->root == t->nil) return NULL;

  node_t *x = t->root;
  while(x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  
  if(t->root == t->nil) return NULL;

  node_t *x = t->root;
  while(x->right != t->nil){
    x = x->right;
  }
  return x;
}

// u : 삭제하려고 하는 노드, v : 붙이는 노드
static void rb_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){//u가 root일 때
    t->root = v;
  }else if (u = u->parent->left){ // v가 u의 왼쪽 자식일 때
    u->parent->left = v;
  }else{ // v가 u의 오른쪽 자식일 때
    u->parent->right = v;
  }
  v->parent = u->parent; // v가 nil일 때
}

static node_t *successor(rbtree *t, node_t *x){
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

int rbtree_erase(rbtree *t, node_t *z) {

  // y = 실제로 트리에서 빠져나가는 노드 (자식 0/1개 : z 자기자신, 자식 2개 : z의 successor)
  node_t *y = z;
  color_t y_origin_color = y->color;

  // x : y가 빠진 자리를 채우는 노드
  node_t *x;

  // 자식 0/1개일 때
  if(y->left == t->nil){ //오른쪽 자식만 있을 경우
    x = z->right;
    rb_transplant(t, z, z->right);
  } else if (y->right = t->nil) { // 왼쪽 자식만 있을 경우
    x = z->left;
    rb_transplant(t, z, z->left);
  } else { // 자식 2개일 경우
    y = successor(t, z->right); // 후계자
    y_origin_color = y->color;
    x = y->right; // y 자리에 가지고 와야하므로
    if (y->parent == z){ // 만약에 지우는 노드 바로 자식에 후계자(successor)가 있다면
      x->parent = y;
    } else {
      //y를 자기 자리에서 때어내고 그 자리를 y->right로 메움
      rb_transplant(t, y, y->right);

      // z의 오른쪽 서브트리를 y에 붙임
      y->right = z->right;
      y->right->parent = y;
    }

    // z 자리에 y를 올리고, z의 왼쪽 서브트리를 y에 붙임
    rb_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    // y는 z의 색을 승계 (실제로 삭제된 색은 y_original_color)
    y->color = z->color;
  }

    // 삭제된 색이 검정이면 균형 복원
  if (y_origin_color == RBTREE_BLACK) {
    delete_fixup(t, x); 
  }

  free(z);
  return 0;

}
// case 1만 부모가 검은색 인 이유 -> case 1 만 형제가 red이기 때문에 다음 노드인 부모는 red가 올 수 없음
static void delete_fixup(rbtree *t, node_t *x) { // x = 삭제된 노드가 차지하던 자리로 올라온 노드 = double black
  while(x != t->root && x->color == RBTREE_BLACK){ // x doubly black인 부분이 뿌리이거나 red로 되는 순간 끝
    if(x==x->parent->left){ // 부모가 오른쪽
      node_t *bro = x->parent->right; // 형제 생성

      // case 1 형제가 빨강일때
      if (bro->color == RBTREE_RED){ 
        // 회전하려면 형제와 부모 색깔을 바꿔야함
        bro->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent); // 그리고 회전
        bro = x->parent->right; // 회전 후 x의 위치도 바꼈으므로 bro의 위치도 갱신
      }

      // case 2 부터는 형제가 다 검은색임!
      // case 2 형제가 검정, 형제 자식도 검정 -> 싹다 검정
      if(bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK){
        // x와 형제의 검은색을 부모에게 줘버려
        bro->color = RBTREE_RED; // 그러면 형제는 red
        x = x->parent; // 부모가 x의 검정을 받으면서 자연스럽게 x는 부모가 된다.
      }else{
        // case 3 형제 검정, 형제 왼쪽 자식 red
        if(bro->right->color == RBTREE_BLACK){ // 오른 자식 
          // bro 기준으로 회전하기 위해 색깔 바꿈
          bro->color = RBTREE_RED; // bro 색깔을 red로
          bro->left->color = RBTREE_BLACK; // 회전 방향의 반대 방향 자식 색깔을 black
          right_rotate(t, bro); // 형제 기준 오른쪽 회전
          bro = x->parent->right; // 회전하고 형제 최신화
        }
        // case 4 형제 검정, 형제의 오른쪽 자식 red
        // 회전하기 전 색깔 바꿔줘야지
        bro->color = x->parent->color; // 형제의 색깔을 x의 부모의 색으로 바꿈
        x->parent->color = RBTREE_BLACK; // 부모 색은 형제 색 검은색 받음
        bro->right->color = RBTREE_BLACK; // 색 옮기는 과정에서 형제의 오른 자식이 검은색이 됨
        left_rotate(t, x->parent);
        //다 끝났으므로 종료
        x = t->root; 
      }
    }else{
      // 위의 대칭
      node_t *w = x->parent->left;  // 형제
      // Case 1
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // Case 2
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        // Case 3
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // Case 4
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

static void inorder_to_array(const rbtree *t, const node_t *x, key_t *arr, size_t n, size_t *idx){
  if (x == t->nil || *idx >= n) return; // 끝까지 탐색

  // 중위 순회 => 재귀를 통한
  inorder_to_array(t, x->left, arr, n, idx); 
  if(*idx < n) arr[(*idx)++] == x->key; // 방문 arr[i]에 키넣기
  inorder_to_arry(t, x->right, arr, n, idx);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  size_t idx = 0; // idx 0부터 시작
  inorder_to_array(t, t->root, arr, n, idx);
  return 0;
}