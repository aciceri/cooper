#include <stdlib.h>
#include <string.h>
#include <stdio.h> //da togliere

long int gcd(long int a, long int b) {
    return b == 0 ? a : gcd(b, a % b);
}

long int lcm(long int a, long int b) {
    return abs((a / gcd(a, b)) * b); //parentheses are important to reduce overflows
}

typedef struct t_syntaxTree {
    char nodeName[16];
    int nodesLen;
    struct t_syntaxTree** nodes;
} t_syntaxTree;

t_syntaxTree* buildTree(int first, char** tokens) {
    t_syntaxTree* tree = malloc(sizeof(t_syntaxTree));
    tree->nodes = NULL;
    int open; //open parentheses
    if (tokens[first][0] == '(') {
        first++;
        tree->nodesLen = 0;
        strcpy(tree->nodeName, tokens[first]);
        open = 1;
        do {
            first++;
            if (open == 1 && tokens[first][0]!=')') {
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

t_syntaxTree* parse(char* wff) {
    char* wffSpaced = malloc(sizeof(char));
    wffSpaced[0] = wff[0];
    int j = 1;
    for (int i = 1; i < strlen(wff) + 1; i++) {
        if (wff[i - 1] == '(') {
            wffSpaced = realloc(wffSpaced, sizeof(char) * (j + 2));
            wffSpaced[j] = ' ';
            wffSpaced[j+1] = wff[i];
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
    t_syntaxTree* syntaxTree = buildTree(0, tokens);

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
    int c;

    for (int i=0; i<tree->nodesLen; i++) {
        if (strcmp("=", tree->nodes[i]->nodeName) == 0 ||
                strcmp("div", tree->nodes[i]->nodeName) == 0) {
            t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

            for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
                if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0)
                    c = atoi(addends[j]->nodes[0]->nodeName); //variable coefficient
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

            sprintf(tree->nodes[i]->nodes[1]->nodeName,
                    "%d",
                    atoi(tree->nodes[i]->nodes[1]->nodeName)*lcm/c);
        }

        else if (strcmp(">", tree->nodes[i]->nodeName) == 0) {
            t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

            for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
                if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0)
                    c = atoi(addends[j]->nodes[0]->nodeName); //variable coefficient
            }

            for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) {
                if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) {
                    if(c > 0) strcpy(addends[j]->nodeName, "");
                    else strcpy(addends[j]->nodeName, "-");
                    strcat(addends[j]->nodeName, var);
                    free(addends[j]->nodes[0]);
                    free(addends[j]->nodes[1]);
                    addends[j]->nodesLen = 0;
                }
                else {
                    sprintf(addends[j]->nodes[0]->nodeName,
                            "%d",
                            atoi(addends[j]->nodes[0]->nodeName)*lcm/abs(c));
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
        else
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
    strcpy(bPoints->nodeName, "bPoints"); //useful only for debugging purposes
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

/*void simplify(t_syntaxTree* t) {
  if (t->nodesLen != 0) {
    for(int i=0; i<t->nodesLen; i++) {
      simplify
    }

    if (strcmp(t->nodeName, "and") == 0) {
      for(int i=0; i<t->nodesLen; i++) {
	if (strcmp(t->nodes[i]->nodeName, "false") == 0) {
	  recFree(t);
	  break;
	}
      }
    }

    //ramo or

    
  }
}*/

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

        nLen++;
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

char* cooper(char* wff, char* var) {

    t_syntaxTree* tree = parse(wff);

    //char* str;
    char* str = treeToStr(tree);
    printf("\n\n----Ingresso----\n%s", str);
    free(str);

    normalize(tree, var);
    str = treeToStr(tree);
    printf("\n\n----Normalizzato----\n%s", str);
    free(str);

    t_syntaxTree* minf = minInf(tree, var);
    str = treeToStr(minf);
    printf("\n\n----Minf----\n%s", str);
    free(str);

    t_syntaxTree* bPts = boundaryPoints(tree, var);
    str = treeToStr(bPts);
    printf("\n\n----Boundary points----\n%s", str);
    free(str);

    t_syntaxTree* f = newFormula(tree, minf, var);
    str = treeToStr(f);
    printf("\n\n----Formula eq----\n%s", str);
    free(str);

    //simplify(tree);
    
    recFree(tree);
    recFree(minf);
    recFree(bPts);
    recFree(f);

    return str;
}

