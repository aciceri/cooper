#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ERROR(msg) {printf(msg); printf("\n"); exit(EXIT_FAILURE);}             


typedef struct t_syntaxTree { //Per rappresentare gli alberi sintattici
  char nodeName[16]; //Lunghezza massima scelta arbitrariamente
  int nodesLen; //Numero di figli dell'albero
  struct t_syntaxTree** nodes; //Puntatore 
} t_syntaxTree;

long int gcd(long int a, long int b) { //Massimo comun divisore 
  return b == 0 ? a : gcd(b, a % b);
}


long int lcm(long int a, long int b) { //Minimo comune multiplo 
  return abs((a / gcd(a, b)) * b);
}


t_syntaxTree* buildTree(int first, char** tokens) { //first è l'indice del
                                                    //token attualmente
                                                    //considerato 
  t_syntaxTree* tree = malloc(sizeof(t_syntaxTree));
  tree->nodes = NULL;
  int open; //Numero di parentesi aperte incontrate finora

  if (tokens[first][0] == '(') {
    if (tokens[first + 1][0] == ')') //Non mi aspetto cose del tipo "()"
      ERROR("Parsing error: empty S-expression '()'");
    if (tokens[first + 1][0] == '(') //Nemmeno del tipo "(() blah blah)"
      ERROR("Parsing error: the root of an S-expression can't \
be another non-banal S-expression, e.g. '(())' is not ok");

    first++; //Passo al token successivo (che in realtà è già stato consumato)
    tree->nodesLen = 0; //Il nodo attuale potrebbe non avere figli
    strcpy(tree->nodeName, tokens[first]); //Il nome del nodo è il primo token
    open = 1; //Per ora abbiamo incontrato una parentesi aperta i.e. siamo al
              //primo livello di annidamento 

    do { 
      first++; //Passo al token successivo

      if (open == 1 && tokens[first][0] != ')') {
	tree->nodesLen++;
	tree->nodes = realloc(tree->nodes, sizeof(t_syntaxTree*) * tree->nodesLen);
	tree->nodes[tree->nodesLen-1] = buildTree(first, tokens);
      }

      if (tokens[first][0] == '(') open++;

      if (tokens[first][0] == ')') open--;
    } while (open != 0);
  }

  else {
    strcpy(tree->nodeName, tokens[first]);
    tree->nodesLen = 0;
    tree->nodes = NULL;
  }

  return tree;
}

int isNumber(char* str) {
  return !(atoi(str) == 0 && str[0] != '0');
}

void checkTree(t_syntaxTree* tree) {
  if (strcmp(tree->nodeName, "and"))
    ERROR("Expression error: the main expression is not an 'and'");
  if (tree->nodesLen == 0) {
    ERROR("Expression error: the main 'and' expression \
must have at least one child");
  }
  for (int i=0; i<tree->nodesLen; i++) {
    if (strcmp(tree->nodes[i]->nodeName, "=") &&
	strcmp(tree->nodes[i]->nodeName, ">") &&
	strcmp(tree->nodes[i]->nodeName, "div"))
      ERROR("Expression error: the children of the main 'and' \
can only be '=', '>' or 'div'");

    if (tree->nodes[i]->nodesLen != 2)
      ERROR("Expression error: '=', '>', 'div' must have two children");
    if (strcmp(tree->nodes[i]->nodes[0]->nodeName, "+"))
      ERROR("Expression error: the first child of '=', '>' or 'div' \
must be '+'");
    if (!isNumber(tree->nodes[i]->nodes[1]->nodeName))
      ERROR("Expression error: the second child of '=', '>', or 'div' \
must be a number constant");

    for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
      if(strcmp(tree->nodes[i]->nodes[0]->nodes[j]->nodeName, "*"))
	ERROR("Expression error: the children of '+' must be '*'");
      if(tree->nodes[i]->nodes[0]->nodes[j]->nodesLen == 0)
	ERROR("Expression error: '*' must have at least one child");
      if (tree->nodes[i]->nodes[0]->nodes[j]->nodesLen != 2)
	ERROR("Expression error: '*' must have two children");
      if (!isNumber(tree->nodes[i]->nodes[0]->nodes[j]->nodes[0]->nodeName) +
	  isNumber(tree->nodes[i]->nodes[0]->nodes[j]->nodes[1]->nodeName))
	ERROR("Expression error: the first child of '*' must \
be a number costant and the second must be a variable");
    }
  }
}


