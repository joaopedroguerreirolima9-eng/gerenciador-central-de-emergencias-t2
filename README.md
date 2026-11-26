# Central de Emergências: Operação Resgate – T2

Trabalho 2 de Algoritmos e Estruturas de Dados II (UFPel).

O projeto amplia o sistema desenvolvido no Trabalho 1, incorporando planejamento de dependências entre atividades, otimização de recursos e análise da rede logística de emergências. O objetivo é apoiar simulações de planejamento operacional e distribuição de recursos por meio de grafos e programação dinâmica.

**Estado do projeto:** base do T1 reutilizada; módulos 6 e 7 implementados e testados nesta versão parcial. O módulo 8 permanece em planejamento e o módulo 9 possui uma proposta definida, ainda não implementada.

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

O programa carrega o CSV e mantém os módulos 1 a 5. No menu principal, a opção 6 permite acessar o Módulo 6 (dependências) e a opção 7 permite acessar o Módulo 7 (otimização).

| Opção | Funcionalidade |
|---|---|
| 1 | Central de Ocorrências |
| 2 | Consulta Rápida |
| 3 | Organização das Ocorrências |
| 4 | Investigação de integridade dos registros (T1) |
| 5 | Operação Resgate com algoritmo guloso (T1) |
| 6 | Planejamento Operacional: dependências (T2) |
| 7 | Planejamento Estratégico: otimização (T2) |
| 0 | Sair |

## Fonte de dados

