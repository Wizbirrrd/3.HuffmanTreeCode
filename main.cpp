#include <iostream>
using namespace std;

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
#define OVERFLOW -1
#define MINSIZE 20
#define WEIGHT_ARRAY_LEN 27
#define STACK_INIT_SIZE 30
#define ARGC 3

typedef int Status;
typedef struct BiTNode {
  char Char;
  int weight;
  struct BiTNode *next; //用于构造树权值链表
  struct BiTNode *parent;
  struct BiTNode *lchild;
  struct BiTNode *rchild;
} BiTNode, *BiTree;

//构造两个栈用于递归
typedef struct {
  BiTree *base;
  BiTree *top;
} PtStack;
typedef struct {
  int *base;
  int *top;
} LvStack;

//构造编码表对字符编码进行存储
typedef struct CodeNode {
  char Char;
  char *code;
  struct CodeNode *next;
} CodeNode, *CodeTable;

void PtStackInit(PtStack &S) {
  S.base = (BiTree *)malloc(STACK_INIT_SIZE * sizeof(BiTree));
  if (!S.base) {
    exit(OVERFLOW);
  }
  S.top = S.base;
}
Status PtPush(PtStack &S, BiTree e) {
  *(S.top++) = e;
  return OK;
}
Status PtPop(PtStack &S, BiTree &e) {
  if (S.top == S.base) {
    return ERROR;
  }
  e = *--S.top;
  return OK;
}
Status EmptyStack(PtStack S) {
  if (S.top == S.base) {
    return TRUE;
  } else {
    return FALSE;
  }
}
void LvStackInit(LvStack &S) {
  S.base = (int *)malloc(STACK_INIT_SIZE * sizeof(int));
  if (!S.base) {
    exit(OVERFLOW);
  }
  S.top = S.base;
}
Status LvPop(LvStack &S, int &e) {
  if (S.top == S.base) {
    return ERROR;
  }
  e = *--S.top;
  return OK;
}
Status LvPush(LvStack &S, int e) {
  *(S.top++) = e;
  return OK;
}
Status StackDestroy(PtStack &S) {
  free(S.base);
  return OK;
}

//生成权值数组
Status WeightInit(char *s, int *w) {
  int len = 0;
  while (*s) {
    if (*s == ' ') {
      w[0]++;
    } else {
      w[*s - 'a' + 1]++;
    }
    s++;
    len++;
  }
  if (len < 20) {
    return ERROR;
  }
  return OK;
}

Status HuffmanInit(BiTree &Huffman, int *w) {
  int i = 0;
  for (i = 0; i < WEIGHT_ARRAY_LEN; i++) {
    if (w[i] != 0) {
      BiTNode *Node = (BiTNode *)malloc(sizeof(BiTNode));
      if (!Node)
        exit(OVERFLOW);
      Node->next = Huffman->next;
      Node->parent = Huffman;
      if (Huffman->next)
        Huffman->next->parent = Node;
      Huffman->next = Node;
      Node->Char = ((i == 0) ? ' ' : 'a' + i - 1);
      Node->weight = w[i];
      Node->parent = Huffman;
    } //根据权值数组构造带权值的循环链表
  }
  return OK;
}

//沿链表找到权值最小的结点并移出链表,返回该节点的指针
Status SelectMin(BiTree &Huffman, BiTree &p) {
  BiTree t = Huffman->next;
  p = t;
  if (!t->next) { //仅剩一个结点
    Huffman->next = NULL;
    return OK;
  }
  while (t) {
    if (p->weight >= t->weight) {
      p = t;
    } //等号使链表中靠后的字符先被取到,保证同等权值时较小字符在左子树
    t = t->next;
  }
  if (p->next)
    p->next->parent = p->parent;
  p->parent->next = p->next;
  p->next = NULL;
  return OK;
}

