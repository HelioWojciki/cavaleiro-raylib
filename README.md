Cavaleiro — Jogo 2D em Raylib

Sobre o jogo
Cavaleiro é um jogo 2D de ação desenvolvido em C com a biblioteca Raylib. O jogador controla um cavaleiro que enfrenta zumbis, marca pontos e precisa derrotar três chefes em sequência crescente de dificuldade: fácil, médio e difícil.

O jogo começa diretamente na partida. Você inicia com 3 vidas. O placar aumenta ao derrotar zumbis comuns, e os chefes aparecem ao alcançar determinados pontos. A partida termina em derrota quando as vidas acabam ou em vitória quando o terceiro chefe é derrotado. Nas duas situações, é possível reiniciar.
Integrantes
Nome completo	R.A.
Pedro Augusto de Souza	26041723
João Victor dos Santos	26031723
Helio Manoel Wojciki	26315323


Controles
Comando	Ação
W / A / S / D	Movimentar o cavaleiro
Botão esquerdo do mouse	Atacar com a espada
Enter	Reiniciar após derrota ou vitória
Tab	Mostrar/ocultar caixas de colisão (modo de depuração)


Regras e progressão
- Existem até 5 zumbis comuns ativos simultaneamente. Cada zumbi derrotado adiciona 1 ponto.
- Os inimigos perseguem o jogador e podem causar dano por contato.
- Ao atingir 10 pontos, aparece o chefe fácil, com 3 pontos de vida.
- Ao atingir 25 pontos, aparece o chefe médio, com 5 pontos de vida.
- Ao atingir 40 pontos, aparece o chefe difícil, com 8 pontos de vida.
- Cada chefe possui barra de vida. Chefes mais difíceis se movimentam mais rapidamente; o médio e o difícil causam 2 pontos de dano por contato, enquanto o fácil causa 1.
- Quando o chefe é derrotado, o jogo retoma os inimigos comuns para a próxima etapa (exceto após o último chefe).
- Ao perder as vidas, aparece GAME OVER; ao derrotar o terceiro chefe, aparece VOCE VENCEU!.
Como baixar e executar no Windows
1. Baixe o repositório como ZIP em Code → Download ZIP ou clone-o pelo Git.
2. Extraia o conteúdo preservando a estrutura de diretórios.
3. Entre na pasta que contém jogo.exe, jogo.c e resources/.
4. Execute jogo.exe a partir dessa pasta.
Importante: o jogo carrega imagens e áudios usando caminhos relativos (resources/...); portanto, não mova apenas o executável para outra pasta. Mantenha resources/ ao lado dele. O arquivo jogo.exe está presente no pacote do código analisado. A execução em outro computador não foi verificada independentemente.
Compilação do código-fonte (Windows)
O projeto inclui .vscode/tasks.json, configurado para usar GCC do w64devkit e Raylib nos caminhos C:/raylib/.... Com a instalação e os caminhos configurados da mesma forma, abra o projeto no VS Code e execute Terminal → Run Build Task (Ctrl+Shift+B).

Com GCC e Raylib configurados no PATH e nas pastas padrão de busca do compilador, a linha de compilação equivalente é:
gcc jogo.c -o jogo.exe -lraylib -lopengl32 -lgdi32 -lwinmm
Se necessário, configure os caminhos de inclusão e biblioteca com -I e -L, como no arquivo .vscode/tasks.json. A compilação depende de Raylib e do compilador estarem corretamente instalados; este comando não foi executado neste ambiente.
Tecnologias utilizadas
- C — implementação da lógica do jogo.
- Raylib — janela, entrada por teclado/mouse, desenho, texturas, colisões, áudio e loop de jogo.
- GCC / w64devkit — compilação para Windows conforme configuração do projeto.
- Visual Studio Code — configuração de compilação presente em .vscode/.
- Git e GitHub — distribuição do código-fonte.
Estrutura do projeto
cavaleiro-raylib/
├── jogo.c                 # Código-fonte em C
├── jogo.exe               # Executável Windows disponível no pacote analisado
├── resources/
│   ├── knight/            # Animações e arquivos do cavaleiro
│   ├── Zombie_Villager_1/ # Animações e arquivos dos zumbis comuns
│   ├── Zombie_Villager_2/ # Animações e arquivos usados para os chefes
│   ├── musica.mp3         # Música de fundo
│   └── sword_sfx.wav      # Efeito de ataque
├── .vscode/               # Configurações de compilação/depuração
├── screenshots/           # Capturas para documentação (adicionar esta pasta)
└── README.md
O loop principal em jogo.c utiliza WindowShouldClose(), atualiza música e estados, trata movimentação e ataques, verifica colisões com CheckCollisionRecs(), e desenha os elementos entre BeginDrawing() e EndDrawing(). nascerZumbi() posiciona novos inimigos, caixaZumbi() define uma área de colisão menor que a imagem e desenharCorpo() desenha os inimigos derrotados.
Capturas de tela
Combate contra os inimigos:

Derrota e reinício:

Transparência sobre o uso de Inteligência Artificial
O grupo utilizou ferramentas de inteligência artificial como apoio durante o desenvolvimento, estimando sua participação em aproximadamente 35% a 40% do processo. Esse número é uma estimativa dos integrantes, não uma contagem objetiva de linhas de código nem uma auditoria de autoria.

Os usos que os integrantes recordam e confirmaram incluem:
- assistência na implementação/ajuste de colisões;
- assistência na lógica de detecção e aplicação de dano aos chefes.
O grupo também relatou ter usado IA em outras partes do desenvolvimento, mas ainda não identificou precisamente todos esses trechos. A revisão do código permite observar as mecânicas, não determinar sozinha quais linhas foram escritas por pessoas ou geradas por IA. Por isso, não atribuímos autoria específica a cada função sem confirmação. O grupo ficou responsável por integrar, testar e ajustar o jogo conforme necessário.

A completar antes da entrega: especificar outras contribuições de IA que os integrantes confirmarem; confirmar as atividades executadas diretamente pelos integrantes para a apresentação oral. Não declarar “100% desenvolvido pelos alunos” em trechos apoiados por IA.
Créditos dos recursos e licenças
- Sprites e animações do cavaleiro e dos zumbis: os pacotes incluídos apontam para CraftPix e para as condições de licença da CraftPix. A página exata de cada pacote e a modalidade de licença (gratuita/paga) precisam ser confirmadas pelo grupo.
- Música (resources/musica.mp3): [PENDENTE: identificar autor, título, origem e permissão de uso/publicação].
- Efeito de espada (resources/sword_sfx.wav): [PENDENTE: identificar autor, origem e permissão de uso/publicação].
- Fontes extras contidas nos pacotes: os arquivos internos mencionam Moon Get! (daFont) e The Wild Breath of Zelda (daFont); essas fontes não são carregadas explicitamente pelo jogo.c analisado. Se forem redistribuídas, verifiquem suas licenças separadamente.
Atenção ao repositório público: as condições gerais da CraftPix restringem a redistribuição de arquivos-fonte de arte (incluindo PNG/EPS/AI) de forma reutilizável por terceiros. O ZIP recebido contém diversos arquivos originais extras de pacotes de assets. Antes de publicar ou manter esses arquivos em repositório público, verifiquem autorização específica e removam os itens não utilizados. Apenas adicionar créditos não substitui a permissão de redistribuição.
Vídeo de apresentação
[PENDENTE: inserir link do vídeo publicado após a gravação]

Vídeo horizontal, mínimo 1280 × 720, duração de até 5 minutos, com apresentação dos três integrantes, demonstração do jogo, explicação da Raylib e transparência sobre IA.
