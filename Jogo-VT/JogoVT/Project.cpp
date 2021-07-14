#include <SDL.h> // BIBLIOTECA DE DESENVOLVIMENTO DE INTERFACE GRÁFICA
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "string"
#include <SDL_mixer.h>

#define BLOCOS 30 // QUANTIDADE DE BLOCOS

GLuint loadTexture(const std::string&fileName) // CARREGAR TEXTURAS
{
    SDL_Surface * imagem = IMG_Load(fileName.c_str()); // CARREGA IMAGEM
    SDL_DisplayFormatAlpha(imagem); // TRANSPARÊNCIA PARA A IMAGEM PNG ALPHA
    unsigned object(0); // CRIA TEXTURA
    glGenTextures(1, &object); // GERA TEXTURA
    glBindTexture(GL_TEXTURE_2D, object); // TIPO DE TEXTURA, 2D

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagem->w, imagem->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagem->pixels);

    SDL_FreeSurface(imagem);

    return object;
}


// A - ELEMENTO 1 - INIMIGO
// B - ELEMENTO 2 - PERSONAGEM OU OUTRO ELEMENTO

bool colisao(float Ax, float Ay, float Acomp, float Aalt, float Bx, float By, float Bcomp, float Balt)
{
    if(Ay + Aalt < By) return false;
    else if(Ay > By + Balt) return false;
    else if(Ax + Acomp < Bx) return false;
    else if(Ax > Bx + Bcomp) return false;

    return true; // RETORNA VERDADEIRO SE NÃO CAIR EM NENHUMA DAS VERIFICAÇÕES ACIMA.
}

struct Bloco
{
    float x;
    float y;
    float comp;
    float alt;
    bool vivo;
};

