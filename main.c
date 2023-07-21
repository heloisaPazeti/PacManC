#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "desenho.h"

#define SPEED 20
#define LIN 27
#define COL 57
#define WHITE 15
#define BLUE 1
#define RED 4
#define GREEN 2
#define YELLOW 14
#define CYAN 3
#define PURPLE 5
#define GREY 7

#define qtdeF 3
#define nomeSize 3
#define mapaLin 25
#define mapaCol 58

//////////////////// STRUCT DE POSICAO

typedef struct {
  int x;
  int y;
} posicao;

//////////////////// STRUCT DO JOGARDOR

typedef struct {
      char nome[4];
      int status;
      int movimento;
      int comidos;
      int invincible;
      int andou;
      int andouinicial;
      unsigned int pontos;
      posicao p;
} pacman;

//////////////////// STRUCT DO INIMIGO

typedef struct {
    int dir;
    int status;
    int movimento;
    char caractereAnterior;
    posicao pos;
} inimigo;

//////////////////// STRUCT DO HIGHSCORE

typedef struct {
  char nome[4];
  int point;
} highscore;

//////////////////// VARIAVEIS IMPORTANTES

pacman jogador;
inimigo fantasma[qtdeF];
char mapa[mapaLin][mapaCol];
int totalpontos;
int qtdeFantasmas = qtdeF;
int qtdeFastasmasMortos = 0;

//////////////////// INICIALIZANDO METODOS

void SetColor(int color);
void cls(HANDLE hConsole);
void LeMapa();
void Start();
void Feed();
void Frutinha();
void PowerUp();
void ExibeMapa();
void AtualizaMap(posicao newpos);
void ChangeGhostDirection(inimigo fant);
void LiberaFantasma();
void MoveFantasma();
void MovePac();
void Save();
int TamNome();
int Score();
int Highscore();
int Collision(posicao newpos);


//////////////////// MAIN

int main()
{
    setlocale(LC_ALL,"Portuguese");
    srand(time(NULL));
    int jogar=1;

    while(jogar){
        Start();
        ExibeMapa();
        MovePac();

        if(Score())
            Save();

        jogar = Highscore();
    }
    return 0;
}

//////////////////// INICIALIZA JOGO

void Start(){
    int size;
    int i, j;
    jogador.pontos = 0;

    ////////////////////////////////////////// LE CARACTERE A CARACTERE DO MAPA COM SUAS CORES
    for (i = 0; i < LIN; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if((i==0 || i==LIN-1) || (j==0 || j==COL-1))
                SetColor(BLUE);
            else
                SetColor(YELLOW);

            printf("%c",inicio[i][j]);

            if (j == COL-1)
                printf("\n");
        }
    }

    SetColor(WHITE);
    printf("Insira seu apelido (coloque 3 letras): ");
    fgets(jogador.nome, 4, stdin);

    size = TamNome();

    /////////////////////////// SE CERTIFICA QUE O NOME TEM 3 CARACTERES
    while(1)
    {
        if(size == nomeSize)
            break;

        printf("Insira seu apelido (coloque 3 letras): ");
        fgets(jogador.nome, 4, stdin);
        size = TamNome();
    }

    LeMapa();
    cls( GetStdHandle( STD_OUTPUT_HANDLE ));
}

//////////////////// LE O MAPA NO ARQ.TXT E INICIALIZA FANTASMAS E JOGADOR

void LeMapa(){
    int i,j,k;
    FILE *fp;
    k=0;
    totalpontos=0;

    fp = fopen("mapajogo.txt", "rt");

    if(fp == NULL)
        printf("Erro, nao foi possivel abrir o arquivo\n");
    else
    {
        for(i = 0; i < mapaLin; i++)
        {
            for(j = 0; j < mapaCol; j++)
            {
                fscanf(fp,"%c", &mapa[i][j]);
                if(mapa[i][j] == 'C')
                {
                    jogador.p.x = j;
                    jogador.p.y = i;
                    jogador.pontos=0;
                    jogador.comidos=0;
                    jogador.status=-1;
                }

                if (mapa[i][j] == 'F')
                {
                    fantasma[k].pos.x = j;
                    fantasma[k].pos.y = i;
                    fantasma[k].dir = 0;
                    fantasma[k].status = 1;
                    fantasma[k].caractereAnterior = ' ';
                    k++;
                }
                if((mapa[i][j] == '.') || (mapa[i][j] == '$') || (mapa[i][j] == '@'))
                    totalpontos++;
            }
        }

    }
}

//////////////////// ATUALIZA MAPA

