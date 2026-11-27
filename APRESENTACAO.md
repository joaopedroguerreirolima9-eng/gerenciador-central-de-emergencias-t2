# Roteiro de apresentação e estudo

Este roteiro acompanha o código entregue. Os dois integrantes devem praticar todos os módulos, pois o enunciado permite que o professor distribua as partes da apresentação na hora.

## Antes de demonstrar

1. Abrir a raiz do projeto e executar `.\build.bat` no PowerShell.
2. Conferir a mensagem de 150 ocorrências carregadas.
3. Manter o README aberto nas seções dos módulos e de complexidade.
4. Ao alterar dados para testar, lembrar que as mudanças são da sessão. Reiniciar restaura o CSV e os cenários iniciais.

O programa precisa de GCC; Python é usado apenas na automação dos testes. Não depende de internet nem de API.

## Sequência de demonstração

| Etapa | O que digitar | O que verificar e explicar |
|---|---|---|
| Consulta do T1 | `2 -> 1 -> ID 4` | A Hash encontra a ocorrência e a consulta mostra seu custo/benefício simulado. `0` volta. |
| Dependências | `4 -> 2 -> 6` | Kahn produz ordem válida para as 30 atividades. |
| Ciclo | No mesmo submenu, `4 -> 30 -> 1`, depois `6` | A nova dependência fecha um ciclo e impede ordenar todas as atividades. |
| Restaurar | `5 -> 30 -> 1`, depois `6` | Remover a aresta restaura a ordem. `0` volta à Investigação e outro `0` ao principal. |
| Mochila e resumo | `7 -> 1 -> NORRISTOWN -> 60` | IDs 4 e 19, 60 minutos, 8 pontos; as outras 12 candidatas ficam de fora. O resumo fecha esses totais. |
| Capacidade zero | Ainda no planejamento, `1 -> NORRISTOWN -> 0` | Seleção e resumo vazios; resultado anterior não é reaproveitado. `0` volta. |
| Rede | `8 -> 1` e depois `2` | 41 regiões e 82 conexões simuladas. |
| Caminho mínimo | Ainda na Logística, `3 -> 1 -> 22` | ABINGTON a NORRISTOWN: caminho reconstruído e 191,68 km na topologia simulada. |
| Árvore mínima | `4` | 40 conexões, todos os vértices, sem ciclos, total 719,23 km. |
| Encerrar | `0` e `0` | Retorna ao principal e libera as estruturas do T1 ao sair. |

Se houver tempo, repetir a mochila com região em branco e capacidade 60. HATFIELD TOWNSHIP e NEW HANOVER ficam empatadas com uma ocorrência selecionada cada.

## Como os arquivos se conectam

`main.c` carrega o CSV, insere as ocorrências na Hash e nos índices B+, inicializa as dependências e monta a rede regional. Depois encaminha cada opção de menu para sua função.

| Estudar | Responsabilidade |
|---|---|
| `hash.c`, `btree.c`, `indices.c` | Base do T1: registro completo por ID e índices por texto/data |
| `dependencias.c`, `kahn.c` | Grafo dirigido de atividades e ordenação topológica |
| `otimizacao.c`, `mochila.c` | Preparação dos candidatos, custo/benefício e programação dinâmica |
| `grafo.c`, `dijkstra.c`, `prim.c` | Rede ponderada, caminho mínimo e árvore mínima |
| `rede.c` | Apenas interação e apresentação das consultas logísticas |
| `resumo.c` | Agrupa a seleção por região e soma os resultados |

Os `.h` descrevem os tipos, constantes e funções públicos. Os `.c` implementam as operações. O prefixo das funções, como `rede_`, identifica o conjunto ao qual pertencem; o nome do arquivo deixa visível a técnica.

## Kahn passo a passo

**Problema:** respeitar a ordem entre atividades. Uma aresta A → B diz que A deve terminar antes de B.

- `arestas[u][v]`: presença de dependência de u para v.
- `grau[v]`: quantas dependências ainda impedem executar v.
- `fila`, `inicio` e `fim`: atividades já liberadas, aguardando processamento.
- `ordem`: sequência produzida.

Primeiro, calcular os graus de entrada e enfileirar os vértices de grau zero. Retirar uma atividade da fila, acrescentar à ordem e reduzir os graus de suas sucessoras. Se alguma chegar a zero, entra na fila. Ao terminar, comparar o total processado com V.

