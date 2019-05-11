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


t_syntaxTree* buildTree(int first, char** tokens) { //first è l'indice del token attualmente
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
    open = 1; //Per ora abbiamo incontrato una parentesi aperta i.e. siamo al primo livello
              //di annidamento 
    
    do { //Itero sui figli del nodo attuale
      first++; //Passo al token successivo
      
      if (open == 1 && tokens[first][0] != ')') { //Finchè il token non è ")"
        //Chiamo ricorsivamente buildTree per allocare i figli del nodo
        tree->nodesLen++;
        tree->nodes = realloc(tree->nodes, sizeof(t_syntaxTree*) * tree->nodesLen);
        tree->nodes[tree->nodesLen-1] = buildTree(first, tokens);
      }
      
      if (tokens[first][0] == '(') open++; //Se incontro "("
      
      if (tokens[first][0] == ')') open--; //Se incontro ")"
    } while (open != 0); //Fintanto che non torno al livello 0 di annidamento
  }
  
  else { //Se il primo token incontrato non è "(" allora ...
    strcpy(tree->nodeName, tokens[first]);
    tree->nodesLen = 0; //... il nodo allocato allocato non avrà figli
    tree->nodes = NULL;
  }
  
  return tree;
}

int isNumber(char* str) { //Controlla che str appartenga a [1-9]+[0-9]*
  return !(atoi(str) == 0 && str[0] != '0');
}