void AtualizaMap(posicao newpos){

    mapa[jogador.p.y][jogador.p.x] = ' ';  // LIMPA POSICAO ANTERIOR
    jogador.p.x = newpos.x;                // SALVA NO POSICAO
    jogador.p.y = newpos.y;
    mapa[jogador.p.y][jogador.p.x] = 'C';  // COLOCA CARACTERE NA NOVA POSICAO
}

//////////////////// EXIBE A TELA NOVA

void ExibeMapa(){

    int i,j;

    cls(GetStdHandle(STD_OUTPUT_HANDLE));
    Sleep(SPEED);

    for(i = 0; i < mapaLin; i++)
    {
        for(j = 0; j < mapaCol; j++)
        {
            switch(mapa[i][j])
            {
                case 'C':
                {
                    SetColor(YELLOW);
                    break;
                }
                case '0':
                {
                    SetColor(BLUE);
                    break;
                }
                case 'X':
                {
                    SetColor(GREY);
                    break;
                }
                case 'F':
                {
                    SetColor(PURPLE);
                    break;
                }
                case '@':
                {
                    SetColor(GREEN);
                    break;
                }
                case '$':
                {
                    SetColor(RED);
                    break;
                }
                default:
                {
                    SetColor(WHITE);
                    break;
                }
            }

            printf("%c",mapa[i][j]);

        }
    }

    printf("\n\n");
    printf("SCORE: %d",jogador.pontos);

    if(jogador.invincible==1)
        printf("          POWER-UP ESTÁ ATIVO %d\n\n",26-jogador.andou);
    else
        printf("          POWER-UP ESTÁ INATIVO\n\n");

    SetColor(GREEN);
    printf(" @ - POWER-UP     ");

    SetColor(RED);
    printf(" $ - VITAMINA \n");

    SetColor(PURPLE);
    printf(" F - FANTASMA     ");

    SetColor(YELLOW);
    printf(" C - PACMAN");

}

//////////////////// MOVIMENTO JOGADOR

void MovePac(){
    char tecla;
    int canGo = 0;
    posicao newP;

    while (1)
    {
        newP.x = jogador.p.x;
        newP.y = jogador.p.y;

        if (_kbhit()) {
            tecla = _getch();

        if (tecla == 'w' || tecla == 'W') // Up
        {
            newP.y--;
            canGo = Collision(newP);
        }

        if (tecla == 's' || tecla == 'S') // Down
        {
            newP.y++;
            canGo = Collision(newP);
        }

        if (tecla == 'a' || tecla == 'A') // Left
        {
            newP.x--;
            canGo = Collision(newP);
        }

        if (tecla == 'd' || tecla == 'D') // Right
        {
            newP.x++;
            canGo = Collision(newP);
        }

        if(jogador.comidos == totalpontos)
            jogador.status = 1;

        if(canGo == 1)
        {
            AtualizaMap(newP);
            jogador.andou++;
        }

        if(jogador.invincible == 1)
            if((jogador.andou - jogador.andouinicial) == 26)
                jogador.invincible=0;


        if(jogador.andou == 5)
            LiberaFantasma();

        if(jogador.status == 0 || jogador.status == 1)
        {
            Sleep(2000);
            break;
        }

        MoveFantasma();
        ExibeMapa();
    }
  }
}

//////////////////// CALCULA SE PLAYER PODE SE MOVER OU COM O QUE VAI COLIDIR

int Collision(posicao newpos){
    int i;
    int colisao;

  ///////////////////////////////////////////// CHECA COLISAO COM PAREDE

    if (mapa[newpos.y][newpos.x] == '0'  ||  mapa[newpos.y][newpos.x] == 'X')
        colisao = 0;
    else
        colisao = 1;

  //////////////////////////////////////////// CHECA COLISAO COM PONTO/FRUTA/POWERUP

    if (mapa[newpos.y][newpos.x] == '.')
        Feed();
    if(mapa[newpos.y][newpos.x] == '$')
        Frutinha();
    if(mapa[newpos.y][newpos.x] == '@')
        PowerUp();

  /////////////////////////////////////////// CHECA COLISAO COM FANTASMA

    if (mapa[newpos.y][newpos.x] == 'F')
    {
        if(jogador.invincible == 0 && qtdeFastasmasMortos < qtdeFantasmas){
            jogador.status = 0;
            mapa[jogador.p.y][jogador.p.x] = 'F';
        }
        else
        {
            for(i = 0; i < qtdeFantasmas; i++)
            {
                if(newpos.y==fantasma[i].pos.y && newpos.x==fantasma[i].pos.x)
                {
                    fantasma[i].status=0;
                    fantasma[i].pos.y=100;
                    fantasma[i].pos.x=100;
                    qtdeFastasmasMortos++;
                    jogador.pontos+=100;
                    break;
                }
            }
        }
   }

  return colisao;
}