t_syntaxTree* parse(char* wff, int strict) {
  char* wffSpaced = malloc(sizeof(char));
  wffSpaced[0] = wff[0];
  int j = 1;

  for (int i = 1; i < strlen(wff) + 1; i++) {

    if (wff[i - 1] == '(') {
      wffSpaced = realloc(wffSpaced, sizeof(char) * (j + 2));
      wffSpaced[j] = ' ';
      wffSpaced[j + 1] = wff[i];
      j += 2;
    }

    else if (wff[i + 1] == ')') {
      wffSpaced = realloc(wffSpaced, sizeof(char) * (j + 2));
      wffSpaced[j] = wff[i];
      wffSpaced[j + 1] = ' ';
      j += 2;
    }

    else {
      wffSpaced = realloc(wffSpaced, sizeof(char) * (j + 1));
      wffSpaced[j] = wff[i];
      j++;
    }
  }

  char* token;
  int nTokens = 1;
  char** tokens = malloc(sizeof(char *));
  tokens[0] = strtok(wffSpaced, " ");

  while ((token = strtok(NULL, " ")) != NULL) {
    nTokens++;
    tokens = realloc(tokens, sizeof(char *) * nTokens);
    tokens[nTokens - 1] = token;
  }

  int countPar = 0;

  for(int i=0; i<nTokens; i++) {
    for(int j=0; j<strlen(tokens[i]); j++)
	  if(tokens[i][j] == ')' && j!= 0)
	    ERROR("Parsing error: every S-expression must \
have a root and at least an argument");
    if (tokens[i][0] == '(') countPar++;
    if (tokens[i][0] == ')') countPar--;
  }

  if (countPar != 0)
    ERROR("Parsing error: the number of parentheses is not even");

  t_syntaxTree* syntaxTree = buildTree(0, tokens);

  if (strict) checkTree(syntaxTree); //chiama exit() se l'albero non va bene

  free(wffSpaced);
  free(tokens);

  return syntaxTree;
}


int getLCM(t_syntaxTree* tree, char* var) {
  if (tree->nodeName[0] == '*') {
    if (strcmp(((t_syntaxTree *)tree->nodes[1])->nodeName, var) == 0) {
      return atoi(((t_syntaxTree *) tree->nodes[0])->nodeName);
    }
  }

  int l = 1;

  for(int i=0; i<tree->nodesLen; i++) {
    l = lcm(l, getLCM((t_syntaxTree *) tree->nodes[i], var));
  }

  return l;
}