Status HuffmanCode(BiTree &Huffman) {
  BiTree p1;
  BiTree p2;
  while (Huffman->next->next) {
    SelectMin(Huffman, p1);
    SelectMin(Huffman, p2);
    BiTree Weight = (BiTree)malloc(sizeof(BiTNode));
    Weight->next = NULL;
    Weight->Char = '\0';
    Weight->weight = p1->weight + p2->weight;
    Weight->lchild = p1;
    p1->parent = Weight;
    Weight->rchild = p2;
    p2->parent = Weight;
    Weight->next = Huffman->next;
    Weight->parent = Huffman;
    if (Huffman->next) {
      Huffman->next->parent = Weight;
    }
    Huffman->next = Weight;
  } //用权值最小的结点构造子树并插入到链表首(序列末)
  p1 = Huffman;
  Huffman = Huffman->next;
  p1->next = NULL;
  free(p1); //释放空结点
  return OK;
}

//向上寻根,生成编码表
Status GetCode(BiTree p, CodeNode *Node, int level) {
  int i;
  Node->Char = p->Char;
  Node->code = (char *)malloc(level * sizeof(char));
  Node->code[level - 1] = '\0';
  for (i = level - 2; i >= 0; i--) {
    if (p->parent->lchild == p)
      Node->code[i] = '0';
    else
      Node->code[i] = '1';
    p = p->parent;
  }
  return 0;
}

//利用栈实现二叉树先序遍历的非递归算法,同时将字符的编码存入表中
Status BiTreeTraverse(BiTree Tree, CodeTable &c) {
  PtStack S;
  LvStack L;
  PtStackInit(S);
  LvStackInit(L);
  BiTree p = Tree;
  int level = 1;
  int degree = 0;
  //printf("字符\t权值\t  度\t层数\n");
  while (p || !EmptyStack(S)) {
    if (p) {
      if (p->Char) {
        //printf("\'%c\'", p->Char);
        CodeNode *Node = (CodeNode *)malloc(sizeof(CodeNode));
        if (!Node)
          exit(OVERFLOW);
        GetCode(p, Node, level);
        Node->next = c->next;
        c->next = Node;
        degree = 0;
      } else {
        //printf("   ");
        degree = 2;
      }
      // printf("%8d", p->weight);
      // printf("%8d", degree);
      // printf("%8d\n", level);
      PtPush(S, p->rchild);
      level++;
      LvPush(L, level);
      p = p->lchild;
    } else {
      PtPop(S, p);
      LvPop(L, level);
    }
  }
  StackDestroy(S);
  CodeTable q = c;
  c = c->next;
  q->next = NULL;
  free(q);
  return OK;
}

//根据编码表编码
Status AdCode(CodeTable c, char *string) {
  CodeTable p;
  while (*string != '\0') {
    p = c;
    while (p->Char != *string) {
      p = p->next;
    }
    printf("%s", p->code);
    string++;
  }

  return OK;
}

//向下寻叶,解码
Status DeCode(BiTree Tree, char *code) { return OK; }

Status HuffmanDestroy(BiTree Huffman) {
  free(Huffman);
  return OK;
}

int main(int argc, char **argv) {
  if (argc != ARGC) {
    printf("ERROR_01");
    return ERROR;
  } //命令行参数检查

  int *w = (int *)malloc(WEIGHT_ARRAY_LEN * sizeof(int));
  if (!w) {
    exit(OVERFLOW);
  }
  if (!WeightInit(argv[1], w)) { //构造权值数组
    printf("ERROR_02");
    return ERROR;
  };
  // for (int i = 0; i < W_ARRAY_LEN; i++) {
  //   cout << w[i] << endl;
  // } //WeightInit调试
  BiTree Huffman =
      (BiTree)malloc(sizeof(BiTNode)); //生成带头节点的链表,方便后续插入操作
  Huffman->next = NULL;
  HuffmanInit(Huffman, w); //构造叶子结点
  free(w);
  HuffmanCode(Huffman); //构造赫夫曼树

  CodeTable c = (CodeTable)malloc(sizeof(CodeNode));
  if (!c) {
    exit(OVERFLOW);
  }
  c->next = NULL;
  BiTreeTraverse(Huffman, c); //构造编码表
  // while (c) {
  //   printf("\'%c\':%s\n", c->Char, c->code);
  //   c = c->next;
  // } // Traverse调试
  AdCode(c, argv[1]);       //根据编码表解码
  DeCode(Huffman, argv[2]); //输出相应解码

  HuffmanDestroy(Huffman);
  free(c);
  return OK;
}