//////////////////// BOLINHAS/FRUTINHAS/POWERUP

void Feed(){
    jogador.pontos+=5;
    jogador.comidos++;
}

void Frutinha(){
    jogador.pontos+=10;
    jogador.comidos++;
}

void PowerUp(){
    jogador.invincible=1;
    jogador.andou=0;
    jogador.andouinicial=jogador.andou;
    jogador.comidos++;
}

//////////////////// MOVIMENTA FANTASMAS

void MoveFantasma() {
  int i;
  for(i = 0; i < qtdeFantasmas; i++)
  {
    posicao newPosition;
    newPosition.x=fantasma[i].pos.x;
    newPosition.y=fantasma[i].pos.y;

    fantasma[i].dir = rand() % 4;

    switch(fantasma[i].dir)
    {
      case 0:
      {
        newPosition.y++;
        break;
      }
      case 1:
      {
        newPosition.y--;
        break;
      }
      case 2:
      {
        newPosition.x--;
        break;
      }
      case 3:
      {
        newPosition.x++;
        break;
      }
    }

    if(mapa[newPosition.y][newPosition.x] == 'C' && jogador.invincible == 0 && qtdeFastasmasMortos < qtdeFantasmas)
        jogador.status=0;

    if(mapa[newPosition.y][newPosition.x] == 'C' && jogador.invincible == 1)
    {
        mapa[fantasma[i].pos.y][fantasma[i].pos.x] = ' ';
        jogador.pontos+=100;
        fantasma[i].status=0;
        qtdeFastasmasMortos++;
    }

    if(fantasma[i].status)
    {

        if((mapa[newPosition.y][newPosition.x] == '0') ||  (mapa[newPosition.y][newPosition.x] == 'X') ||  (mapa[newPosition.y][newPosition.x] == 'F'))
        {
            ChangeGhostDirection(fantasma[i]);
            i--;
        }
        else
        {
            mapa[fantasma[i].pos.y][fantasma[i].pos.x] = fantasma[i].caractereAnterior;
            fantasma[i].pos.x = newPosition.x;
            fantasma[i].pos.y = newPosition.y;
            fantasma[i].caractereAnterior = mapa[fantasma[i].pos.y][fantasma[i].pos.x];

            if(fantasma[i].caractereAnterior == 'C')
              fantasma[i].caractereAnterior = ' ';

            mapa[fantasma[i].pos.y][fantasma[i].pos.x] = 'F';
        }
    }
  }
}

//////////////////// MUDAR DIRECAO DOS FANTASMAS

void ChangeGhostDirection(inimigo fant){
  int newDir = rand() % 4;

  if(newDir != fant.dir)
    fant.dir = newDir;
}

//////////////////// ABRE "CASINHA" DOS FANTASMAS

void LiberaFantasma() {
  int i, j;
  for(i = 0; i < mapaLin; i++)
  {
     for(j = 0; j < mapaCol; j++)
     {
       if(mapa[i][j] == 'X')
         mapa[i][j] = ' ';
     }
  }
}

//////////////////// CALCULA SCORE

int Score(){
    int gamesave = -1;

    if(jogador.status == 0)
       printf("          VOCÊ PERDEU!\n\n");
    if(jogador.status == 1)
        printf("VOCÊ VENCEU! PARABÉNS :^D\n\n");

    printf("VOCÊ OBTEVE UM TOTAL DE %d PONTOS!!!",jogador.pontos);

    printf("GOSTARIA DE SALVAR SEU SCORE E SEU MAPA? (Não - 0)(Sim - 1)");

    while((gamesave != 0) && (gamesave != 1))
    {
        fflush(stdin);
        scanf("%d",&gamesave);
    }

    return gamesave;
}

//////////////////// FAZ O SAVE

void Save(){
    int i,j;

    FILE *fp;
    fp=fopen("SCORES.txt","at");
    if(fp == NULL)
        printf("Erro, nao foi possivel salvar o score\n");
    else
    {
        fprintf(fp,"\n");
        fprintf(fp,"%s-%d",jogador.nome,jogador.pontos);
    }

    fclose(fp);
    fp = fopen("GAMEMAP.txt","wt");

    if(fp == NULL)
        printf("Erro, nao foi possivel salvar o mapa\n");
    else
    {
       for(i = 0; i < mapaLin; i++)
        {
            for(j = 0; j < mapaCol; j++)
                fprintf(fp,"%c",mapa[i][j]);
        }
    }


    fclose(fp);
    printf("Jogo salvo! Pressione qualquer tecla para continuar\n");

    while(1)
    {
        if(_kbhit())
        {
            fflush(stdin);
            break;
        }
    }

}