int main(int argc, char* args[]){


    SDL_Init(SDL_INIT_EVERYTHING); // SDL_INIT INICIA ALGUMA COISA, ESTÁ ESPECIFICADO PARA INICIAR TUDO (EVERYTHING) POR EXEMPLO, AUDIOS, VÍDEOS E ETC...

    SDL_Surface *icone;
    SDL_Surface *tela;

    Uint16 formato = AUDIO_S16SYS;

    Mix_OpenAudio(22050, formato, 2, 4096);

    Mix_Music *musica;

    musica = Mix_LoadMUS("fundo.mp3");

    icone = IMG_Load("icon.png");

    glDisable(GL_BLEND);

    // CONTROLE DE MEMÓRIA
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // TAMANHO DA JANELA - 600 EM 'X' 400 EM 'Y' E 32 BITS POR PIXEL, TIPO DE JANELA OPEN GL

    tela = SDL_SetVideoMode(600, 400, 32, SDL_OPENGL);

    SDL_WM_SetIcon(icone, NULL);

    // NOME E ICONE DA JANELA DO PROGRAMA
    SDL_WM_SetCaption("Jogo VT - ALGORITMOS", NULL);

    // COR DA JANELA

    glClearColor(1, 1, 1, 1);

    // ÁREA EXIBIDA

    glViewport(0, 0, 600, 400);

    // SOMBRA

    glShadeModel(GL_SMOOTH);

    // 2D

    glMatrixMode(GL_PROJECTION); // APLICAR A PROJEÇÃO NA MATRIZ ATUAL
    glLoadIdentity(); // FUNÇÃO PRÉ DEFINIDA PARA TRABALHAR COM DESENHO GEOMÉTRICO

    //3D
    glDisable(GL_DEPTH_TEST); // FUNÇÃO 3D DESABILITADA

    // USO DA IMAGEM
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // INICIO DA MÚSICA

    Mix_PlayMusic(musica, -1);

    bool executando = true; // EXECUTANDO DO TIPO BOOL (TRUE)

    SDL_Event eventos; // EVENTOS PARA CLICKS DO MOUSE OU DO TECLADO.

    // VARIÁVEIS DO PERSONAGEM

    float personX = 250;
    float personY = 350;
    float personComp = 55;
    float personAlt = 50;

    // PERSONAGEM PARADO

    bool esq = false, dir = false;

    unsigned int inimigo_textura = 0;
    inimigo_textura = loadTexture("inimigo.png");

    unsigned int person = 0;
    person = loadTexture("personagem.png");

    unsigned int terra = 0;
    terra = loadTexture("terra.png");


    // VARIÁVEIS DO INIMIGO

    float inimigoX = 50;
    float inimigoY = 90;
    float inimigoCA = 40;

    float velX = 0.07;
    float velY = 0.07;

    Bloco blocos[BLOCOS];

    // CRIAÇÃO DOS BLOCOS

    for(int i = 0, x = 4, y = 10; i < BLOCOS; i++, x += 61)
    {
        if(x > 560)
        {
            x = 4;
            y += 23;
        }

        blocos[i].x = x;
        blocos[i].y = y;
        blocos[i].comp = 60;
        blocos[i].alt = 20;
        blocos[i].vivo = true;
    }

    // LOOP DO JOGO

    while(executando)
    {
        //EVENTOS
        while(SDL_PollEvent(&eventos))
        {
            // FECHA COM O X DA JANELA
            if(eventos.type == SDL_QUIT) // SDL_QUIT É IGUAL AO X DA JANELA
            {
                executando = false;
            }

            if(eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE) // KEYUP = ALGUMA TECLA PRESSIONADA FOI SOLTA && SDLK_ESCAPE PARÂMETRO DA TECLA ESC
            {
                executando = false;
            }

            // ANDA OU NÃO

            if(eventos.type == SDL_KEYDOWN) // TECLA PRESSIONADA
            {
                if(eventos.key.keysym.sym == SDLK_LEFT)
                {
                    esq = true;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = true;
                }
            }
            else if(eventos.type == SDL_KEYUP) // TECLA SOLTA
            {
                if(eventos.key.keysym.sym == SDLK_LEFT) // SDLK_LEFT ESQUERDA
                {
                    esq = false;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT) // SDLK_RIGHT DIREITA
                {
                    dir = false;
                }
            }


        }
        // LÓGICA

            // MOVIMENTOS DO PERSONAGEM

            if(esq == true) // SE A TECLA ESQUERDA FOR PRESSIONADA
            {
                personX -= 0.1; // DECRESCER 0.1 PIXEL PARA A ESQUERDA X

            }
            else if(dir == true)
            {
                personX += 0.1; // ACRESCER 0.1 PIXEL PARA A DIREITA X

            }

            if(personX < 0)
            {
                personX = 0;
            }
            else if(personX + personComp > 600)
            {
                personX = 600 - personComp;
            }


            // MOVIMENTOS DO INIMIGO EM X

            inimigoX += velX;

            for(int i=0; i<BLOCOS; i++)
            {
               if(blocos[i].vivo == true)
               {
                   if(colisao(inimigoX, inimigoY, inimigoCA, inimigoCA, blocos[i].x, blocos[i].y, blocos[i].comp, blocos[i].alt))
                   {
                        velX = -velX;
                        blocos[i].vivo = false;
                        break;
                   }
               }
            }

            // MOVIMENTOS DO INIMIGO EM Y

            inimigoY += velY;

                        for(int i=0; i<BLOCOS; i++)
            {
               if(blocos[i].vivo == true)
               {
                   if(colisao(inimigoX, inimigoY, inimigoCA, inimigoCA, blocos[i].x, blocos[i].y, blocos[i].comp, blocos[i].alt))
                   {
                        velY = -velY;
                        break;
                   }
               }
            }

            // LÓGICA INIMIGO
            if(inimigoX < 0)
            {
                velX = -velX;
            }
            else if(inimigoX + inimigoCA > 600)
            {
                velX = -velX;
            }
            else if(inimigoY < 0)
            {
                velY = -velY;
            }
            else if(inimigoY + inimigoCA > 400)
            {

                executando = false;

            }
            if(colisao (inimigoX, inimigoY, inimigoCA, inimigoCA, personX, personY, personComp, personAlt) == true)
            {
                velY = -velY;
            }


        // RENDERIZAÇÃO
        glClear(GL_COLOR_BUFFER_BIT); // LIMPA O BUFFER

        // INICIA A MATRIZ
        glPushMatrix();

        // DIMENSÕES DA MATRIZ
        glOrtho(0, 600, 400, 0, -1, 1); // DEFINIR O TAMANHO DA MATRIZ ( 0 PONTO SUPERIOR ESQUERDO EM DIAGONAL, 600 PONTO SUPERIOR DIREITO EM DIAGONAL ) E ( 400 PONTO INFERIOR EM HORIZONTAL E 0 PONTO SUPERIOR EM HORIZONTAL )

        // COR
        //glColor3D ( 1 ou 0 )
        //glColor3f ( DECIMAIS )
        //glColor3ub ( R, G, B ) - 0 ATÉ 255
        //glColor4ub(R, G, B, A); 0 ATÉ 255


        // DESENHO DO PERSONAGEM

        glColor4ub(255, 255, 255, 255); // TRANSPARENTE/BRANCO

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, person);

        //INICIA O DESENHO
        glBegin(GL_QUADS); // GL_POINTS, GL_LINES, GL_LINES_LOOP, GL_QUADS, GL_TRIANGLES, GL_POLIGON

        glTexCoord2d(0, 0); glVertex2f(personX, personY); // PRIMEIRO PONTO X, Y
        glTexCoord2d(1, 0); glVertex2f(personX+personComp, personY); // SEGUNDO PONTO X, Y

        glTexCoord2d(1, 1); glVertex2f(personX+personComp, personY+personAlt);
        glTexCoord2d(0, 1); glVertex2f(personX, personY+personAlt);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        // FIM DO DESENHO DO PERSONAGEM

        // DESENHO DO INIMIGO

        glColor4ub(255, 255, 255, 255);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, inimigo_textura);

        glBegin(GL_QUADS); // DESENHO DO INIMIGO

        glTexCoord2d(0, 0); glVertex2f(inimigoX, inimigoY); // PRIMEIRO PONTO X, Y
        glTexCoord2d(1, 0); glVertex2f(inimigoX+inimigoCA, inimigoY); // SEGUNDO PONTO X, Y

        glTexCoord2d(1, 1); glVertex2f(inimigoX+inimigoCA, inimigoY+inimigoCA);
        glTexCoord2d(0, 1); glVertex2f(inimigoX, inimigoY+inimigoCA);

        glEnd();

        glDisable(GL_TEXTURE_2D);

        // FIM DO DESENHO DO INIMIGO

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, terra);

        glBegin(GL_QUADS);

        glColor4ub(255, 255, 255, 255);

        for(int i = 0; i < BLOCOS; i++)
        {
            if(blocos[i].vivo == true)
            {
                glTexCoord2d(0, 0); glVertex2f(blocos[i].x, blocos[i].y);
                glTexCoord2d(1, 0); glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y);
                glTexCoord2d(1, 1); glVertex2f(blocos[i].x + blocos[i].comp, blocos[i].y + blocos[i].alt);
                glTexCoord2d(0, 1); glVertex2f(blocos[i].x, blocos[i].y + blocos[i].alt);
            }
        }

        glEnd();

        glDisable(GL_TEXTURE_2D);


        // FECHA A MATRIZ
        glPopMatrix();

        // ANIMAÇÃO
        SDL_GL_SwapBuffers();

    }

    SDL_FreeSurface(icone);
    Mix_FreeMusic(musica);
    SDL_FreeSurface(tela);
    Mix_CloseAudio();

    SDL_Quit(); // FECHA O COMANDO SDL_INIT

    return 0;
}