void normalize(t_syntaxTree* tree, char* var) {
  int lcm = getLCM(tree, var);
  int c = lcm; //se un parametro di and non ha la x allora il coefficiente per cui si moltiplica è lcm così è come fare per uno

  for (int i=0; i<tree->nodesLen; i++) {
    if (strcmp("=", tree->nodes[i]->nodeName) == 0 ||
	strcmp("div", tree->nodes[i]->nodeName) == 0) {
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0)
	  c = atoi(addends[j]->nodes[0]->nodeName);
      }


      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) {
	  strcpy(addends[j]->nodeName, var);
	  free(addends[j]->nodes[0]);
	  free(addends[j]->nodes[1]);
	  addends[j]->nodesLen = 0;
	}
	else {
	  sprintf(addends[j]->nodes[0]->nodeName,
		  "%d",
		  atoi(addends[j]->nodes[0]->nodeName)*lcm/c);
	}
      }
      //printf("\n\n%d %s\n\n", lcm/c, tree->nodes[i]->nodes[1]->nodeName);
      sprintf(tree->nodes[i]->nodes[1]->nodeName,
	      "%d",
	      atoi(tree->nodes[i]->nodes[1]->nodeName)*lcm/c);
    }

    else if (strcmp(">", tree->nodes[i]->nodeName) == 0) {
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
        if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) {
          c = atoi(addends[j]->nodes[0]->nodeName); /*printf("\n\n%d %s %s\n\n", c, addends[j]->nodes[1]->nodeName, var)*/;}
      }


      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) {
	  if (c>0) strcpy(addends[j]->nodeName, "");
	  else strcpy(addends[j]->nodeName, "-");
	  strcat(addends[j]->nodeName, var);
	  free(addends[j]->nodes[0]);
	  free(addends[j]->nodes[1]);
	  addends[j]->nodesLen = 0;
	}
	else {
	  sprintf(addends[j]->nodes[0]->nodeName,
		  "%d",
            atoi(addends[j]->nodes[0]->nodeName)*abs(lcm/c));
	}
      }

      sprintf(tree->nodes[i]->nodes[1]->nodeName,
	      "%d",
	      atoi(tree->nodes[i]->nodes[1]->nodeName)*lcm/abs(c));
    }
  }

  tree->nodesLen++;
  tree->nodes = realloc(tree->nodes, sizeof(t_syntaxTree*) * tree->nodesLen);
  tree->nodes[tree->nodesLen-1] = malloc(sizeof(t_syntaxTree));
  strcpy(tree->nodes[tree->nodesLen-1]->nodeName, "div");
  tree->nodes[tree->nodesLen-1]->nodesLen = 2;
  tree->nodes[tree->nodesLen-1]->nodes = malloc(sizeof(t_syntaxTree*) * 2);
  tree->nodes[tree->nodesLen-1]->nodes[0] = malloc(sizeof(t_syntaxTree));
  tree->nodes[tree->nodesLen-1]->nodes[1] = malloc(sizeof(t_syntaxTree));
  tree->nodes[tree->nodesLen-1]->nodes[0]->nodesLen = 0;
  tree->nodes[tree->nodesLen-1]->nodes[0]->nodes = NULL;
  tree->nodes[tree->nodesLen-1]->nodes[1]->nodesLen = 0;
  tree->nodes[tree->nodesLen-1]->nodes[1]->nodes = NULL;
  strcpy(tree->nodes[tree->nodesLen-1]->nodes[0]->nodeName, var);
  sprintf(tree->nodes[tree->nodesLen-1]->nodes[1]->nodeName, "%d", lcm);
}


void recFree(t_syntaxTree* tree) {
  for (int i=0; i<tree->nodesLen; i++) {
    recFree(tree->nodes[i]);
  }

  free(tree->nodes);
  free(tree);
}


t_syntaxTree* recCopy(t_syntaxTree* tree) {
  t_syntaxTree* t = malloc(sizeof(t_syntaxTree));
  t->nodesLen = tree->nodesLen;

  if (t->nodesLen == 0)
    t->nodes = NULL;
  else
    t->nodes = malloc(sizeof(t_syntaxTree*) * t->nodesLen);

  strcpy(t->nodeName, tree->nodeName);

  for(int i=0; i<t->nodesLen; i++) {
    t->nodes[i] = recCopy(tree->nodes[i]);
  }

  return t;
}


t_syntaxTree* minInf(t_syntaxTree* tree, char* var) {
  t_syntaxTree* nTree = recCopy(tree);

  char minvar[16];
  minvar[0] = '\0';
  strcpy(minvar, "-");
  strcat(minvar, var);

  for (int i=0; i<nTree->nodesLen; i++) {
    if (strcmp(">", nTree->nodes[i]->nodeName) == 0) {
      t_syntaxTree** addends = nTree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<nTree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(addends[j]->nodeName, var) == 0)
	  strcpy(nTree->nodes[i]->nodeName, "false");
	else if (strcmp(addends[j]->nodeName, minvar) == 0)
	  strcpy(nTree->nodes[i]->nodeName, "true");
      }

      for (int j=0; j<nTree->nodes[i]->nodesLen; j++)
	recFree(nTree->nodes[i]->nodes[j]);

      free(nTree->nodes[i]->nodes);
      nTree->nodes[i]->nodesLen = 0;
      nTree->nodes[i]->nodes = NULL;
    }

    else if (strcmp("=", nTree->nodes[i]->nodeName) == 0) {
      for (int j=0; j<nTree->nodes[i]->nodesLen; j++)
	recFree(nTree->nodes[i]->nodes[j]);

      free(nTree->nodes[i]->nodes);
      nTree->nodes[i]->nodesLen = 0;
      nTree->nodes[i]->nodes = NULL;
      strcpy(nTree->nodes[i]->nodeName, "false");
    }
  }

  return nTree;
}


