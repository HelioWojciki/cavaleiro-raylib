#include "raylib.h"

#define NUM_FRAMES 7   // corrida do cavaleiro (000 a 006)
#define NUM_FRAMES2 8  // ataque do cavaleiro (000 a 007)
#define NUM_ZUMBI 12   // frames de corrida de cada zumbi (000 a 011)
#define NUM_MORTE 15   // frames de morte de cada zumbi (000 a 014)
#define NUM_INIMIGOS 5 // qtd de zumbis normais
#define MAX_CORPOS 100 // max de corpos no chão

#define PONTOS_BOSS_1 10
#define PONTOS_BOSS_2 25
#define PONTOS_BOSS_3 40

#define VIDA_BOSS_1 3
#define VIDA_BOSS_2 5
#define VIDA_BOSS_3 8

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600

// faz um zumbi normal (re)nascer fora da tela, com velocidade e desvio aleatórios
void nascerZumbi(Vector2 *pos, float *vel, Vector2 *desvio, int pontos)
{
    bool esquerda = GetRandomValue(0, 1);
    pos->x = esquerda ? -60 : LARGURA_JANELA + 60;
    pos->y = GetRandomValue(300, 450);

    float extraVel = pontos * 0.03f;
    if (extraVel > 1.2f)
        extraVel = 1.2f; // trava pra não ficar absurdo

    *vel = (GetRandomValue(8, 15) / 10.0f) + extraVel;
    desvio->x = GetRandomValue(-15, 15); // cada um mira num ponto um pouco diferente
    desvio->y = GetRandomValue(-10, 10);
}

// caixa de colisão do zumbi (menor que a imagem, pra não bater no "vazio")
Rectangle caixaZumbi(Vector2 p, Texture2D tz, float escZ, float altCav)
{
    float larg = tz.width * escZ;
    float alt = tz.height * escZ;
    float yDes = p.y + altCav - alt; // alinhado pelos pés
    return (Rectangle){p.x + larg * 0.3f, yDes + alt * 0.2f, larg * 0.4f, alt * 0.8f};
}

