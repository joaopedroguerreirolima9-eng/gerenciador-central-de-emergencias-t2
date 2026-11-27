# Central de Emergências: Operação Resgate – T2

Trabalho 2 de Algoritmos e Estruturas de Dados II (UFPel).

O projeto amplia o sistema desenvolvido no Trabalho 1, incorporando planejamento de dependências entre atividades, otimização de recursos e análise da rede logística de emergências. O objetivo é apoiar simulações de planejamento operacional e distribuição de recursos por meio de grafos e programação dinâmica.

**Estado do projeto:** módulos 6 a 9 implementados e integrados aos modos de interação, preservando a base do T1. Investigação reúne integridade e dependências; Planejamento Estratégico inclui otimização e resumo regional; Logística permite consultar a rede, Dijkstra e Prim. A regressão final e a revisão dos entregáveis permanecem para T07/T08.

## Integrantes

- João Pedro Guerreiro Lima
- Mariana Ferreira Tica

## Repositórios

- [Trabalho 2 – repositório atual](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2)
- [Trabalho 1 – repositório original](https://github.com/marianatica/gerenciador-central-de-emergencias)

## Como executar

Pré-requisito: compilador **GCC**. Execute os comandos na pasta raiz do projeto.

**Windows (PowerShell):**

```powershell
.\build.bat
```

**Linux / macOS:**

```bash
gcc -Iinclude src/main.c -o central
./central
```

O programa carrega o CSV e mantém os módulos 1 a 5. A opção 4 reúne as investigações de integridade e dependências; a opção 6 mantém o acesso direto às dependências. A opção 7 executa a otimização e o resumo regional. A opção 8 abre o modo Logística.

| Opção | Funcionalidade |
|---|---|
| 1 | Central de Ocorrências |
| 2 | Consulta Rápida |
| 3 | Organização das Ocorrências |
| 4 | Investigação: submenu 1 para integridade (T1), submenu 2 para dependências e ciclos (T2) |
| 5 | Operação Resgate com algoritmo guloso (T1) |
| 6 | Planejamento Operacional: dependências (T2) |
| 7 | Planejamento Estratégico: otimização e resumo regional (T2) |
| 8 | Logística: regiões, conexões, Dijkstra e Prim (T2) |
| 0 | Sair |

## Fonte de dados

| Item | Descrição |
|---|---|
| Dataset | [911 Calls – Emergency Dataset (Kaggle)](https://www.kaggle.com/datasets/mchirico/montcoalert) |
| Localidade | Montgomery County, Pensilvânia, EUA |
| Arquivo | `data/ocorrencias_911.csv` |
| Base inicial | 150 registros reutilizados do T1 |
| Campos | Tipo, descrição, região, endereço, CEP, latitude, longitude e data/hora |

O sistema mantém os identificadores (IDs) gerados no T1. As atividades e dependências do Módulo 6 são simuladas, assim como os tempos e benefícios do Módulo 7, definidos abaixo. As conexões logísticas do Módulo 8 são geradas por uma regra didática descrita na modelagem da rede.

## Módulos do Trabalho 2

| Módulo | Técnica proposta | Situação |
|---|---|---|
| 6 – Planejamento Operacional | Ordenação topológica (Kahn) | Implementado |
| 7 – Operação Estratégica | Programação dinâmica (Mochila 0/1) | Implementado |
| 8 – Rede de Emergências | Dijkstra e Prim | Implementado e integrado ao modo Logística |
| 9 – Laboratório de Inovação | Resumo estratégico por região | Implementado e integrado ao módulo 7 |

### Módulo 6 – Planejamento Operacional: Dependências de Atendimento

Representar atividades operacionais e suas dependências em um grafo direcionado, permitindo cadastrar dependências, identificar ciclos e apresentar uma sequência válida de execução quando possível.

**Modelagem:** vértices representam atividades e uma aresta `A -> B` indica que A deve ser concluída antes de B. O grafo direcionado utiliza uma matriz de adjacência, com capacidade para 100 atividades. O cenário inicial simulado contém 30 atividades e 54 dependências, distribuídas em seis fases. O menu permite cadastrar atividades, cadastrar/remover dependências, listar os dados e restaurar o cenário inicial. Alterações nas dependências são mantidas em memória durante a execução.

**Algoritmo:** ordenação topológica de Kahn. Utiliza os graus de entrada dos vértices e uma fila de atividades sem dependências pendentes. Se nem todos os vértices forem processados, existe um ciclo.

**Justificativa:** permite validar o planejamento e determinar uma ordem que respeite todas as precedências, em tempo `O(V²)` nesta implementação com matriz. A matriz simplifica a consulta, inclusão e remoção de arestas.

Exemplo de teste: na opção 6 do menu principal, escolha 6 (verificar ciclos e ordem topológica). O cenário inicial permite ordenar as 30 atividades. Cadastre a dependência 30 -> 1 pela opção 4 do submenu e execute a verificação novamente: o sistema detecta um ciclo e lista as atividades bloqueadas. Remova a dependência pela opção 5 para restaurar a ordem válida.

### Módulo 7 – Operação Estratégica: Otimização de Recursos

Selecionar um subconjunto de ocorrências que maximize o benefício total sem ultrapassar uma capacidade operacional, informando os registros selecionados e os que ficaram de fora.

**Modelagem:** problema da Mochila 0/1. Cada ocorrência tem custo inteiro positivo (tempo estimado em minutos) e benefício (pontuação de prioridade). O usuário informa a capacidade de 0 a 1440 minutos e pode filtrar por região. Como esses atributos não existem no CSV, são simulados a partir da classificação por categoria reaproveitada do T1:

| Categoria | Tempo (min) | Benefício |
|---|---:|---:|
| EMS | 30 | 4 |
| Fire | 45 | 3 |
| Traffic | 20 | 2 |
| Outras | 15 | 1 |

Os valores são parâmetros didáticos, não estimativas clínicas. O limite de capacidade controla o tamanho da tabela de programação dinâmica.

**Algoritmo:** programação dinâmica bottom-up. O estado `dp[i][c]` representa o maior benefício possível considerando as primeiras `i` ocorrências e a capacidade `c`. Cada ocorrência pode ser incluída uma única vez ou deixada de fora.

```text
Se custo[i] > c:
    dp[i][c] = dp[i-1][c]
Caso contrário:
    dp[i][c] = max(dp[i-1][c], beneficio[i] + dp[i-1][c-custo[i]])
```

Os casos-base com zero ocorrências ou capacidade zero têm benefício zero. A reconstrução da solução identifica os IDs selecionados. Em empate, não inclui o item da linha atual; os candidatos são ordenados por ID antes da otimização.

**Justificativa:** encontra uma solução ótima para o modelo discreto adotado, ao contrário de uma escolha exclusivamente gulosa. A tabela completa tem tempo e espaço `O(n * C)`.

Exemplo executado com a base de dados: menu principal 7, submenu 1, região `NORRISTOWN`, capacidade `60`.

```text
Selecionadas: IDs 4 e 19
Selecionadas: 2 de 14 | Tempo usado: 60/60 min | Restante: 0 min | Beneficio maximo: 8
```

As outras 12 ocorrências candidatas são listadas como não selecionadas.

O módulo destina-se a simulações e não substitui protocolos oficiais de atendimento de emergências.

### Módulo 8 – Rede de Emergências: O Desafio Logístico

O enunciado exige duas entre três técnicas: caminhos mínimos, árvore geradora mínima e fluxo máximo. A proposta é implementar as duas primeiras.

**Modelagem definida na T02 e construção implementada na T03:** grafo não direcionado e ponderado, representado por matriz de adjacência, com uma região por vértice. A amostra atual contém **150 ocorrências e 41 regiões distintas**. A inicialização utiliza todas as 41 regiões, com limite de 100 vértices, reutilizando os registros já carregados na Tabela Hash do T1.

Cada região será posicionada pela média aritmética das latitudes e longitudes de suas ocorrências. Esse ponto representa a amostra de chamados, sem identificar uma base ou hospital. Os nomes serão preservados como no CSV e ordenados por `strcmp`, fixando os índices internos de 0 a 40; no menu, os IDs serão de 1 a 41. O carregamento atual não contém regiões vazias nem coordenadas inválidas. A inicialização deverá informar erro nesses casos ou se houver mais de 100 regiões, sem descartar registros silenciosamente.

**Conexões simuladas:** para cada índice `i`, ligar a região aos índices `(i + 1) % V` e `(i + 2) % V`, nos dois sentidos. Não incluir laços nem contar duplicatas. Para as 41 regiões atuais, a regra produz **82 arestas não direcionadas**. As conexões com o sucessor formam um anel que garante conectividade; as demais oferecem caminhos alternativos. Com menos de cinco regiões, eliminar repetições; uma região isolada tem zero arestas. A ordem alfabética define um cenário didático reproduzível e não representa proximidade geográfica ou vias reais.

**Pesos:** reutilizar `distancia_km` do módulo guloso do T1 entre os pontos médios regionais. A fórmula existente é `111 * abs(latA - latB) + 85 * abs(lngA - lngB)`. Converter a aproximação em km para centésimos de km com `custo = (int)(distancia * 100 + 0.5)` e exibir `custo / 100.0` em km. Não há estimativa de tempo de viagem. As coordenadas vêm do dataset; as conexões e sua interpretação logística são simuladas.

Na matriz, `-1` representa ausência de conexão, inclusive na diagonal; zero é um peso permitido. Rejeitar pesos negativos, laços e arestas duplicadas. O limite por aresta será `(INT_MAX - 1) / MAX_PONTOS_REDE`, reservando `INT_MAX` para infinito e permitindo acumular os custos de caminhos simples e árvores sem estouro. Uma conexão A–B conta como uma aresta, embora ocupe as posições `[A][B]` e `[B][A]` da matriz.

**Integração:** a rede é inicializada a partir da Hash após carregar o CSV e permanece como cenário da sessão, sem modificar ocorrências, índices ou IDs do T1. Cadastro, alteração e remoção de ocorrências durante a sessão não reconstroem automaticamente esse cenário, comportamento informado no menu. O módulo 8 usa `include/rede.h` e `src/rede.c`, incluído uma única vez em `main.c`. Se os dados iniciais impedirem a construção da rede, o sistema informa a indisponibilidade da Logística e mantém os outros modos acessíveis.

O cabeçalho `rede.h` define `Rede`, `PontoRede`, `CaminhoRede`, `ArestaRede`, `ArvoreRede` e os contratos de construção, Dijkstra, Prim e menu. A implementação está em `src/rede.c`. Os testes reutilizam a unidade de compilação do programa principal, incluindo a implementação da rede uma única vez.

No menu principal, escolher **8 — Logística**:

| Opção da Logística | Operação |
|---|---|
| 1 | Listar regiões com IDs a partir de 1 |
| 2 | Listar cada conexão não direcionada uma vez, com custo em km |
| 3 | Consultar caminho mínimo por IDs de origem e destino; `0` cancela a consulta |
| 4 | Consultar árvore geradora mínima, suas conexões e custo total |
| 0 | Voltar ao menu principal |

Exemplo executado: **8 → 3 → origem 1 (ABINGTON) → destino 22 (NORRISTOWN)** exibe o percurso e **191,68 km**. Ainda na Logística, a opção **4** exibe as **40 arestas** da árvore e custo total **719,23 km**. O programa informa destino inalcançável ou rede desconexa quando não há solução completa e não apresenta resultados parciais como solução.

Em empate, os algoritmos deverão escolher o menor índice ao selecionar o próximo vértice e atualizar o predecessor somente quando houver melhoria estrita de custo, tornando os resultados reproduzíveis.

#### Caminhos mínimos – Dijkstra

Implementado em `rede_dijkstra`: determina o caminho de menor custo entre dois pontos e preenche `CaminhoRede` com os vértices em ordem e o custo total. Retorna `REDE_OK`, `REDE_SEM_SOLUCAO` para destino inalcançável ou `REDE_ERRO` para argumentos inválidos. Em falha, o resultado fica vazio; origem igual ao destino gera um vértice e custo zero. O menu valida os IDs antes de convertê-los para índices internos.

**Justificativa:** Dijkstra é adequado para custos não negativos e permite reconstruir o caminho por meio dos predecessores. Com matriz de adjacência, o tempo é `O(V²)`.

Teste pequeno: custos A–B = 4, B–C = 3 e A–C = 10 resultam no caminho A–B–C, com custo 7 nas unidades inteiras do teste. Na rede simulada do CSV, o teste de ABINGTON a NORRISTOWN reconstrói:

```text
ABINGTON -> WHITEMARSH -> WEST NORRITON -> UPPER SALFORD -> UPPER MERION
-> UPPER GWYNEDD -> TOWAMENCIN -> TELFORD -> SKIPPACK -> ROCKLEDGE
-> PLYMOUTH -> NORRISTOWN | 191.68 km
```

O valor é o custo mínimo na topologia didática de anel e conexões adicionais; não é uma rota rodoviária real. Os 1.681 pares de origem/destino da rede foram comparados com Floyd-Warshall, usado apenas como referência nos testes.

#### Árvore geradora mínima – Prim

Implementado em `rede_prim`: determina as conexões necessárias para interligar toda a rede com o menor custo total, sem ciclos. Preenche `ArvoreRede` com as arestas selecionadas e o custo total. Retorna `REDE_OK` em sucesso, `REDE_SEM_SOLUCAO` para rede vazia ou desconexa e `REDE_ERRO` para argumentos inválidos. Um único vértice tem árvore válida com zero arestas e custo zero. O resultado fica vazio em falha, descartando qualquer árvore parcial.

O algoritmo começa no índice 0 e mantém, para cada vértice ainda fora da árvore, a aresta de menor custo que o liga à parte já construída. A cada passo, inclui o vértice com a conexão mais barata. Em empate, escolhe o menor índice e preserva o predecessor anterior quando não há melhoria estrita. Diferentemente de Dijkstra, compara o custo de uma aresta com a árvore, sem somar o custo de um percurso a partir da origem.

**Justificativa:** Prim permite reutilizar a mesma representação de grafo de Dijkstra e possui tempo `O(V²)` com matriz de adjacência.

Na rede simulada de **41 vértices e 82 arestas**, Prim selecionou **40 arestas**, com custo total de **719,23 km**. Os testes conferem o custo com Kruskal e verificam que todas as regiões estão conectadas, nenhuma aresta cria ciclo e a soma dos pesos corresponde ao total informado. O valor representa a soma das conexões da árvore na topologia didática, sem interpretação de rota rodoviária real. A árvore e seus custos são exibidos na opção 4 da Logística.

Exemplo ilustrativo: A–B (4 km), A–C (10 km) e B–C (3 km). Dijkstra encontra A–B–C como caminho mínimo de A até C (7 km); Prim seleciona A–B e B–C para conectar todos os vértices (7 km).

### Módulo 9 – Laboratório de Inovação: Funcionalidade Complementar

**Implementado na T05:** resumo estratégico por região das ocorrências selecionadas pelo Módulo 7. A necessidade atendida é mostrar como a seleção distribui atendimentos, tempo e benefício entre as regiões, além do total geral da mochila.

A funcionalidade usa os mesmos vetores `itens` e `escolhido` que produziram a solução da mochila, sem executar outra otimização. Recupera a região de cada ocorrência selecionada pelo ID na Hash, agrupa os resultados e exibe quantidade de atendimentos, minutos e benefício por região, em ordem alfabética. Todas as regiões empatadas com mais atendimentos são destacadas. O custo e o benefício vêm dos itens selecionados, preservando exatamente os valores usados na otimização.

O contrato está em `include/resumo.h` e a implementação em `src/resumo.c`: `resumo_calcular` constrói o resultado, `resumo_mostrar` o apresenta e `resumo_liberar` libera o vetor de regiões. O relatório é exibido imediatamente após uma otimização bem-sucedida, antes de liberar os vetores da seleção. Não guarda um resultado antigo entre consultas. Região vazia aparece como `(sem regiao)`; uma seleção vazia tem totais zero e nenhuma região líder. IDs selecionados ausentes ou dados inválidos fazem o cálculo retornar erro, sem apresentar um relatório parcial.

Exemplo executado pelo terminal: menu principal **7**, opção **1**, região `NORRISTOWN`, capacidade `60`. Após os IDs 4 e 19 selecionados pela mochila, o módulo 9 exibe:

```text
=== MODULO 9: RESUMO ESTRATEGICO POR REGIAO ===
Regiao: NORRISTOWN | Selecionadas: 2 | Tempo: 60 min | Beneficio: 8 pontos
Total do resumo: 2 ocorrencia(s) | 60 min | 8 pontos
Regiao(oes) com mais atendimentos (2):
- NORRISTOWN
```

Com região em branco e capacidade 60, o teste selecionou uma ocorrência de HATFIELD TOWNSHIP e uma de NEW HANOVER, ambas destacadas com um atendimento. O total continua sendo 2 ocorrências, 60 minutos e 8 pontos. Capacidade zero gera um resumo vazio, sem reaproveitar uma seleção anterior.

## Integração com o Trabalho 1

Os módulos 1 a 5 permanecem disponíveis. A Tabela Hash é o armazenamento principal das ocorrências, e as Árvores B+ fornecem índices por tipo, região e data. O Algoritmo Guloso do T1 continua responsável pela ordenação de atendimentos, enquanto o Módulo 7 resolve a seleção ótima sob uma restrição de capacidade.

O Módulo 7 reutiliza os registros da Tabela Hash, o índice B+ por região e a classificação do algoritmo guloso. A consulta rápida também apresenta os dados simulados de cada ocorrência e permite filtrar por prioridade. A opção 4 agora reúne integridade do T1 e dependências do T2 em um submenu, sem reimplementar essas operações. O acesso direto da opção 6 utiliza a mesma estrutura de dependências: alterações feitas por um acesso são vistas pelo outro. O módulo 9 usa os IDs selecionados e a Hash para montar o resumo; o módulo 8 constrói sua rede sobre as regiões da mesma base e é acessado pela opção 8. A documentação detalhada das funcionalidades anteriores está no [README do T1](https://github.com/marianatica/gerenciador-central-de-emergencias).

## Modos de interação

| Modo | Funcionalidades e situação |
|---|---|
| Investigação – Análise Operacional | Opção 4: integridade no submenu 1; dependências e ciclos no submenu 2. A opção 6 mantém acesso direto às mesmas dependências. |
| Planejamento Estratégico | Seleção de ocorrências sob restrições e resumo regional da mesma seleção, exibido automaticamente (implementados) |
| Logística – Rede de Emergências | Opção 8: regiões, conexões e custos, caminhos mínimos e árvore geradora mínima |
| Consulta Rápida | Reutilização das consultas do T1 e acesso aos dados do Módulo 7 (implementado) |

## Análise de complexidade

Complexidades dos algoritmos implementados:

| Algoritmo | Tempo | Espaço auxiliar |
|---|---|---|
| Kahn (matriz de adjacência) | `O(V²)` | `O(V)` |
| Mochila 0/1 (tabela completa) | `O(n * C)` | `O(n * C)` |
| Dijkstra (matriz de adjacência) | `O(V²)` | `O(V)` |
| Prim (matriz de adjacência) | `O(V²)` | `O(V)` |
| Resumo regional (busca linear de grupos e ordenação) | `O(n + s * r + r log r)` | `O(s)` |

`V`: vértices; `E`: arestas; `n`: ocorrências candidatas; `C`: capacidade inteira. O espaço auxiliar não inclui a representação do grafo: listas de adjacência usam `O(V + E)` e matrizes usam `O(V²)`. A Mochila 0/1 tem custo pseudopolinomial em relação à capacidade.

No resumo, `s` é o número de selecionadas e `r` o número de regiões dessas ocorrências. O tempo considera custo médio `O(1)` nas consultas por ID à Hash; colisões podem aumentar esse custo. O vetor reserva no máximo `s` posições, sem novo limite fixo de regiões. Nenhuma ocorrência selecionada implica resultado vazio, sem alocação do vetor.

## Testes e validação

Validação local executada em **26/11/2026**, no **Windows com GCC 15.2.0**, sem avisos na compilação com `-std=c11 -Wall -Wextra`. O arquivo `tests/teste_algoritmos.c` cobre os algoritmos e a integração com o CSV; os menus foram verificados separadamente por entradas de terminal. Resultados:

- [x] Módulo 6: 30 vértices, 54 arestas, ordem válida, ciclo, laço, duplicata, ID inválido e vértice isolado.
- [x] Módulo 7: capacidade zero/inválida, reconstrução e 200 comparações com força bruta; teste interativo com a região NORRISTOWN.
- [x] CSV com 150 registros, consulta pelo índice regional e seleção dos IDs 4 e 19 para NORRISTOWN, com 60 minutos e 8 pontos.
- [x] Abertura e retorno dos sete menus; ordem inicial, inclusão de ciclo e remoção da dependência pelo menu do Módulo 6.
- [x] Módulo 8: construção da rede, caminhos mínimos e destino inalcançável (T03).
- [x] Módulo 8: árvore geradora mínima e rede desconexa (T04).
- [x] Módulo 8: integração pelo menu (T06).
- [x] Módulo 9: totais regionais e gerais iguais aos da seleção da mochila, empates, região vazia e seleção vazia (T05).
- [ ] Revisar todos os módulos 1 a 5 após a integração final.
- [ ] Registrar dimensões e resultados da rede logística após implementação.

Os testes de Kahn conferem a precedência de todas as arestas na ordem gerada. Os testes da mochila usam enumeração de subconjuntos como referência independente e conferem o custo e o benefício da seleção reconstruída. Os 200 casos usam semente fixa para reprodução.

Para executar os testes dos módulos 6 e 7, a partir da raiz do projeto:

**Windows (PowerShell):**

```powershell
gcc -std=c11 -Wall -Wextra -Iinclude tests/teste_algoritmos.c -o teste_t2.exe
.\teste_t2.exe
```

**Linux / macOS:**

```bash
gcc -std=c11 -Wall -Wextra -Iinclude tests/teste_algoritmos.c -o teste_t2
./teste_t2
```

O executável deve terminar com `Todos os testes passaram.`. As verificações usam `assert`; compile sem `-DNDEBUG`. O teste reutiliza a unidade de compilação de `src/main.c`, renomeando sua função de entrada para executar os algoritmos sem iniciar os menus. O comando Linux/macOS não foi executado nesta validação local.

Para executar os testes da rede e de Dijkstra no Windows:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Iinclude tests/teste_rede.c -o teste_rede.exe
.\teste_rede.exe
```

Validação da T03 no Windows com GCC 15.2.0, sem avisos:

- [x] Construção a partir da Hash: médias regionais, ordem alfabética, 41 vértices e 82 arestas; base original preservada.
- [x] Redes com 0 a 5 regiões; rejeição de dados inválidos e mais de 100 regiões.
- [x] Dijkstra: caminho reconstruído, origem igual ao destino, destino isolado, peso zero, empate e argumentos inválidos.
- [x] Caminho com 100 vértices e pesos máximos, verificando limite de custo acumulado.
- [x] Comparação com Floyd-Warshall em 20 grafos pequenos e nos 1.681 pares da rede do CSV, incluindo conferência das arestas e do custo reconstruído.

O executável deve terminar com `Todos os testes da rede passaram.`. Floyd-Warshall é exclusivo da validação; Dijkstra continua sendo o algoritmo de caminhos mínimos da implementação. A construção da rede tem custo `O(N * V + V² + V log V)`, incluindo agrupamento linear, ordenação dos nomes e inicialização da matriz; `N` é o número de ocorrências. Usa `O(V)` de espaço auxiliar além da própria matriz.

Validação da T04 no mesmo executável e ambiente:

- [x] Prim: triângulo de custo conhecido, rede vazia, vértice único, região isolada, empates, pesos zero e argumentos inválidos.
- [x] Resultado vazio quando a rede é desconexa, mesmo após selecionar conexões de uma componente.
- [x] Comparação de custo com Kruskal em 20 grafos pequenos, nas redes de 0 a 5 regiões, em um caminho de 100 vértices com pesos máximos e na rede do CSV.
- [x] Verificação de `V - 1` arestas, existência e custo das conexões, ausência de ciclos e conectividade de todos os vértices.
- [x] Rede do CSV: 40 arestas, total de 71923 centésimos de km (719,23 km).

Kruskal é usado apenas nos testes como referência independente; Prim é a implementação de árvore geradora mínima do módulo 8. A suíte de Dijkstra e os testes dos módulos 6 e 7 também foram executados novamente com sucesso.

Para executar os testes do módulo 9 no Windows:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Iinclude tests/teste_resumo.c -o teste_resumo.exe
.\teste_resumo.exe
```

Validação da T05, no Windows com GCC 15.2.0, sem avisos:

- [x] Quantidades, minutos e benefícios por região e no total, preservando os itens e a máscara da mochila.
- [x] Várias regiões, todos os empates, região sem nome e seleção vazia.
- [x] Argumentos inválidos, ID selecionado ausente, máscara inválida e proteção contra estouro na soma.
- [x] NORRISTOWN/60 e 21 capacidades entre 0 e 300 minutos na base completa.
- [x] Fluxo pelo menu: resumo automático, capacidade zero, seleção com várias regiões, região inexistente e duas otimizações consecutivas.

O executável termina com `Todos os testes do resumo passaram.`. Os testes anteriores dos módulos 6/7 e da rede também passaram após essa integração.

Validação da T06 em **27/11/2026**, no Windows com GCC, sem avisos em `-std=c11 -Wall -Wextra -Werror`:

- [x] Abertura e retorno dos oito acessos principais, incluindo os dois submenus da Investigação.
- [x] Dependências compartilhadas entre a Investigação e a opção 6; ciclo criado por um acesso e removido pelo outro.
- [x] Listagem de 41 regiões e 82 conexões, caminho mínimo e árvore geradora com os custos esperados pelo terminal.
- [x] Origem igual ao destino, IDs inválidos, cancelamento, opção inválida e encerramento da entrada.
- [x] Otimização com resumo, Consulta Rápida e manutenção do cenário logístico após cadastrar uma ocorrência.
- [x] CSV vazio e falha de inicialização da rede sem bloquear as funcionalidades do T1.

Os dez testes de terminal são reproduzíveis com **Python 3**, sem bibliotecas externas. Python é necessário apenas para esta automação de testes; o programa continua sendo compilado e executado em C:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Iinclude src/main.c -o central.exe
python tests/teste_menus.py
```

Os CSVs alternativos usados nessa suíte são criados em diretórios temporários, preservando a amostra do projeto. As três suítes C também passaram novamente após a integração.

## Próximas etapas

- Executar a regressão final de cadastro, alteração, remoção, índices e demais operações do T1 (T07).
- Revisar os entregáveis, testar uma cópia limpa e preparar a apresentação (T08).

## Bibliotecas utilizadas

A base do T1 utiliza a biblioteca padrão de C (`stdio.h`, `stdlib.h` e `string.h`). Os módulos 6 e 7 utilizam a biblioteca padrão de C para entrada/saída, alocação de memória, manipulação de strings, limites de inteiros e ordenação de candidatos por ID (`qsort`). Kahn e a mochila foram implementados diretamente em C, sem bibliotecas externas de grafos ou otimização.

O módulo 8 usa também `math.h` para rejeitar coordenadas não finitas com `isfinite`; não utiliza bibliotecas externas de grafos. A distância é calculada pela função do T1, sem API ou consulta de mapas.

O módulo 9 usa alocação/liberação de memória, manipulação de strings e `qsort` da biblioteca padrão, além das consultas à Hash implementada no T1.