| Item | Descrição |
|---|---|
| Dataset | [911 Calls – Emergency Dataset (Kaggle)](https://www.kaggle.com/datasets/mchirico/montcoalert) |
| Localidade | Montgomery County, Pensilvânia, EUA |
| Arquivo | `data/ocorrencias_911.csv` |
| Base inicial | 150 registros reutilizados do T1 |
| Campos | Tipo, descrição, região, endereço, CEP, latitude, longitude e data/hora |

O sistema mantém os identificadores (IDs) gerados no T1. As atividades e dependências do Módulo 6 são simuladas, assim como os tempos e benefícios do Módulo 7, definidos abaixo. As conexões logísticas ainda serão modeladas.

## Módulos do Trabalho 2

| Módulo | Técnica proposta | Situação |
|---|---|---|
| 6 – Planejamento Operacional | Ordenação topológica (Kahn) | Implementado |
| 7 – Operação Estratégica | Programação dinâmica (Mochila 0/1) | Implementado |
| 8 – Rede de Emergências | Dijkstra e Prim | Modelagem e contrato definidos; implementação pendente |
| 9 – Laboratório de Inovação | Resumo estratégico por região | Proposta definida |

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

**Modelagem definida na T02 (implementação pendente):** grafo não direcionado e ponderado, representado por matriz de adjacência, com uma região por vértice. A amostra atual contém **150 ocorrências e 41 regiões distintas**. Serão utilizadas todas as 41 regiões, com limite de 100 vértices, reutilizando os registros já carregados na Tabela Hash do T1.

Cada região será posicionada pela média aritmética das latitudes e longitudes de suas ocorrências. Esse ponto representa a amostra de chamados, sem identificar uma base ou hospital. Os nomes serão preservados como no CSV e ordenados por `strcmp`, fixando os índices internos de 0 a 40; no menu, os IDs serão de 1 a 41. O carregamento atual não contém regiões vazias nem coordenadas inválidas. A inicialização deverá informar erro nesses casos ou se houver mais de 100 regiões, sem descartar registros silenciosamente.

**Conexões simuladas:** para cada índice `i`, ligar a região aos índices `(i + 1) % V` e `(i + 2) % V`, nos dois sentidos. Não incluir laços nem contar duplicatas. Para as 41 regiões atuais, a regra produz **82 arestas não direcionadas**. As conexões com o sucessor formam um anel que garante conectividade; as demais oferecem caminhos alternativos. Com menos de cinco regiões, eliminar repetições; uma região isolada tem zero arestas. A ordem alfabética define um cenário didático reproduzível e não representa proximidade geográfica ou vias reais.

**Pesos:** reutilizar `distancia_km` do módulo guloso do T1 entre os pontos médios regionais. A fórmula existente é `111 * abs(latA - latB) + 85 * abs(lngA - lngB)`. Converter a aproximação em km para centésimos de km com `custo = (int)(distancia * 100 + 0.5)` e exibir `custo / 100.0` em km. Não há estimativa de tempo de viagem. As coordenadas vêm do dataset; as conexões e sua interpretação logística são simuladas.

Na matriz, `-1` representa ausência de conexão, inclusive na diagonal; zero é um peso permitido. Rejeitar pesos negativos, laços e arestas duplicadas. O limite por aresta será `(INT_MAX - 1) / MAX_PONTOS_REDE`, reservando `INT_MAX` para infinito e permitindo acumular os custos de caminhos simples e árvores sem estouro. Uma conexão A–B conta como uma aresta, embora ocupe as posições `[A][B]` e `[B][A]` da matriz.

**Integração:** a rede será inicializada a partir da Hash após carregar o CSV e permanecerá como cenário da sessão, sem modificar ocorrências, índices ou IDs do T1. Cadastro, alteração e remoção de ocorrências durante a sessão não reconstruirão automaticamente esse cenário. O módulo 8 usará `include/rede.h` e `src/rede.c`; a implementação será incluída uma única vez em `main.c`. O acesso pelo modo Logística será integrado na T06.

O cabeçalho `rede.h` já define `Rede`, `PontoRede`, `CaminhoRede`, `ArestaRede`, `ArvoreRede` e os contratos de construção, Dijkstra, Prim e menu. **As funções ainda não estão implementadas nesta etapa.** O cenário de 41 vértices e 82 arestas foi conferido a partir do CSV durante a modelagem; as saídas dos algoritmos serão verificadas depois da implementação.

Em empate, os algoritmos deverão escolher o menor índice ao selecionar o próximo vértice e atualizar o predecessor somente quando houver melhoria estrita de custo, tornando os resultados reproduzíveis.

#### Caminhos mínimos – Dijkstra

Determina o caminho de menor custo entre dois pontos da rede. A consulta deverá apresentar o percurso, o custo total ou informar que o destino é inalcançável.

**Justificativa:** Dijkstra é adequado para custos não negativos e permite reconstruir o caminho por meio dos predecessores. Com matriz de adjacência, o tempo é `O(V²)`.

#### Árvore geradora mínima – Prim

Determina as conexões necessárias para interligar toda a rede com o menor custo total, sem ciclos. A consulta deverá apresentar as arestas selecionadas e o custo total, ou identificar que a rede é desconexa.

**Justificativa:** Prim permite reutilizar a mesma representação de grafo de Dijkstra e possui tempo `O(V²)` com matriz de adjacência.

Exemplo ilustrativo: A–B (4 km), A–C (10 km) e B–C (3 km). Dijkstra encontra A–B–C como caminho mínimo de A até C (7 km); Prim seleciona A–B e B–C para conectar todos os vértices (7 km).

### Módulo 9 – Laboratório de Inovação: Funcionalidade Complementar

**Proposta (ainda não implementada):** resumo estratégico por região das ocorrências selecionadas pelo Módulo 7, permitindo analisar como os recursos disponíveis foram distribuídos.

A funcionalidade deverá reutilizar o resultado da mochila, recuperar as ocorrências selecionadas pelo ID na Tabela Hash e agrupá-las por região. Para cada região, exibirá a quantidade de atendimentos, os minutos utilizados e os pontos de benefício, destacando a região ou regiões com mais atendimentos selecionados. Não será necessário executar um novo algoritmo de otimização.

Exemplo de saída prevista (não executada no Módulo 9): para o teste do Módulo 7 em NORRISTOWN, o resumo apresentaria 2 ocorrências selecionadas, 60 minutos e 8 pontos nessa região.

## Integração com o Trabalho 1

Os módulos 1 a 5 permanecem disponíveis. A Tabela Hash é o armazenamento principal das ocorrências, e as Árvores B+ fornecem índices por tipo, região e data. O Algoritmo Guloso do T1 continua responsável pela ordenação de atendimentos, enquanto o Módulo 7 resolve a seleção ótima sob uma restrição de capacidade.

O Módulo 7 reutiliza os registros da Tabela Hash, o índice B+ por região e a classificação do algoritmo guloso. A consulta rápida também apresenta os dados simulados de cada ocorrência e permite filtrar por prioridade. Nesta versão, a investigação de integridade do T1 permanece na opção 4; a investigação das dependências e dos ciclos do Módulo 6 está disponível separadamente na opção 6. Os módulos 8 e 9 deverão continuar reutilizando a base existente. A documentação detalhada das funcionalidades anteriores está no [README do T1](https://github.com/marianatica/gerenciador-central-de-emergencias).

## Modos de interação

| Modo | Funcionalidades e situação |
|---|---|
| Investigação – Análise Operacional | Integridade dos registros na opção 4; dependências e ciclos na opção 6. As duas funcionalidades estão implementadas, com acessos separados. |
| Planejamento Estratégico | Seleção de ocorrências sob restrições (implementado); resumo por região (planejado) |
| Logística – Rede de Emergências | Consulta de caminhos mínimos, conexões e custos da infraestrutura (planejado) |
| Consulta Rápida | Reutilização das consultas do T1 e acesso aos dados do Módulo 7 (implementado) |

## Análise de complexidade

Complexidades dos módulos implementados e das técnicas propostas para o Módulo 8:

| Algoritmo | Tempo | Espaço auxiliar |
|---|---|---|
| Kahn (matriz de adjacência) | `O(V²)` | `O(V)` |
| Mochila 0/1 (tabela completa) | `O(n * C)` | `O(n * C)` |
| Dijkstra (matriz de adjacência; planejado) | `O(V²)` | `O(V)` |
| Prim (matriz de adjacência; planejado) | `O(V²)` | `O(V)` |

`V`: vértices; `E`: arestas; `n`: ocorrências candidatas; `C`: capacidade inteira. O espaço auxiliar não inclui a representação do grafo: listas de adjacência usam `O(V + E)` e matrizes usam `O(V²)`. A Mochila 0/1 tem custo pseudopolinomial em relação à capacidade.

## Testes e validação

Validação local executada em **26/11/2026**, no **Windows com GCC 15.2.0**, sem avisos na compilação com `-std=c11 -Wall -Wextra`. O arquivo `tests/teste_algoritmos.c` cobre os algoritmos e a integração com o CSV; os menus foram verificados separadamente por entradas de terminal. Resultados:

- [x] Módulo 6: 30 vértices, 54 arestas, ordem válida, ciclo, laço, duplicata, ID inválido e vértice isolado.
- [x] Módulo 7: capacidade zero/inválida, reconstrução e 200 comparações com força bruta; teste interativo com a região NORRISTOWN.
- [x] CSV com 150 registros, consulta pelo índice regional e seleção dos IDs 4 e 19 para NORRISTOWN, com 60 minutos e 8 pontos.
- [x] Abertura e retorno dos sete menus; ordem inicial, inclusão de ciclo e remoção da dependência pelo menu do Módulo 6.
- [ ] Módulo 8: caminhos mínimos, destino inalcançável, árvore geradora mínima e rede desconexa.
- [ ] Módulo 9: conferir os totais do relatório com o resultado da otimização.
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

## Próximas etapas

- Implementar Dijkstra e Prim e integrar o modo Logística.
- Implementar o resumo por região do Módulo 9.
- Realizar testes finais de integração e atualizar este relatório.

## Bibliotecas utilizadas

A base do T1 utiliza a biblioteca padrão de C (`stdio.h`, `stdlib.h` e `string.h`). Os módulos 6 e 7 utilizam a biblioteca padrão de C para entrada/saída, alocação de memória, manipulação de strings, limites de inteiros e ordenação de candidatos por ID (`qsort`). Kahn e a mochila foram implementados diretamente em C, sem bibliotecas externas de grafos ou otimização.