//////////////////// CALCULA O HIGHSCORE

int Highscore() {
    FILE *fp;
    highscore *pontos;
    int linhas,i,j;
    int aux;
    char str_aux[4];
    char c;
    char c_ponto[10];
    char opcao;

    fp=fopen("SCORES.txt","rt");

    cls(GetStdHandle(STD_OUTPUT_HANDLE));
    linhas=0;

    if(fp == NULL)
        printf("Erro\n");
    else
    {

        while( (c=fgetc(fp))!=EOF)
        {
            if(c=='\n')
                linhas++;
        }

        linhas++;

        fclose(fp);
        fp=fopen("SCORES.txt","rt");
        pontos = malloc(linhas*sizeof(highscore));

        for(i=0;i<linhas;i++)
        {
            fgets(pontos[i].nome,4,fp);
            fgetc(fp);
            fgets(c_ponto,10,fp);
            pontos[i].point=atoi(c_ponto);
        }

        for (i = linhas - 1; i > 0; i--)
        {
            for (j = 0; j < i; j++)
            {

                if (pontos[j].point < pontos[j + 1].point)
                {
                    aux = pontos[j].point;
                    pontos[j].point = pontos[j + 1].point;
                    pontos[j + 1].point = aux;

                    strcpy(str_aux,pontos[j].nome);
                    strcpy(pontos[j].nome,pontos[j + 1].nome);
                    strcpy(pontos[j + 1].nome,str_aux);
                }
            }
        }
        SetColor(BLUE);
        printf("PONTUAÇÕES DOS 5 MELHORES JOGADORES");
        SetColor(WHITE);
        if(linhas>=5){
            for(i=0;i<5;i++){
                printf("\n\n");
                printf("        %s  -  %d",pontos[i].nome,pontos[i].point);
                printf("\n");
            }
        }
        else
        {
            for(i=0;i<linhas;i++){
                printf("\n\n");
                printf("        %s  -  %d",pontos[i].nome,pontos[i].point);
                printf("\n");
            }
        }

    }

    fclose(fp);
    getchar();
    printf("\n");
    while(opcao!='N' && opcao!='S'){
        printf("Você gostaria de jogar novamente? (S - Sim) (N - Não)");
        fflush(stdin);
        scanf("%c",&opcao);
        fflush(stdin);
    }
    if(opcao=='S'){
        cls(GetStdHandle(STD_OUTPUT_HANDLE));
        jogador.andou=0;
        return 1;
    }
    if(opcao=='N'){
        return 0;
    }
    return 0;
}

////////////////////////////////////////////////////////// HELPERS

//////////////////// TROCAR A COR DOS PRINT

void SetColor(int color){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    switch(color)
    {
        case BLUE:
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            break;

        case RED:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
            break;

        case GREEN:
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            break;

        case YELLOW:
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
            break;

        case PURPLE:
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
            break;

        case WHITE:
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            break;

        case GREY:
            SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
            break;

        default: break;
    }
}

//////////////////// PEGA O TAMANHO DO NOME DO JOGADOR

int TamNome() {
    int tamanho;
    tamanho = 0;

    while(jogador.nome[tamanho]!='\0' && jogador.nome[tamanho]!=' ' && jogador.nome[tamanho]!='\n' && jogador.nome[tamanho]!=EOF)
        tamanho++;

    return tamanho;
}

//////////////////// FUNCAO QUE LIMPA A TELA

void cls(HANDLE hConsole){

  COORD coordScreen = {0, 0}; // home for the cursor
  DWORD cCharsWritten;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD dwConSize;

  // Get the number of character cells in the current buffer.
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    return;
  }

  dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

  // Fill the entire screen with blanks.
  if (!FillConsoleOutputCharacter(
          hConsole,        // Handle to console screen buffer
          (TCHAR)' ',      // Character to write to the buffer
          dwConSize,       // Number of cells to write
          coordScreen,     // Coordinates of first cell
          &cCharsWritten)) // Receive number of characters written
  {
    return;
  }

  // Get the current text attribute.
  if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
    return;
  }

  // Set the buffer's attributes accordingly.
  if (!FillConsoleOutputAttribute(
          hConsole,         // Handle to console screen buffer
          csbi.wAttributes, // Character attributes to use
          dwConSize,        // Number of cells to set attribute
          coordScreen,      // Coordinates of first cell
          &cCharsWritten))  // Receive number of characters written
  {
    return;
  }

  // Put the cursor at its home coordinates.
  SetConsoleCursorPosition(hConsole, coordScreen);
}