void checkTree(t_syntaxTree* tree) { //Controlla che l'albero costruito soddisfi
                                     //la forma richiesta dall'algoritmo
  if (strcmp(tree->nodeName, "and")) //La radice deve essere "and"
    ERROR("Expression error: the main expression is not an 'and'");
  if (tree->nodesLen == 0) { //La radice "and" deve avere almeno un figlio
    ERROR("Expression error: the main 'and' expression \
must have at least one child");
  }
  for (int i=0; i<tree->nodesLen; i++) { //Scorro sui figli della radice ("and")
    if (strcmp(tree->nodes[i]->nodeName, "=") && //Se un figlio non è "="
        strcmp(tree->nodes[i]->nodeName, ">") && //o ">"
        strcmp(tree->nodes[i]->nodeName, "div")) //o "div"
      ERROR("Expression error: the children of the main 'and' \
can only be '=', '>' or 'div'");
    
    if (tree->nodes[i]->nodesLen != 2) //"=", ">" e "div" devono avere due figli
      ERROR("Expression error: '=', '>', 'div' must have two children");
    if (strcmp(tree->nodes[i]->nodes[0]->nodeName, "+")) //Il primo figlio di "=", ">" e
                                                         //"div" deve essere "+" mentre ...
      ERROR("Expression error: the first child of '=', '>' or 'div' \
must be '+'");
    if (!isNumber(tree->nodes[i]->nodes[1]->nodeName)) //...il secondo un numero
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


t_syntaxTree* parse(char* wff, int strict) { //Funzione principale di parsing
  char* wffSpaced = malloc(sizeof(char)); //Sarà la stringa come la sringa in ingresso ma
                                          //con spazi tra i token 
  wffSpaced[0] = wff[0];
  int j = 1;

  for (int i = 1; i < strlen(wff) + 1; i++) { //Scorro sui caratteri della stringa in
                                              //ingresso 
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
  
  char* token; //Stringa di supporto contenente un singolo token
  int nTokens = 1; //Numero dei token
  char** tokens = malloc(sizeof(char *)); //Array dei tokens
  tokens[0] = strtok(wffSpaced, " "); //Inserisco il primo token
  
  while ((token = strtok(NULL, " ")) != NULL) { //Finchè ce ne sono ne aggiungo
    nTokens++;
    tokens = realloc(tokens, sizeof(char *) * nTokens);
    tokens[nTokens - 1] = token;
  }
  
  int countPar = 0; //Contatore delle partentesi aperte incontrate finora
  
  for(int i=0; i<nTokens; i++) { //Scorro sui tokens
    for(int j=0; j<strlen(tokens[i]); j++) //Scorro sul singolo token
      if(tokens[i][j] == ')' && j!= 0) //Se ")" non è il primo carattere di un token
        ERROR("Parsing error: every S-expression must \
have a root and at least an argument");
    if (tokens[i][0] == '(') countPar++; //Incremento il contatore
    if (tokens[i][0] == ')') countPar--; //Decremento il contatore
  }
  
  if (countPar != 0) //Alla fine ogni parentesi aperta deve essere stata chiusa
    ERROR("Parsing error: the number of parentheses is not even");
  
  t_syntaxTree* syntaxTree = buildTree(0, tokens);
  
  if (strict) checkTree(syntaxTree); //chiama exit() se l'albero non va bene,
                                     //viene effettuato solo se strict != 0
  
  free(wffSpaced);
  free(tokens);
  
  return syntaxTree;
}


int getLCM(t_syntaxTree* tree, char* var) { //Restituisce il minimo comune multiplo dei
                                            //coefficienti di var che appaiaono in tutto
                                            //l'albero 
  if (tree->nodeName[0] == '*') { //Se sono in un nodo "*" e ...
    if (strcmp(((t_syntaxTree *)tree->nodes[1])->nodeName, var) == 0) {
      //... se il secondo figlio è var (non può mai essere il primo!)
      return atoi(((t_syntaxTree *) tree->nodes[0])->nodeName); //ritorno il coefficiente
    }
  }
  
  int l = 1; //Inizializzo a 1 nel caso il nodo attuale sia senza figli
  
  for(int i=0; i<tree->nodesLen; i++) { //Scorro sui figli e ...
    l = lcm(l, getLCM((t_syntaxTree *) tree->nodes[i], var)); //... scarico ricorsivamente                                                          //ricorsivamente
  }

  return l;
}


void normalize(t_syntaxTree* tree, char* var) { //Modifica i coefficienti della variabili
                                                //trasformandoli in 1 o -1
  int lcm = getLCM(tree, var);
  int c = lcm;
  
  for (int i=0; i<tree->nodesLen; i++) { //Scorro tra i figli della radice
    if (strcmp("=", tree->nodes[i]->nodeName) == 0 || //Se sono in un "=" o ...
        strcmp("div", tree->nodes[i]->nodeName) == 0) { //... in un "div"
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;
      
      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro tra gli addendi
        if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) 
          c = atoi(addends[j]->nodes[0]->nodeName); //c è il coefficiente della variabile 
      }
      
      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro fra gli addendi
        if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) { //Se sono sulla variabile
          strcpy(addends[j]->nodeName, var); //In questo modo il coefficiente è 1
          free(addends[j]->nodes[0]);
          free(addends[j]->nodes[1]);
          addends[j]->nodesLen = 0;
        }
        else { //Se non sono sulla variabile 
          sprintf(addends[j]->nodes[0]->nodeName,
                  "%d",
                  atoi(addends[j]->nodes[0]->nodeName)*lcm/c); //Ricalcolo i coefficienti
        }
      }

      //Ricalcolo anche i coefficienti del termine costante a secondo membro di "=" o "div"
      sprintf(tree->nodes[i]->nodes[1]->nodeName,
	      "%d",
	      atoi(tree->nodes[i]->nodes[1]->nodeName)*lcm/c);
    }

    else if (strcmp(">", tree->nodes[i]->nodeName) == 0) { //Se invcece sono su un ">"
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;

      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro sugli addendi
        if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) {
          c = atoi(addends[j]->nodes[0]->nodeName); //Mi segno il coefficiente di var
        }
      }
      
      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro sugli addendi
        if (strcmp(addends[j]->nodes[1]->nodeName, var) == 0) { //Se sono sulla variabile
          if (c>0) strcpy(addends[j]->nodeName, ""); //Se il coefficiente è positivo non
                                                     //faccio niente
          else strcpy(addends[j]->nodeName, "-"); //Altrimenti lo cambio di segno
          strcat(addends[j]->nodeName, var);
          free(addends[j]->nodes[0]);
          free(addends[j]->nodes[1]);
          addends[j]->nodesLen = 0;
        }
        else { //Se non sono sulla variabile
          sprintf(addends[j]->nodes[0]->nodeName,
                  "%d",
                  atoi(addends[j]->nodes[0]->nodeName)*abs(lcm/c)); //Ricalcolo i
                                                                    //coefficienti
        }
      }

      //Ricalcolo anche i coefficienti del secondo membro di ">"
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