void eval(t_syntaxTree* tree, char* var, t_syntaxTree* val) {
  for (int i=0; i<tree->nodesLen; i++) {
    if (strcmp(tree->nodes[i]->nodeName, var) == 0) {
      recFree(tree->nodes[i]);
      tree->nodes[i] = recCopy(val);
    }
    else {
      char mvar[17] = "-";
      strcat(mvar, var);
      if (strcmp(tree->nodes[i]->nodeName, mvar) == 0) {
        recFree(tree->nodes[i]);

        tree->nodes[i] = malloc(sizeof(t_syntaxTree));
        strcpy(tree->nodes[i]->nodeName, "-");
        tree->nodes[i]->nodesLen = 1;
        tree->nodes[i]->nodes = malloc(sizeof(t_syntaxTree*));
tree->nodes[i]->nodes[0] = recCopy(val);
      }
    }

    eval(tree->nodes[i], var, val);
  }
}


int calcm(t_syntaxTree* tree, char* var) {
  int m=1;

  for(int i=0; i<tree->nodesLen; i++) {
    if(strcmp(tree->nodes[i]->nodeName, "div") == 0) {

      if(strcmp(tree->nodes[i]->nodes[0]->nodeName, var) == 0)
	m = lcm(m, atoi(tree->nodes[i]->nodes[1]->nodeName));

      else if(strcmp(tree->nodes[i]->nodes[0]->nodeName, "+") == 0) {
	for(int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	  if (strcmp(tree->nodes[i]->nodes[0]->nodes[j]->nodeName, var) == 0) {
	    m = lcm(m, atoi(tree->nodes[i]->nodes[1]->nodeName));
	    break;
	  }
	}
      }
    }
  }

  return m;
}


t_syntaxTree* boundaryPoints(t_syntaxTree* tree, char* var) {
  char str[16];
  str[0] = '\0';
  t_syntaxTree* bPoints = malloc(sizeof(t_syntaxTree));
  bPoints->nodes = NULL;
  strcpy(bPoints->nodeName, "bPoints"); //nome solo per debugging
  bPoints->nodesLen = 0;

  for(int i=0; i<tree->nodesLen; i++) {
    if (strcmp(tree->nodes[i]->nodeName, "=") == 0) {
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(var, addends[j]->nodeName) == 0) {
	  bPoints->nodesLen++;
	  bPoints->nodes = realloc(bPoints->nodes, sizeof(t_syntaxTree *) * bPoints->nodesLen);
	  t_syntaxTree* bp = malloc(sizeof(t_syntaxTree));
	  bp->nodes = NULL;
	  strcpy(bp->nodeName, "+");
	  bp->nodesLen = 0;

	  for (int k=0; k<tree->nodes[i]->nodes[0]->nodesLen; k++) {
	    if (strcmp(var, addends[k]->nodeName) != 0) {
	      bp->nodesLen++;
	      bp->nodes = realloc(bp->nodes, sizeof(t_syntaxTree*) * bp->nodesLen);
	      bp->nodes[bp->nodesLen-1] = recCopy(addends[k]);

	      sprintf(str, "%d", -atoi(bp->nodes[bp->nodesLen-1]->nodes[0]->nodeName));
	      strcpy(bp->nodes[bp->nodesLen-1]->nodes[0]->nodeName, str);
	    }
	  }

	  bp->nodesLen++;
	  bp->nodes = realloc(bp->nodes, sizeof(t_syntaxTree*) * bp->nodesLen);
	  bp->nodes[bp->nodesLen-1] = malloc(sizeof(t_syntaxTree));
	  bp->nodes[bp->nodesLen - 1]->nodesLen = 0;
	  bp->nodes[bp->nodesLen - 1]->nodes = NULL;
	  sprintf(str, "%d", -1+atoi(tree->nodes[i]->nodes[1]->nodeName));
	  strcpy(bp->nodes[bp->nodesLen - 1]->nodeName, str);

	  bPoints->nodes[bPoints->nodesLen-1] = bp;
	  break;
	}
      }
    }

    if (strcmp(tree->nodes[i]->nodeName, ">") == 0) {
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
	if (strcmp(var, addends[j]->nodeName) == 0) {
	  bPoints->nodesLen++;
	  bPoints->nodes = realloc(bPoints->nodes, sizeof(t_syntaxTree *) * bPoints->nodesLen);
	  t_syntaxTree* bp = malloc(sizeof(t_syntaxTree));
	  bp->nodes = NULL;
	  strcpy(bp->nodeName, "+");
	  bp->nodesLen = 0;

	  for (int k=0; k<tree->nodes[i]->nodes[0]->nodesLen; k++) {
	    if (strcmp(var, addends[k]->nodeName) != 0) {
	      bp->nodesLen++;
	      bp->nodes = realloc(bp->nodes, sizeof(t_syntaxTree*) * bp->nodesLen);
	      bp->nodes[bp->nodesLen-1] = recCopy(addends[k]);
	      sprintf(str, "%d", -atoi(bp->nodes[bp->nodesLen-1]->nodes[0]->nodeName));
	      strcpy(bp->nodes[bp->nodesLen-1]->nodes[0]->nodeName, str);
	    }
	  }

	  bp->nodesLen++;
	  bp->nodes = realloc(bp->nodes, sizeof(t_syntaxTree*) * bp->nodesLen);
	  bp->nodes[bp->nodesLen-1] = malloc(sizeof(t_syntaxTree));
	  bp->nodes[bp->nodesLen - 1]->nodesLen = 0;
	  bp->nodes[bp->nodesLen - 1]->nodes = NULL;
	  sprintf(str, "%d", +atoi(tree->nodes[i]->nodes[1]->nodeName));
	  strcpy(bp->nodes[bp->nodesLen - 1]->nodeName, str);

	  bPoints->nodes[bPoints->nodesLen-1] = bp;
	  break;
	}
      }
    }
  }

  return bPoints;
}


