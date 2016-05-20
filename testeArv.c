#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treeA.c"

int main(int argc, char* argv[]){

	tree *root = NULL;

	FILE* fp;

	fp = fopen("teste.log", "w+");

	root = createTree();

	insertTree(root,"Zeca","Estaciona");
	insertTree(root,"Joana","Fana");
	insertTree(root,"Carlos","Silvita");
	insertTree(root,"Fanecas","Fritas");

	
existUser(root->root,"Joaquim","")==1?printf("\nYAY\n"):printf("\nNOOOO!\n");

	insertTree(root,"Joaquim","Fascinhas");
	
existUser(root->root,"Joaquim","Fascinhas")==1?printf("\nYAY\n"):printf("\nNOOOO!\n");
	
existUser(root->root,"Carlos","Fana")==1?printf("\nYAY\n"):printf("\nNOOOO!\n");
	
existUser(root->root,"Zeca","Estaciona")==1?printf("\nYAY\n"):printf("\nNOOOO!\n");

	printTree(root->root,fp);	

	cleanTree(root->root);

	free(root);

	fclose(fp);

	return 0;

}