Se o total for menor que V, existe ciclo. Os vértices que ficaram bloqueados podem estar no ciclo ou depender dele. O programa não afirma que todos os bloqueados pertencem diretamente ao ciclo.

Na matriz, calcular graus e visitar sucessores exige `O(V²)`; a fila e os vetores usam `O(V)`.

**Praticar:** explicar por que a fila começa com grau zero e por que uma ordenação topológica não existe quando há ciclo.

## Mochila 0/1 passo a passo

**Problema:** escolher ocorrências para maximizar pontos sem ultrapassar o tempo disponível. Cada ocorrência é incluída uma vez ou excluída.

`plano_item` transforma a categoria do T1 em custo/benefício simulado. `plano_mochila` recebe esses itens e preenche `escolhido`, que vale 0 ou 1 por candidata.

`dp[i][c]` significa: melhor benefício considerando os primeiros i itens e capacidade c. Para cada item, comparar:

- Excluir: manter `dp[i-1][c]`.
- Incluir, se couber: somar o benefício a `dp[i-1][c-custo]`.

Usar a linha anterior nas duas alternativas impede escolher o mesmo item repetidamente. A linha zero e a capacidade zero começam em zero.

Na reconstrução, comparar a linha atual com a anterior. Se o benefício mudou, o item foi escolhido, e sua capacidade é descontada. Em empate, o código deixa esse item de fora. A otimização é de benefício; não promete uma segunda otimização por quantidade de ocorrências ou menor tempo utilizado.

A tabela é alocada com `calloc` para ficar fora da pilha e começar em zero. A declaração `int (*dp)[colunas]` permite acessar essa memória como `dp[i][c]`. `sizeof *dp` é o tamanho de uma linha; o teste com `SIZE_MAX` verifica se o tamanho da alocação cabe no tipo usado para tamanhos. `free(dp)` libera a tabela ao final.

Tempo e memória são `O(n*C)`. O limite de 1440 minutos controla o tamanho da tabela.

**Praticar:** mostrar uma célula em que o item não cabe e outra em que incluir vence excluir. Explicar por que uma escolha gulosa não garante o ótimo da mochila 0/1.

## Construção do grafo logístico

São dois grafos diferentes no trabalho: o direcionado de atividades do módulo 6 e o não direcionado de regiões do módulo 8.

A rede logística percorre a Hash e agrupa as ocorrências pelo nome da região. Calcula a média das coordenadas de cada grupo, ordena os nomes e cria conexões com os próximos um e dois índices, circularmente.

Para 41 vértices, isso dá 82 arestas únicas. Cada região se conecta a vizinhas na lista em ambos os sentidos. A ligação com o próximo forma um anel; as outras fornecem alternativas.

A média das coordenadas é apenas um ponto representativo dos chamados. A topologia alfabética é simulada: não prova existência de estradas, hospitais ou bases nesses pontos. A aproximação de distância vem da função do T1. O peso é inteiro em centésimos de km para trabalhar com somas inteiras; na impressão é dividido por 100.

A matriz usa `-1` para ausência de conexão, pois zero pode ser um custo válido. As duas posições `[u][v]` e `[v][u]` têm o mesmo peso, mas contam como uma aresta.

**Praticar:** justificar a simulação e a escala do grafo. Dizer que a rede é um cenário fixo da sessão e que uma alteração na ocorrência não reconstrói as conexões automaticamente.

## Dijkstra e Prim

As implementações se parecem porque ambas escolhem o menor valor entre vértices ainda não processados. O significado desse valor é diferente:

| Ponto | Dijkstra | Prim |
|---|---|---|
| Objetivo | Menor custo entre origem e destino | Menor soma de conexões para ligar toda a rede |
| Valor guardado | Distância acumulada desde a origem | Custo de uma aresta que liga o vértice à árvore |
| Atualização principal | `distancias[u] + custo` | `custo` |
| Resultado | Sequência de vértices de um percurso | `V - 1` arestas conectando V vértices |

Em **Dijkstra**, `anterior[v]` permite reconstruir o caminho a partir do destino. O código coleta os vértices ao contrário e inverte o vetor. Pesos não negativos são necessários para a regra de confirmar o vértice com menor distância conhecida. `INT_MAX` representa infinito.

Em **Prim**, `anterior[v]` indica a origem da conexão candidata. O algoritmo começa no índice 0 e, a cada inclusão, procura conexões mais baratas para os vértices restantes. A árvore só é devolvida quando todos são conectados. Se não houver mais conexão e ainda faltar vértice, a rede é desconexa e o resultado parcial é descartado.