t_syntaxTree* newFormula(t_syntaxTree* tree, t_syntaxTree* minf, char* var) {
  int m = calcm(minf, var);
  t_syntaxTree* val;
  char str[16];
  t_syntaxTree* nTree = malloc(sizeof(t_syntaxTree));
  strcpy(nTree->nodeName, "or");
  nTree->nodesLen = 0;
  nTree->nodes = NULL;

  t_syntaxTree* t;
  t_syntaxTree* bp;
  t_syntaxTree *bPts = boundaryPoints(tree, var);

  for(int i=1; i<=m; i++) {
    nTree->nodesLen++;
    nTree->nodes = realloc(nTree->nodes, sizeof(t_syntaxTree *) * nTree->nodesLen);
    t = recCopy(minf);
    val = malloc(sizeof(t_syntaxTree));
    sprintf(str, "%d", i);
    strcpy(val->nodeName, str);
    val->nodesLen = 0;
    val->nodes = NULL;
    eval(t, var, val);
    recFree(val);
    nTree->nodes[nTree->nodesLen-1] = t;

    for(int j=0; j<bPts->nodesLen; j++) {
      nTree->nodesLen++;
      nTree->nodes = realloc(nTree->nodes, sizeof(t_syntaxTree *) * nTree->nodesLen);
      t = recCopy(tree);
      bp = recCopy(bPts->nodes[j]);
      sprintf(str, "%d", i+atoi(bp->nodes[bp->nodesLen-1]->nodeName));
      strcpy(bp->nodes[bp->nodesLen-1]->nodeName, str);
      eval(t, var, bp);
      recFree(bp);

      nTree->nodes[nTree->nodesLen-1] = t;
    }
  }

  recFree(bPts);
  return nTree;
}


void simplify(t_syntaxTree* t) {
  if (t->nodesLen != 0) {
    int simplified = 0;

    if (strcmp(t->nodeName, "and") == 0) {
      for(int i=0; i<t->nodesLen; i++) {
	if (strcmp(t->nodes[i]->nodeName, "false") == 0) {
	  simplified = 1;

	  for (int j=0; j<t->nodesLen; j++)
	    recFree(t->nodes[j]);

	  strcpy(t->nodeName, "false");
	  t->nodesLen = 0;
	  break;
	}
      }
    }

    if (strcmp(t->nodeName, "or") == 0) {
      for(int i=0; i<t->nodesLen; i++) {
	if (strcmp(t->nodes[i]->nodeName, "true") == 0) {
	  simplified = 1;

	  for (int j=0; j<t->nodesLen; j++)
	    recFree(t->nodes[j]);

	  strcpy(t->nodeName, "true");
	  t->nodesLen = 0;
	  break;
	}
      }
    }

    if (!simplified)
      for(int i=0; i<t->nodesLen; i++)
	simplify(t->nodes[i]);
  }
}