void recFree(t_syntaxTree* tree) { //Dealloca ricorsivamente tutto l'albero
  for (int i=0; i<tree->nodesLen; i++) {
    recFree(tree->nodes[i]);
  }

  free(tree->nodes);
  free(tree);
}


t_syntaxTree* recCopy(t_syntaxTree* tree) { //Effettua una copia di un albero
                                            //ricorsivamente, ovvero riallocando tutto
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


t_syntaxTree* minInf(t_syntaxTree* tree, char* var) { //Calcola $\varphi_{- \infty}$
  t_syntaxTree* nTree = recCopy(tree); //La funzione lavora su una copia dell'albero

  char minvar[16]; //La variabile con "-" davanti
  minvar[0] = '\0';
  strcpy(minvar, "-");
  strcat(minvar, var);

  for (int i=0; i<nTree->nodesLen; i++) { //Scorro sui figli della radice
    if (strcmp(">", nTree->nodes[i]->nodeName) == 0) { //Se sono in un ">"
      t_syntaxTree** addends = nTree->nodes[i]->nodes[0]->nodes;
      
      for (int j=0; j<nTree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro sugli addendi
        if (strcmp(addends[j]->nodeName, var) == 0) //Se l'addendo contiene la variabile
          strcpy(nTree->nodes[i]->nodeName, "false"); //L'addendo diventa "false"
        else if (strcmp(addends[j]->nodeName, minvar) == 0) //Se l'addendo contiene la
                                                            //variabile col segno "-"
          strcpy(nTree->nodes[i]->nodeName, "true"); //L'addendo diventa "true"
      }
      
      for (int j=0; j<nTree->nodes[i]->nodesLen; j++) //Scorro sui figli dei figli della
                                                      //radice

        recFree(nTree->nodes[i]->nodes[j]); //E dealloco tutto
      
      free(nTree->nodes[i]->nodes);
      nTree->nodes[i]->nodesLen = 0;
      nTree->nodes[i]->nodes = NULL;
    }
    
    else if (strcmp("=", nTree->nodes[i]->nodeName) == 0) { //Se sono in un "="
      for (int j=0; j<nTree->nodes[i]->nodesLen; j++) //Scorro sui figli ...
        recFree(nTree->nodes[i]->nodes[j]); //... e dealloco tutto
      
      free(nTree->nodes[i]->nodes);
      nTree->nodes[i]->nodesLen = 0;
      nTree->nodes[i]->nodes = NULL;
      strcpy(nTree->nodes[i]->nodeName, "false"); //I nodi possono diventare solo "false"
    }
  }
  
  return nTree;
}


//Sostituisce val al posto di var nell'albero, non effettua davvero una valutazione!
void eval(t_syntaxTree* tree, char* var, t_syntaxTree* val) {
  for (int i=0; i<tree->nodesLen; i++) { //Scorro sui figli di tree
    if (strcmp(tree->nodes[i]->nodeName, var) == 0) { //Nel caso trovi var
      recFree(tree->nodes[i]); //Dealloco e ...
      tree->nodes[i] = recCopy(val); //... sostituisco con una copia di val
    }
    else { //Nel caso non trovi var potrei comunque ancora trovare var con "-" davanti
      char mvar[17] = "-"; //E' var con "-" davanti
      strcat(mvar, var);
      if (strcmp(tree->nodes[i]->nodeName, mvar) == 0) { //Se trovo var con "-" davanti
        recFree(tree->nodes[i]); //Dealloco tutto
        //Creo un nuovo nodo "-" contenente val come unico figlio
        tree->nodes[i] = malloc(sizeof(t_syntaxTree));
        strcpy(tree->nodes[i]->nodeName, "-");
        tree->nodes[i]->nodesLen = 1;
        tree->nodes[i]->nodes = malloc(sizeof(t_syntaxTree*));
        tree->nodes[i]->nodes[0] = recCopy(val);
      }
    }
    
    eval(tree->nodes[i], var, val); //Scarico ricorsivamente su tutti i figli
  }
}


int calcm(t_syntaxTree* tree, char* var) { //Calcolo il minimo comune multiplo di tutti i
                                           //coefficienti della variabile
  int m=1;
  
  for(int i=0; i<tree->nodesLen; i++) { //Scorro sui figli della radice
    if(strcmp(tree->nodes[i]->nodeName, "div") == 0) { //Se sono in un "div"
      
      if(strcmp(tree->nodes[i]->nodes[0]->nodeName, var) == 0) //Se trovo la variabile senza
                                                               //coefficiente (i.e. 1 o -1)
        m = lcm(m, atoi(tree->nodes[i]->nodes[1]->nodeName)); //Calcolo il m.c.m.
      
      else if(strcmp(tree->nodes[i]->nodes[0]->nodeName, "+") == 0) { //Altrimenti se trovo
                                                                      //un "+"
        for(int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Sorro tra gli addendi
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


t_syntaxTree* boundaryPoints(t_syntaxTree* tree, char* var) { //Calcolo dei bounday points
  char str[16];
  str[0] = '\0';
  t_syntaxTree* bPoints = malloc(sizeof(t_syntaxTree)); //Salvo i boundary points in un
                                                        //albero
  bPoints->nodes = NULL;
  strcpy(bPoints->nodeName, "bPoints"); //Nome utile solo per il debugging
  bPoints->nodesLen = 0;

  for(int i=0; i<tree->nodesLen; i++) { //Scorro sui figli della radice
    if (strcmp(tree->nodes[i]->nodeName, "=") == 0) { //Se sono in un "="
      t_syntaxTree** addends = tree->nodes[i]->nodes[0]->nodes;
      
      for (int j=0; j<tree->nodes[i]->nodes[0]->nodesLen; j++) { //Scorro sugli addendi
        if (strcmp(var, addends[j]->nodeName) == 0) { //Se trovo la variabile
          bPoints->nodesLen++; //Aggiungo un boundary point
          bPoints->nodes = realloc(bPoints->nodes,
                                   sizeof(t_syntaxTree *) * bPoints->nodesLen);
          t_syntaxTree* bp = malloc(sizeof(t_syntaxTree));
          bp->nodes = NULL;
          strcpy(bp->nodeName, "+");
          bp->nodesLen = 0;
          
          for (int k=0; k<tree->nodes[i]->nodes[0]->nodesLen; k++) { //Scorro sugli addendi
            if (strcmp(var, addends[k]->nodeName) != 0) { //Se non sono sulla variabile
              bp->nodesLen++; //Aggiungo comunque
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
          bPoints->nodes = realloc(bPoints->nodes,
                                   sizeof(t_syntaxTree *) * bPoints->nodesLen);
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


//Calcola la formula equivalente
t_syntaxTree* newFormula(t_syntaxTree* tree, t_syntaxTree* minf, char* var) {
  int m = calcm(minf, var); //Minimo comune multiplo dei coefficienti di var
  t_syntaxTree* val;
  char str[16];
  t_syntaxTree* nTree = malloc(sizeof(t_syntaxTree)); //Nuovo albero che verrà restituito
  strcpy(nTree->nodeName, "or"); //La radice del nuovo albero è un "or"
  nTree->nodesLen = 0;
  nTree->nodes = NULL;

  t_syntaxTree* t;
  t_syntaxTree* bp;
  t_syntaxTree *bPts = boundaryPoints(tree, var); //Calcola i boundary points

  //La seguente è semplicemente una applicazione della formula del teorema
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

  recFree(bPts); //Non servono più, dealloco
  return nTree;
}


void simplify(t_syntaxTree* t) { //Semplifica l'albero (non è la migliore semplicifcazione)
  if (t->nodesLen != 0) { //Semplifico solo se il nodo attuale ha figli
    int simplified = 0; //E' 0 se non ho effettuato semplificazioni (valore inizializzato)
    
    if (strcmp(t->nodeName, "and") == 0) { //Se il nodo attuale è un "and"
      for(int i=0; i<t->nodesLen; i++) { //E se scorrendo tra i figli ...
        if (strcmp(t->nodes[i]->nodeName, "false") == 0) { //... trovo un "false"
          simplified = 1; //Mi segno che ho effettuato una semplificazione
          
          for (int j=0; j<t->nodesLen; j++) //Dealloco tutto ...
            recFree(t->nodes[j]);
          
          strcpy(t->nodeName, "false"); //... e il nodo "and" diventa un "false"
          t->nodesLen = 0;
          break;
        }
      }
    }
    
    if (strcmp(t->nodeName, "or") == 0) { //Se il nodo attuale è un "or"
      for(int i=0; i<t->nodesLen; i++) { //E se scorrendo tra i figli ...
        if (strcmp(t->nodes[i]->nodeName, "true") == 0) { //... trovo un "true"
          simplified = 1; //Mi segno che ho effettuato una semplificazione
          
          for (int j=0; j<t->nodesLen; j++) //Dealloco tutto ...
            recFree(t->nodes[j]);
          
          strcpy(t->nodeName, "true"); //... e il nodo "or" diventa un "true"
          t->nodesLen = 0;
          break;
        }
      }
    }
    
    if (!simplified) //Se non ho effettuato semplificazioni ...
      for(int i=0; i<t->nodesLen; i++)
        simplify(t->nodes[i]); //... tento di semplificare i figli del nodo attuale
  }
}


int recTreeToStr(t_syntaxTree* t, char** str, int len) { //Trasforma un albero sintattico in
                                                         //una stringa
  if (t->nodesLen == 0) { //Se il nodo attuale non ha figli
    int nLen = len + strlen(t->nodeName); //Aggiorna la lunghezza della stringa prodotta
    *str = realloc(*str, sizeof(char) * nLen); //Rialloca la stringa
    strcat(*str, t->nodeName); //Aggiungi alla stringa il nome del nodo
    return nLen;
  }
  
  else { //Se invece il nodo attuale ha figli
    int nLen = len + strlen(t->nodeName) + 1; //Aggiorna la lunghezza della stringa prodotta
                                              //considerando anche la parentesi "("
    *str = realloc(*str, sizeof(char) * nLen); //Rialloca la stringa
    strcat(*str, "("); //Aggiungi alla stringa "(" ...
    strcat(*str, t->nodeName); //... e il nome del nodo
    
    for (int i=0; i<t->nodesLen; i++) { //Per ogni figlio del nodo attuale
      nLen++; //A causa dello spazio " "
      *str = realloc(*str, sizeof(char) * nLen); //Rialloca la stringa
      strcat(*str, " "); //Aggiungi lo spazio
      nLen = recTreeToStr(t->nodes[i], str, nLen); //Scarica ricorsivamente
    }
    
    nLen++; //A causa della parentesi ")"
    *str = realloc(*str, sizeof(char) * nLen); //Rialloca la stringa
    strcat(*str, ")"); //Aggiungi la parentesi ")"

    return nLen; //Ritorna quanti caratteri è diventata adesso, utile per la ricorsione
  }
}


char* treeToStr(t_syntaxTree* tree) { //Funzione inversa di parse(), in realtà è un wrapper
                                      //non ricorsivo di recTreeToStr()
  char* str=malloc(sizeof(char));
  str[0] = '\0'; //Le stringhe finiscono con '\0' in C
  recTreeToStr(tree, &str, 1);
  return str;
}

void adjustForYices(t_syntaxTree* t) { //Aggiusta gli alberi in modo che siano parsabili da
                                       //Yices o software equivalenti (e.g. z3)
  for (int i=0; i<t->nodesLen; i++) { //Per ogni figlio del nodo attuale
    if (strcmp(t->nodes[i]->nodeName, "div") == 0) { //Se trovo un "div" ...
      t_syntaxTree* pt = t->nodes[i];
      strcpy(pt->nodeName, "mod"); //... lo sostituisco con un "mod"
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


char* cooperToStr(char* wff, char* var) { //Elimina var dalla formula
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