// desenha um corpo no chão (frame da animação de morte)
void desenharCorpo(Texture2D tx, Vector2 pos, bool virado, float esc, float altCav)
{
    float larg = tx.width * esc;
    float alt = tx.height * esc;
    Rectangle origem = {0, 0, virado ? -(float)tx.width : (float)tx.width, (float)tx.height};
    Rectangle destino = {pos.x, pos.y + altCav - alt, larg, alt}; // pés alinhados
    DrawTexturePro(tx, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
}

int main(void)
{
    // ===================== 1. INICIALIZAÇÃO =====================
    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "Cavaleiro");
    SetTargetFPS(60);
    InitAudioDevice();
    Music musica = LoadMusicStream("resources/musica.mp3");
    PlayMusicStream(musica);
    Sound somAtaque = LoadSound("resources/sword_sfx.wav");
    SetSoundVolume(somAtaque, 0.7f); // 0.0 a 1.0

    Texture2D run[NUM_FRAMES];
    Texture2D run2[NUM_FRAMES2];
    Texture2D zumbi[NUM_ZUMBI];  // villager 1 correndo (zumbis normais)
    Texture2D zumbi2[NUM_ZUMBI]; // villager 2 correndo (chefes)
    Texture2D morte1[NUM_MORTE]; // villager 1 morrendo
    Texture2D morte2[NUM_MORTE]; // villager 2 morrendo

    for (int i = 0; i < NUM_FRAMES; i++)
        run[i] = LoadTexture(TextFormat("resources/knight/_PNG/1_KNIGHT/_RUN/_RUN_%03d.png", i));
    for (int i = 0; i < NUM_FRAMES2; i++)
        run2[i] = LoadTexture(TextFormat("resources/knight/_PNG/1_KNIGHT/_ATTACK/ATTACK_%03d.png", i));
    for (int i = 0; i < NUM_ZUMBI; i++)
        zumbi[i] = LoadTexture(TextFormat("resources/Zombie_Villager_1/PNG/PNG Sequences/Run Slashing/0_Zombie_Villager_Run Slashing_%03d.png", i));
    for (int i = 0; i < NUM_ZUMBI; i++)
        zumbi2[i] = LoadTexture(TextFormat("resources/Zombie_Villager_2/PNG/PNG Sequences/Run Slashing/0_Zombie_Villager_Run Slashing_%03d.png", i));
    for (int i = 0; i < NUM_MORTE; i++)
        morte1[i] = LoadTexture(TextFormat("resources/Zombie_Villager_1/PNG/PNG Sequences/Dying/0_Zombie_Villager_Dying_%03d.png", i));
    for (int i = 0; i < NUM_MORTE; i++)
        morte2[i] = LoadTexture(TextFormat("resources/Zombie_Villager_2/PNG/PNG Sequences/Dying/0_Zombie_Villager_Dying_%03d.png", i));

    // --- Cavaleiro ---
    Vector2 position = {350.0f, 300.0f};
    float escala = 0.12f;
    int frameAtual = 0;
    int contador = 0;
    int framesPorImagem = 6;
    bool viradoEsquerda = false;
    bool atacando = false;
    int frameAtaque = 0;
    int framesPorImagemAtaque = 4;

    // --- Estado do jogo ---
    int vida = 3;
    int pontos = 0;
    int invencivel = 0; // quadros restantes de invencibilidade
    bool gameOver = false;
    bool venceu = false;
    bool debug = false;

    // --- Zumbis normais ---
    Vector2 posZumbi[NUM_INIMIGOS];
    Vector2 desvio[NUM_INIMIGOS];
    float velZumbi[NUM_INIMIGOS];
    bool zumbiVirado[NUM_INIMIGOS];
    bool ativo[NUM_INIMIGOS];
    float escalaZumbi = 0.12f;
    int frameZumbi = 0;
    int contadorZumbi = 0;

    for (int i = 0; i < NUM_INIMIGOS; i++)
    {
        nascerZumbi(&posZumbi[i], &velZumbi[i], &desvio[i], pontos);
        zumbiVirado[i] = false;
        ativo[i] = true;
    }

    // --- Chefes (1 facil, 2 medio, 3 dificil) ---
    int tipoBoss = 0;
    int vidaMaxBoss = 1;
    bool chamou1 = false;
    bool chamou2 = false;
    bool chamou3 = false;

    Vector2 posBoss = {0, 0};
    float velBoss = 1.5f;     // normais vão até 1.5, cavaleiro anda 3
    float escalaBoss = 0.16f; // maior que os normais
    int vidaBoss = 0;
    bool bossAtivo = false;
    bool bossVirado = false;
    bool bossAcertado = false; // já levou dano neste ataque
    int bossPisca = 0;
    int avisoBoss = 0;

    // --- Corpos no chão ---
    Vector2 corpoPos[MAX_CORPOS];
    bool corpoVirado[MAX_CORPOS];
    int corpoFrame[MAX_CORPOS];
    int corpoCont[MAX_CORPOS];
    int numCorpos = 0;
    int proxCorpo = 0;

    bool bossMorto = false;
    Vector2 posBossMorto = {0, 0};
    bool bossMortoVirado = false;
    int bossFrameMorte = 0;
    int bossContMorte = 0;

    // ===================== 2. LOOP PRINCIPAL =====================
    while (!WindowShouldClose())
    {
        // ---------- UPDATE ----------
        UpdateMusicStream(musica);
        if (IsKeyPressed(KEY_TAB))
            debug = !debug;

        float largCav = run[0].width * escala;
        float altCav = run[0].height * escala;

        // animação de queda dos corpos (fica fora do else pra continuar depois do fim do jogo)
        for (int i = 0; i < numCorpos; i++)
        {
            if (corpoFrame[i] < NUM_MORTE - 1)
            { // para no último frame
                corpoCont[i]++;
                if (corpoCont[i] >= 4)
                {
                    corpoCont[i] = 0;
                    corpoFrame[i]++;
                }
            }
        }
        if (bossMorto && bossFrameMorte < NUM_MORTE - 1)
        {
            bossContMorte++;
            if (bossContMorte >= 4)
            {
                bossContMorte = 0;
                bossFrameMorte++;
            }
        }

        if (gameOver || venceu)
        {
            if (IsKeyPressed(KEY_ENTER))
            { // reinicia
                position = (Vector2){350.0f, 300.0f};
                vida = 3;
                pontos = 0;
                invencivel = 0;
                atacando = false;
                gameOver = false;
                venceu = false;

                tipoBoss = 0;
                vidaMaxBoss = 1;
                chamou1 = false;
                chamou2 = false;
                chamou3 = false;
                bossAtivo = false;
                bossMorto = false;
                vidaBoss = 0;
                velBoss = 1.5f;
                bossPisca = 0;
                avisoBoss = 0;

                numCorpos = 0;
                proxCorpo = 0;
                for (int i = 0; i < NUM_INIMIGOS; i++)
                {
                    nascerZumbi(&posZumbi[i], &velZumbi[i], &desvio[i], pontos);
                    ativo[i] = true;
                }
            }
        }
        else
        {
            bool andando = false;

            // movimento do cavaleiro
            if (IsKeyDown(KEY_D))
            {
                position.x += 3;
                viradoEsquerda = false;
                andando = true;
            }
            if (IsKeyDown(KEY_A))
            {
                position.x -= 3;
                viradoEsquerda = true;
                andando = true;
            }
            if (IsKeyDown(KEY_S))
            {
                position.y += 3;
                andando = true;
            }
            if (IsKeyDown(KEY_W))
            {
                position.y -= 3;
                andando = true;
            }

            // limites da tela e do chão
            if (position.x < 0)
                position.x = 0;
            if (position.x > LARGURA_JANELA - largCav)
                position.x = LARGURA_JANELA - largCav;
            if (position.y > 450)
                position.y = 450;
            if (position.y < 300)
                position.y = 300;

            // ataque (um clique = uma animação completa)
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !atacando)
            {
                atacando = true;
                frameAtaque = 0;
                contador = 0;
                bossAcertado = false; // novo golpe pode acertar o chefe de novo
                SetSoundPitch(somAtaque, GetRandomValue(90, 110) / 100.0f);
                PlaySound(somAtaque);
            }

            // animação do cavaleiro
            if (atacando)
            {
                contador++;
                if (contador >= framesPorImagemAtaque)
                {
                    contador = 0;
                    frameAtaque++;
                    if (frameAtaque >= NUM_FRAMES2)
                    {
                        atacando = false;
                        frameAtaque = 0;
                    }
                }
            }
            else if (andando)
            {
                contador++;
                if (contador >= framesPorImagem)
                {
                    contador = 0;
                    frameAtual = (frameAtual + 1) % NUM_FRAMES;
                }
            }
            else
                frameAtual = 5;

            // chama cada chefe na sua hora
            if (!chamou1 && !bossAtivo && pontos >= PONTOS_BOSS_1)
            {
                chamou1 = true;
                bossAtivo = true;
                tipoBoss = 1;
                vidaMaxBoss = VIDA_BOSS_1;
                vidaBoss = VIDA_BOSS_1;
                velBoss = 1.3f;
                escalaBoss = 0.15f;
                posBoss = (Vector2){LARGURA_JANELA + 60, 350};
                avisoBoss = 150; // ~2,5s de aviso na tela
            }
            else if (chamou1 && !chamou2 && !bossAtivo && pontos >= PONTOS_BOSS_2)
            {
                chamou2 = true;
                bossAtivo = true;
                tipoBoss = 2;
                vidaMaxBoss = VIDA_BOSS_2;
                vidaBoss = VIDA_BOSS_2;
                velBoss = 1.7f;
                escalaBoss = 0.17f;
                posBoss = (Vector2){LARGURA_JANELA + 60, 350};
                avisoBoss = 150;
            }
            else if (chamou2 && !chamou3 && !bossAtivo && pontos >= PONTOS_BOSS_3)
            {
                chamou3 = true;
                bossAtivo = true;
                tipoBoss = 3;
                vidaMaxBoss = VIDA_BOSS_3;
                vidaBoss = VIDA_BOSS_3;
                velBoss = 2.1f;
                escalaBoss = 0.19f;
                posBoss = (Vector2){LARGURA_JANELA + 60, 350};
                avisoBoss = 150;
            }

            // zumbis normais perseguem o cavaleiro e vão pegando velocidade
            for (int i = 0; i < NUM_INIMIGOS; i++)
            {
                if (!ativo[i])
                    continue;
                if (velZumbi[i] < 2.7f)
                    velZumbi[i] += 0.0005f;

                float alvoX = position.x + desvio[i].x;
                float alvoY = position.y + desvio[i].y;

                if (posZumbi[i].x < alvoX - velZumbi[i])
                {
                    posZumbi[i].x += velZumbi[i];
                    zumbiVirado[i] = false;
                }
                else if (posZumbi[i].x > alvoX + velZumbi[i])
                {
                    posZumbi[i].x -= velZumbi[i];
                    zumbiVirado[i] = true;
                }

                if (posZumbi[i].y < alvoY - velZumbi[i])
                    posZumbi[i].y += velZumbi[i];
                else if (posZumbi[i].y > alvoY + velZumbi[i])
                    posZumbi[i].y -= velZumbi[i];
            }

            // chefe persegue o cavaleiro
            if (bossAtivo)
            {
                if (posBoss.x < position.x - velBoss)
                {
                    posBoss.x += velBoss;
                    bossVirado = false;
                }
                else if (posBoss.x > position.x + velBoss)
                {
                    posBoss.x -= velBoss;
                    bossVirado = true;
                }

                if (posBoss.y < position.y - velBoss)
                    posBoss.y += velBoss;
                else if (posBoss.y > position.y + velBoss)
                    posBoss.y -= velBoss;
            }

            // animação de corrida dos zumbis (compartilhada)
            contadorZumbi++;
            if (contadorZumbi >= 4)
            {
                contadorZumbi = 0;
                frameZumbi = (frameZumbi + 1) % NUM_ZUMBI;
            }

            if (bossPisca > 0)
                bossPisca--;
            if (avisoBoss > 0)
                avisoBoss--;

            // ---------- COLISÃO ----------
            Texture2D tz = zumbi[frameZumbi];
            Texture2D tzb = zumbi2[frameZumbi];

            Rectangle hitCav = {position.x + largCav * 0.3f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.8f};
            Rectangle hitAtaque = viradoEsquerda
                                      ? (Rectangle){position.x + largCav * 0.1f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.6f}
                                      : (Rectangle){position.x + largCav * 0.5f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.6f};

            if (invencivel > 0)
                invencivel--;

            // zumbis normais
            for (int i = 0; i < NUM_INIMIGOS; i++)
            {
                if (!ativo[i])
                    continue;
                Rectangle hitZ = caixaZumbi(posZumbi[i], tz, escalaZumbi, altCav);

                if (atacando && CheckCollisionRecs(hitAtaque, hitZ))
                {
                    pontos++; // matou o zumbi

                    // deixa o corpo no chão
                    corpoPos[proxCorpo] = posZumbi[i];
                    corpoVirado[proxCorpo] = zumbiVirado[i];
                    corpoFrame[proxCorpo] = 0;
                    corpoCont[proxCorpo] = 0;
                    proxCorpo = (proxCorpo + 1) % MAX_CORPOS;
                    if (numCorpos < MAX_CORPOS)
                        numCorpos++;

                    if (bossAtivo)
                        ativo[i] = false; // só segura spawn enquanto tem chefe na tela
                    else
                        nascerZumbi(&posZumbi[i], &velZumbi[i], &desvio[i], pontos);
                }
                else if (invencivel == 0 && CheckCollisionRecs(hitCav, hitZ))
                {
                    vida--;          // zumbi te pegou
                    invencivel = 90; // 1,5s
                    if (vida <= 0)
                        gameOver = true;
                }
            }

            // chefe
            if (bossAtivo)
            {
                Rectangle hitB = caixaZumbi(posBoss, tzb, escalaBoss, altCav);

                if (atacando && !bossAcertado && CheckCollisionRecs(hitAtaque, hitB))
                {
                    vidaBoss--;
                    bossAcertado = true; // só 1 dano por golpe
                    bossPisca = 20;
                    if (vidaBoss <= 0)
                    {
                        bossAtivo = false;
                        bossMorto = true; // corpo do chefe fica no chão
                        posBossMorto = posBoss;
                        bossMortoVirado = bossVirado;
                        bossFrameMorte = 0;
                        bossContMorte = 0;

                        if (tipoBoss == 3)
                        {
                            venceu = true;
                        }
                        else
                        {
                            // volta a leva de zumbis pro próximo trecho
                            for (int i = 0; i < NUM_INIMIGOS; i++)
                            {
                                nascerZumbi(&posZumbi[i], &velZumbi[i], &desvio[i], pontos);
                                ativo[i] = true;
                            }
                        }
                    }
                }
                else if (invencivel == 0 && CheckCollisionRecs(hitCav, hitB))
                {
                    vida -= (tipoBoss == 1) ? 1 : 2;
                    invencivel = 90;
                    if (vida <= 0)
                    {
                        vida = 0;
                        gameOver = true;
                    }
                }
            }
        }

        // ---------- DRAW ----------
        Texture2D t = atacando ? run2[frameAtaque] : run[frameAtual];
        Rectangle origem = {0, 0, viradoEsquerda ? -(float)t.width : (float)t.width, (float)t.height};
        Rectangle destino = {position.x, position.y, t.width * escala, t.height * escala};
        Texture2D tz = zumbi[frameZumbi];
        Texture2D tzb = zumbi2[frameZumbi];

        BeginDrawing();
        ClearBackground(BLACK);

        // céu em degradê
        DrawRectangleGradientV(0, 0, LARGURA_JANELA, 400, (Color){90, 170, 255, 255}, (Color){210, 235, 255, 255});

        // sol
        DrawCircle(650, 90, 40, (Color){255, 240, 150, 255});

        // nuvens (3 círculos cada)
        DrawCircle(140, 90, 25, WHITE);
        DrawCircle(170, 80, 32, WHITE);
        DrawCircle(205, 92, 24, WHITE);
        DrawCircle(430, 140, 20, WHITE);
        DrawCircle(455, 130, 28, WHITE);
        DrawCircle(485, 142, 20, WHITE);

        // colinhas ao fundo (o chão cobre a parte de baixo)
        DrawCircle(120, 480, 150, (Color){60, 140, 80, 255});
        DrawCircle(520, 520, 190, (Color){50, 120, 70, 255});
        DrawCircle(770, 470, 120, (Color){60, 140, 80, 255});

        // chão em degrade
        DrawRectangleGradientV(0, 400, LARGURA_JANELA, 200, (Color){50, 150, 60, 255}, (Color){20, 90, 40, 255});

        // corpos no chão (por baixo de todo mundo)
        for (int i = 0; i < numCorpos; i++)
            desenharCorpo(morte1[corpoFrame[i]], corpoPos[i], corpoVirado[i], escalaZumbi, altCav);
        if (bossMorto)
            desenharCorpo(morte2[bossFrameMorte], posBossMorto, bossMortoVirado, escalaBoss, altCav);

        // zumbis normais
        for (int i = 0; i < NUM_INIMIGOS; i++)
        {
            if (!ativo[i])
                continue;
            float largZ = tz.width * escalaZumbi;
            float altZ = tz.height * escalaZumbi;
            Rectangle origemZ = {0, 0, zumbiVirado[i] ? -(float)tz.width : (float)tz.width, (float)tz.height};
            Rectangle destinoZ = {posZumbi[i].x, posZumbi[i].y + altCav - altZ, largZ, altZ};
            DrawTexturePro(tz, origemZ, destinoZ, (Vector2){0, 0}, 0.0f, WHITE);
        }

        // chefe (fica vermelho quando leva dano)
        if (bossAtivo)
        {
            float largB = tzb.width * escalaBoss;
            float altB = tzb.height * escalaBoss;
            Rectangle origemB = {0, 0, bossVirado ? -(float)tzb.width : (float)tzb.width, (float)tzb.height};
            Rectangle destinoB = {posBoss.x, posBoss.y + altCav - altB, largB, altB};
            Color cor = WHITE;
            if (bossPisca > 0)
                cor = RED;
            else if (tipoBoss == 2)
                cor = (Color){225, 225, 255, 255};
            else if (tipoBoss == 3)
                cor = (Color){255, 185, 185, 255};
            DrawTexturePro(tzb, origemB, destinoB, (Vector2){0, 0}, 0.0f, cor);
        }

        // cavaleiro (pisca quando está invencível)
        if (invencivel == 0 || (invencivel / 6) % 2 == 0)
            DrawTexturePro(t, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);

        // caixas de colisão (TAB)
        if (debug)
        {
            DrawRectangleLinesEx((Rectangle){position.x + largCav * 0.3f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.8f}, 2, BLUE);
            if (atacando)
            {
                Rectangle ha = viradoEsquerda
                                   ? (Rectangle){position.x + largCav * 0.1f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.6f}
                                   : (Rectangle){position.x + largCav * 0.5f, position.y + altCav * 0.2f, largCav * 0.4f, altCav * 0.6f};
                DrawRectangleLinesEx(ha, 2, YELLOW);
            }
            for (int i = 0; i < NUM_INIMIGOS; i++)
                if (ativo[i])
                    DrawRectangleLinesEx(caixaZumbi(posZumbi[i], tz, escalaZumbi, altCav), 2, RED);
            if (bossAtivo)
                DrawRectangleLinesEx(caixaZumbi(posBoss, tzb, escalaBoss, altCav), 2, PURPLE);
        }

        // interface
        DrawText("WASD = andar | Mouse esquerdo = atacar ", 10, 10, 20, BLACK);
        DrawText(TextFormat("Vida: %d", vida), 10, 40, 24, RED);
        DrawText(TextFormat("Pontos: %d", pontos), 10, 70, 24, BLACK);

        // barra de vida do chefe
        if (bossAtivo)
        {
            const char *rotulo = (tipoBoss == 1) ? "CHEFE FACIL" : (tipoBoss == 2) ? "CHEFE MEDIO" : "CHEFE DIFICIL";
            DrawText(rotulo, 570, 40, 20, BLACK);
            DrawRectangle(600, 65, 180, 16, DARKGRAY);
            DrawRectangle(600, 65, (int)(180.0f * vidaBoss / vidaMaxBoss), 16, RED);
            DrawRectangleLines(600, 65, 180, 16, BLACK);
        }

        if (avisoBoss > 0)
            DrawText("O CHEFE CHEGOU!", 190, 150, 50, PURPLE);

        if (gameOver)
        {
            DrawText("GAME OVER", 270, 220, 60, RED);
            DrawText("ENTER para reiniciar", 270, 290, 24, BLACK);
        }
        if (venceu)
        {
            DrawText("VOCE VENCEU!", 250, 220, 60, GOLD);
            DrawText("ENTER para jogar de novo", 250, 290, 24, BLACK);
        }
        EndDrawing();
    }

    // ===================== 3. FINALIZAÇÃO =====================
    for (int i = 0; i < NUM_FRAMES; i++)
        UnloadTexture(run[i]);
    for (int i = 0; i < NUM_FRAMES2; i++)
        UnloadTexture(run2[i]);
    for (int i = 0; i < NUM_ZUMBI; i++)
        UnloadTexture(zumbi[i]);
    for (int i = 0; i < NUM_ZUMBI; i++)
        UnloadTexture(zumbi2[i]);
    for (int i = 0; i < NUM_MORTE; i++)
        UnloadTexture(morte1[i]);
    for (int i = 0; i < NUM_MORTE; i++)
        UnloadTexture(morte2[i]);
    UnloadSound(somAtaque);
    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}