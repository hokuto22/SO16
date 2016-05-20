#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "treeA.h"

// NODES SAO DESIGNADOS POR "NOD" DEVIDO A ERRO DE PARSING DO GCC

void cleanTree(node *t) {	

	if(t) {
		cleanTree(t->right);
		cleanTree(t->left);
		free(t->username);
		free(t->password);
		free(t);
	}
}

tree* createTree() {

	tree* tree = NULL;

	if((tree=malloc(sizeof(tree)))==NULL) 
		return NULL;

	tree->root = NULL;

	return tree;
}

node* createNode() {

	node* nod = NULL;

	if((nod=malloc(sizeof(node)))==NULL)
		return NULL;
	
	nod->left = NULL;
	nod->right= NULL;
	nod->username= " ";
	nod->password= " ";

	return nod;
}

int heightTree(node* nod) {
	int height_left = 0;
	int height_right = 0;

	if(nod->left)
		height_left = heightTree(nod->left);
	if(nod->right)
		height_right = heightTree(nod->right);

	return height_right > height_left ? ++height_right : ++height_left;
} 

int balanceFactor(node* node) {
	int bf = 0;

	if(node->left)
		bf += heightTree(node->left);
	if(node->right)
		bf -= heightTree(node->right);

	return bf;
} 

node* rotateLL(node* nod) {
	node* a = nod;
	node* b = a->left;

	a->left = b->right;
	b->right = a;

	return(b);
}

node* rotateLR(node* nod) {

	node* a = nod;
	node* b = a->left;
	node* c = b->right;

	a->left = c->right;
	b->right= c->left;
	c->left = b;
	c->right= a;

	return(c);
}

node* rotateRL(node* nod) {
	
	node* a = nod;
	node* b = a->right;
	node* c = b->left;

	a->right= c->left;
	b->left = c->right;
	c->right= b;
	c->left = a;

	return(c);
}

node* rotateRR(node* nod) {

	node* a = nod;
	node* b = a->right;

	a->right= b->left;
	b->left = a;

	return(b);
}

node* balanceNode(node* nod) {
	node* newroot = NULL;
	
	int bf = 0;

	if(nod->left)
		nod->left = balanceNode(nod->left);
	if(nod->right)
		nod->right= balanceNode(nod->right);

	bf = balanceFactor(nod);

	if(bf >= 2) {
		if(balanceFactor(nod->left) <= -1 )
			newroot = rotateLR(nod);
		else
			newroot = rotateLL(nod);
	}else if(bf <= -2) {
		if(balanceFactor(nod->right)<= 1)
			newroot = rotateRL(nod);
		else
			newroot = rotateRR(nod);
	}else
		newroot = nod;

	return(newroot);
}

void balanceTree(tree *t) {

	node* newroot = NULL;

	newroot = balanceNode(t->root);

	if(newroot != t->root)
		t->root = newroot;	
}

void insertTree(tree *t, char *name, char *pass) {

	node* nod = NULL;
	node* next = NULL;
	node* last = NULL;

	if(t->root == NULL) {
		nod = createNode();
		nod->username=strdup(name);
		nod->password=strdup(pass);
		t->root = nod;
	}else {
		next = t->root;

		while(next != NULL) {
			last = next;

			if(strcmp(next->username,name)>0)
				next = next->left;
			else
				next = next->right;
		}

		nod = createNode();
		nod->username = strdup(name);
		nod->password = strdup(pass);

		if(strcmp(last->username,name)>0) 
			last->left = nod;
		if(strcmp(last->username,name)<0)
			last->right= nod;
	}

	balanceTree(t);
}


void printTree(node *t, FILE *fd) {
	if(t != NULL) {
		printTree(t->left,fd);
		fprintf(fd,"%s;%s\n",t->username,t->password);
		printTree(t->right,fd);
	}
}
/* 0-NAO EXISTE! 1-EXISTE! */
int existUser(node *t,char *username, char *pass) {

	if(t) {
		if(strcmp(t->username,username) == 0){
			if(strcmp(t->password,pass) == 0)
				return 1;
			else
				return -4;
		}
		if(strcmp(t->username,username) > 0)
			return existUser(t->left,username,pass);
		else
			return existUser(t->right,username,pass);
	}	

	return -2;
}
