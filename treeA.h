#ifndef treeA_h
#define treeA_h

#include <stdio.h>

struct avl_node {
	char *username;
	char *password;
	struct avl_node *right;
	struct avl_node *left;
};

typedef struct avl_node node;

struct avl_tree {
	struct avl_node *root;
};

typedef struct avl_tree tree;

/*Limpa a estrutura*/
void cleanTree(node* root);

/*Cria base de AVL*/
tree* createTree();

/*Cria temp nodes*/
node* createNode();

/*Altura da arvore*/
int heightTree(node* node);

/*Calcula Balanceamento*/
int balanceFactor(node* node);

/*4 funcoes de rotacao*/
node* rotateLL(node* node);

node* rotateLR(node* node);

node* rotateRL(node* node);

node* rotateRR(node* node);

/*Balancea consoante nodo*/
node* balanceNode(node* node);

/*Balancea AVL*/
void balanceTree(tree* root);

/*Insere na arvore*/
void insertTree(tree* root, char *name, char *pass);

/*Print da arvore*/
void printTree(node* root, FILE *fd);

/*1- Existe User, 0- Nao existe!*/
int existUser(node* root, char *name, char *pass);

#endif