int recTreeToStr(t_syntaxTree* t, char** str, int len) {
  if (t->nodesLen == 0) {
    int nLen = len + strlen(t->nodeName);
    *str = realloc(*str, sizeof(char) * nLen);
    strcat(*str, t->nodeName);
    return nLen;
  }

  else {
    int nLen = len + strlen(t->nodeName) + 1;
    *str = realloc(*str, sizeof(char) * nLen);
    strcat(*str, "(");
    strcat(*str, t->nodeName);

    for (int i=0; i<t->nodesLen; i++) {
      nLen++;
      *str = realloc(*str, sizeof(char) * nLen);
      strcat(*str, " ");
      nLen = recTreeToStr(t->nodes[i], str, nLen);
    }

    nLen++; //nLen++;
    *str = realloc(*str, sizeof(char) * nLen);
    strcat(*str, ")");

    return nLen;
  }
}


char* treeToStr(t_syntaxTree* tree) {
  char* str=malloc(sizeof(char));
  str[0] = '\0';
  recTreeToStr(tree, &str, 1);
  return str;
}

void adjustForYices(t_syntaxTree* t) {
  for (int i=0; i<t->nodesLen; i++) {
    if (strcmp(t->nodes[i]->nodeName, "div") == 0) {
      t_syntaxTree* pt = t->nodes[i];
      strcpy(pt->nodeName, "mod");
      t->nodes[i] = malloc(sizeof(t_syntaxTree));
      strcpy(t->nodes[i]->nodeName, "=");
      t->nodes[i]->nodesLen = 2;
      t->nodes[i]->nodes = malloc(sizeof(t_syntaxTree*) * 2);
      t->nodes[i]->nodes[0] = pt;
      t->nodes[i]->nodes[1] = malloc(sizeof(t_syntaxTree));
      strcpy(t->nodes[i]->nodes[1]->nodeName, "0");
      t->nodes[i]->nodes[1]->nodesLen = 0;
      t->nodes[i]->nodes[1]->nodes = NULL;
      continue;
    }
    adjustForYices(t->nodes[i]);
  }
}


char* cooperToStr(char* wff, char* var) {
  t_syntaxTree* tree, *minf, *f;
  char* str;

  tree = parse(wff, 1); //Genera l'albero sintattico a partire dalla stringa
  normalize(tree, var); //Trasforma l'albero di tree
  //printf("\nNormalizzato %s\n\n", treeToStr(tree));
  minf = minInf(tree, var); //Restituisce l'albero di $\varphi_{- \infty}$
  //printf("\nMininf %s\n\n", treeToStr(minf));
  //printf("\nbPts %s\n\n", treeToStr(boundaryPoints(tree, var)));
  f = newFormula(tree, minf, var); //Restituisce la formula equivalente
  //printf("\nFormula equivalente %s\n\n", treeToStr(f));
  simplify(f); //opzionale
  adjustForYices(f);
  str = treeToStr(f); //Genera la stringa a partire dall'albero

  recFree(tree); //Libera la memoria
  recFree(minf);
  recFree(f);

  //return "ciao";
  return str;
}


char** cooperToArray(char* wff, char* var, int* len) {
  t_syntaxTree* tree, *minf, *f;
  char* buffer;
  char** array;

  tree = parse(wff, 1); //Genera l'albero sintattico a partire dalla stringa
  normalize(tree, var); //Trasforma l'albero di tree
  minf = minInf(tree, var); //Restituisce l'albero di $\varphi_{- \infty}$
  f = newFormula(tree, minf, var); //Restituisce la formula equivalente
  simplify(f); //opzionale
  adjustForYices(f);

  *len = f->nodesLen;

  array = malloc(sizeof(char*) * *len);

  for (int i=0; i<*len; i++) {
    buffer = treeToStr(f->nodes[i]);
    array[i] = malloc(sizeof(char) * strlen(buffer));
    strcpy(array[i], buffer);
    free(buffer);
  }

  recFree(tree); //Libera la memoria
  recFree(minf);
  recFree(f);

  return array;
}