Nos dois casos, o desempate por menor índice e a atualização apenas com melhoria estrita tornam a saída reproduzível. Com matriz, ambos usam tempo `O(V²)` e vetores auxiliares `O(V)`.

**Praticar:** explicar por que uma árvore mínima não precisa produzir os menores caminhos a partir de uma origem. No exemplo A–B = 4, B–C = 3 e A–C = 10, em unidades de custo, o caminho A–B–C e a árvore com A–B/B–C custam 7, mas os problemas continuam diferentes.

## Resumo regional

`resumo_calcular` recebe os mesmos itens e a mesma máscara de seleção gerados pela mochila. Para cada item selecionado, busca a região pelo ID na Hash e encontra/cria seu grupo no vetor.

A quantidade, os minutos e o benefício são acumulados por grupo e no total. `maior_quantidade` identifica o valor de destaque; a impressão percorre todos os grupos e mostra todos os empates.

O relatório é gerado antes de liberar a seleção e não recalcula a mochila. `resumo_liberar` libera seu vetor. Região vazia recebe um rótulo de apresentação; ela não é apagada nem confundida com uma região existente.

A busca linear de grupos é simples para essa escala. Com s selecionadas e r regiões, o agrupamento custa `O(s*r)`; também há a leitura das n candidatas e a ordenação dos r nomes. As consultas à Hash têm custo médio constante, sujeito a colisões.

**Praticar:** somar duas linhas do relatório e conferir que a soma coincide com o resultado da mochila.

## Perguntas para ensaiar

| Pergunta | Pontos que a resposta precisa cobrir |
|---|---|
| O que veio do T1? | CSV, registros, Hash, três B+, consultas, integridade, guloso e funções reutilizadas |
| Por que matriz? | Escala pequena, consulta simples de arestas, mesma representação para Dijkstra/Prim; custo de memória `O(V²)` |
| Por que não usar zero como ausência? | Uma aresta pode ter custo zero; ausência é `-1` |
| Por que recusar peso negativo? | Distância é não negativa e Dijkstra exige essa condição; ela permite compartilhar a rede com Prim |
| Como provar que a seleção é ótima? | Recorrência examina incluir/excluir para cada estado; testes pequenos com força bruta conferem o benefício |
| Como o T2 usa a base do T1? | Mochila seleciona IDs da Hash/B+; resumo consulta os IDs; rede usa regiões e coordenadas da mesma Hash |
| Onde estão Kruskal e Floyd-Warshall? | Somente nos testes, como referências para comparar Prim e Dijkstra |
| Uma região sem atendimento aparece no resumo? | Só entram as regiões das ocorrências selecionadas; o resultado vazio é tratado |
| Os dados são reais? | Os chamados da amostra vêm do dataset; custos/benefícios e conexões são simulados e documentados |
| E as bibliotecas? | Biblioteca padrão de C para apoio; nenhum algoritmo central foi substituído por biblioteca pronta |

## Limites que precisam ser conhecidos

As alterações são em memória. A carga e várias consultas do T1 têm limite de 1000 registros; o projeto usa 150. Os grafos suportam 100 vértices. A B+ preserva a remoção simplificada do T1, sem redistribuição/fusão de nós. O CSV é o formato simples da amostra, e a data é validada pelo formato, não por um calendário completo.

A simulação de alteração não autorizada do módulo 4 muda um registro sem atualizar os índices, intencionalmente. Se a demonstrarem, façam isso ao final ou reiniciem antes das outras consultas.

## Ensaio da dupla

Cada pessoa deve conseguir executar o roteiro e explicar os cinco arquivos centrais (`kahn`, `mochila`, `grafo`, `dijkstra`, `prim`) e o resumo. Alternar quem demonstra e quem responde. Depois, trocar os papéis.

- [ ] Mariana executou o roteiro completo e explicou as decisões sem ler respostas prontas.
- [ ] João executou o roteiro completo e explicou as decisões sem ler respostas prontas.
- [ ] Os dois conseguem alterar uma capacidade, uma dependência ou um peso e prever o efeito.
- [ ] Horário final de entrega e data/horário da apresentação conferidos no canal da disciplina.

Esses itens dependem do ensaio de vocês; os testes automatizados não comprovam domínio individual.
