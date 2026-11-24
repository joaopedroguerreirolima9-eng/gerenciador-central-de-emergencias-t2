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

O programa carrega o CSV e mantém os módulos 1 a 5. No menu principal, a opção 4 permite acessar o Módulo 6 (dependências) e a opção 5 permite acessar o Módulo 7 (otimização).

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
| 8 – Rede de Emergências | Dijkstra e Prim | Planejado |
| 9 – Laboratório de Inovação | Resumo estratégico por região | Proposta definida |

### Módulo 6 – Planejamento Operacional: Dependências de Atendimento

Representar atividades operacionais e suas dependências em um grafo direcionado, permitindo cadastrar dependências, identificar ciclos e apresentar uma sequência válida de execução quando possível.

**Modelagem:** vértices representam atividades e uma aresta `A -> B` indica que A deve ser concluída antes de B. O grafo direcionado utiliza uma matriz de adjacência, com capacidade para 100 atividades. O cenário inicial simulado contém 30 atividades e 54 dependências, distribuídas em seis fases. O menu permite cadastrar atividades, cadastrar/remover dependências, listar os dados e restaurar o cenário inicial. Alterações nas dependências são mantidas em memória durante a execução.

**Algoritmo:** ordenação topológica de Kahn. Utiliza os graus de entrada dos vértices e uma fila de atividades sem dependências pendentes. Se nem todos os vértices forem processados, existe um ciclo.

**Justificativa:** permite validar o planejamento e determinar uma ordem que respeite todas as precedências, em tempo `O(V²)` nesta implementação com matriz. A matriz simplifica a consulta, inclusão e remoção de arestas.

Exemplo de teste: na opção 4 do menu principal, escolha 2 (dependências) e 6 (verificar). O cenário inicial permite ordenar as 30 atividades. Cadastre a dependência 30 -> 1 pela opção 4 do submenu e execute a verificação novamente: o sistema detecta um ciclo e lista as atividades bloqueadas. Remova a dependência pela opção 5 para restaurar a ordem válida.

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

Exemplo executado com a base de dados: menu principal 5, submenu 1, região `NORRISTOWN`, capacidade `60`.

```text
Selecionadas: IDs 4 e 19
Selecionadas: 2 de 14 | Tempo usado: 60/60 min | Restante: 0 min | Beneficio maximo: 8
```

As outras 12 ocorrências candidatas são listadas como não selecionadas.

O módulo destina-se a simulações e não substitui protocolos oficiais de atendimento de emergências.

### Módulo 8 – Rede de Emergências: O Desafio Logístico

O enunciado exige duas entre três técnicas: caminhos mínimos, árvore geradora mínima e fluxo máximo. A proposta é implementar as duas primeiras.

**Modelagem proposta:** grafo não direcionado e ponderado, representado por matriz de adjacência. Os vértices representarão elementos da infraestrutura (regiões, bases ou pontos de apoio), e as arestas representarão conexões com custos não negativos. A origem dos pesos e das conexões deverá ser documentada. Pretende-se utilizar pelo menos 30 vértices e 50 arestas, quando compatível com a modelagem, conforme a recomendação do enunciado.

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

O Módulo 7 reutiliza os registros da Tabela Hash, o índice B+ por região e a classificação do algoritmo guloso. A consulta rápida também apresenta os dados simulados de cada ocorrência e permite filtrar por prioridade. O Módulo 6 integra o modo de investigação do T1. Os módulos 8 e 9 deverão continuar reutilizando a base existente. A documentação detalhada das funcionalidades anteriores está no [README do T1](https://github.com/marianatica/gerenciador-central-de-emergencias).

## Modos de interação

| Modo | Funcionalidades e situação |
|---|---|
| Investigação – Análise Operacional | Verificação de dependências inválidas e ciclos; reutilização da investigação do T1 (implementado) |
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

Testes executados nesta versão com GCC em Linux:

- [x] Módulo 6: 30 vértices, 54 arestas, ordem válida, ciclo, laço, duplicata, ID inválido e vértice isolado.
- [x] Módulo 7: capacidade zero/inválida, reconstrução e 200 comparações com força bruta; teste interativo com a região NORRISTOWN.
- [ ] Módulo 8: caminhos mínimos, destino inalcançável, árvore geradora mínima e rede desconexa.
- [ ] Módulo 9: conferir os totais do relatório com o resultado da otimização.
- [ ] Revisar todos os módulos 1 a 5 após a integração final.
- [ ] Registrar dimensões e resultados da rede logística após implementação.

Para executar os testes dos módulos 6 e 7:

```bash
gcc -std=c11 -Wall -Wextra -Iinclude tests/teste_algoritmos.c -o teste_t2
./teste_t2
```

No Windows, pode-se gerar `teste_t2.exe` e executar `.\teste_t2.exe`. A versão parcial foi compilada sem avisos no Linux; o Windows não foi testado.

## Próximas etapas

- Implementar Dijkstra e Prim e integrar o modo Logística.
- Implementar o resumo por região do Módulo 9.
- Realizar testes finais de integração e atualizar este relatório.

## Bibliotecas utilizadas

A base do T1 utiliza a biblioteca padrão de C (`stdio.h`, `stdlib.h` e `string.h`). Os módulos 6 e 7 utilizam a biblioteca padrão de C para entrada/saída, alocação de memória, manipulação de strings, limites de inteiros e ordenação de candidatos por ID (`qsort`). Kahn e a mochila foram implementados diretamente em C, sem bibliotecas externas de grafos ou otimização.
