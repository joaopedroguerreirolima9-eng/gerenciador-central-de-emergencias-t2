# Central de Emergências: Operação Resgate – T2

Trabalho 2 de Algoritmos e Estruturas de Dados II (UFPel).

O projeto amplia o sistema desenvolvido no Trabalho 1, incorporando planejamento de dependências entre atividades, otimização de recursos e análise da rede logística de emergências. O objetivo é apoiar simulações de planejamento operacional e distribuição de recursos por meio de grafos e programação dinâmica.

**Estado do projeto:** base do T1 reutilizada; módulos 6 a 9 em planejamento. As escolhas de algoritmos, modelagens e exemplos deste documento são propostas de desenvolvimento, não resultados de implementação ou testes já realizados.

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

Esses comandos correspondem à versão inicial, herdada do T1, que carrega o CSV e apresenta os módulos 1 a 5. As instruções serão revisadas após a integração dos novos módulos.

## Fonte de dados

| Item | Descrição |
|---|---|
| Dataset | [911 Calls – Emergency Dataset (Kaggle)](https://www.kaggle.com/datasets/mchirico/montcoalert) |
| Localidade | Montgomery County, Pensilvânia, EUA |
| Arquivo | `data/ocorrencias_911.csv` |
| Base inicial | 150 registros reutilizados do T1 |
| Campos | Tipo, descrição, região, endereço, CEP, latitude, longitude e data/hora |

O sistema mantém os identificadores (IDs) gerados no T1. Dados adicionais necessários aos novos módulos, como atividades operacionais, custos de atendimento, benefícios e conexões logísticas, deverão ter sua origem e regras de geração documentadas. Valores simulados serão identificados como tais.

## Módulos do Trabalho 2

| Módulo | Técnica proposta | Situação |
|---|---|---|
| 6 – Planejamento Operacional | Ordenação topológica (Kahn) | Planejado |
| 7 – Operação Estratégica | Programação dinâmica (Mochila 0/1) | Planejado |
| 8 – Rede de Emergências | Dijkstra e Prim | Planejado |
| 9 – Laboratório de Inovação | Resumo estratégico dos atendimentos | Planejado |

### Módulo 6 – Planejamento Operacional: Dependências de Atendimento

Representar atividades operacionais e suas dependências em um grafo direcionado, permitindo cadastrar dependências, identificar ciclos e apresentar uma sequência válida de execução quando possível.

**Modelagem proposta:** vértices representam atividades, e uma aresta `A -> B` indica que A deve ser concluída antes de B. O grafo será representado por listas de adjacência.

**Algoritmo proposto:** ordenação topológica de Kahn. Utiliza os graus de entrada dos vértices e uma fila de atividades sem dependências pendentes. Se nem todos os vértices forem processados, existe um ciclo.

**Justificativa:** permite validar o planejamento e determinar uma ordem que respeite todas as precedências, em tempo `O(V + E)`.

Exemplo ilustrativo: `Identificar região -> Designar equipe -> Encaminhar atendimento`. Uma dependência do último passo para o primeiro forma um ciclo e impede uma ordenação válida.

### Módulo 7 – Operação Estratégica: Otimização de Recursos

Selecionar um subconjunto de ocorrências que maximize o benefício total sem ultrapassar uma capacidade operacional, informando os registros selecionados e os que ficaram de fora.

**Modelagem proposta:** problema da Mochila 0/1. Cada ocorrência terá um custo inteiro positivo (tempo estimado em minutos) e um benefício (pontuação de prioridade). A capacidade será o tempo total disponível. Esses atributos não existem no CSV original; as regras de simulação ainda deverão ser definidas e justificadas.

**Algoritmo proposto:** programação dinâmica. O estado `dp[i][c]` representa o maior benefício possível considerando as primeiras `i` ocorrências e a capacidade `c`. Cada ocorrência pode ser incluída uma única vez ou deixada de fora.

```text
Se custo[i] > c:
    dp[i][c] = dp[i-1][c]
Caso contrário:
    dp[i][c] = max(dp[i-1][c], beneficio[i] + dp[i-1][c-custo[i]])
```

Os casos-base com zero ocorrências ou capacidade zero têm benefício zero. A reconstrução da solução identificará os IDs selecionados.

**Justificativa:** encontra uma solução ótima para o modelo discreto adotado, ao contrário de uma escolha exclusivamente gulosa. A tabela completa tem tempo e espaço `O(n * C)`.

Exemplo ilustrativo: capacidade de 60 minutos; A (40 min, 8 pontos), B (30 min, 7 pontos) e C (30 min, 6 pontos). A solução ótima seleciona B e C, obtendo 13 pontos em 60 minutos. A fica de fora.

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

**Proposta:** resumo estratégico das ocorrências selecionadas pelo Módulo 7, permitindo analisar como os recursos disponíveis foram distribuídos.

O relatório deverá apresentar a quantidade de ocorrências selecionadas e não selecionadas, tempo utilizado e restante, benefício total e distribuição por categoria. Os dados completos serão recuperados pelo ID das ocorrências na Tabela Hash existente.

Exemplo ilustrativo: para o cenário do Módulo 7, o relatório apresentaria 2 de 3 ocorrências selecionadas, 60 minutos utilizados, 0 minutos restantes e benefício total de 13 pontos.

## Integração com o Trabalho 1

Os módulos 1 a 5 permanecem disponíveis. A Tabela Hash é o armazenamento principal das ocorrências, e as Árvores B+ fornecem índices por tipo, região e data. O Algoritmo Guloso do T1 continua responsável pela ordenação de atendimentos, enquanto o Módulo 7 resolve a seleção ótima sob uma restrição de capacidade.

Os novos módulos deverão reutilizar os registros e consultas existentes por meio dos IDs, evitando duplicar a base principal. A documentação detalhada das funcionalidades anteriores está no [README do T1](https://github.com/marianatica/gerenciador-central-de-emergencias).

## Modos de interação

| Modo | Funcionalidades previstas |
|---|---|
| Investigação – Análise Operacional | Verificação de dependências inválidas e ciclos; reutilização da investigação do T1 |
| Planejamento Estratégico | Seleção de ocorrências sob restrições e resumo dos recursos utilizados |
| Logística – Rede de Emergências | Consulta de caminhos mínimos, conexões e custos da infraestrutura |
| Consulta Rápida | Reutilização das consultas do T1 e acesso aos registros utilizados pelos novos módulos |

## Análise de complexidade

Complexidades esperadas para as implementações propostas:

| Algoritmo | Tempo | Espaço auxiliar |
|---|---|---|
| Kahn (lista de adjacência) | `O(V + E)` | `O(V)` |
| Mochila 0/1 (tabela completa) | `O(n * C)` | `O(n * C)` |
| Dijkstra (matriz de adjacência) | `O(V²)` | `O(V)` |
| Prim (matriz de adjacência) | `O(V²)` | `O(V)` |

`V`: vértices; `E`: arestas; `n`: ocorrências candidatas; `C`: capacidade inteira. O espaço auxiliar não inclui a representação do grafo: listas de adjacência usam `O(V + E)` e matrizes usam `O(V²)`. A Mochila 0/1 tem custo pseudopolinomial em relação à capacidade.

## Testes e validação

Testes planejados, ainda não executados para o T2:

- [ ] Módulo 6: grafo sem ciclos, grafo com ciclos e dependências inválidas.
- [ ] Módulo 7: capacidade zero, capacidade insuficiente e seleção ótima em instâncias pequenas.
- [ ] Módulo 8: caminhos mínimos, destino inalcançável, árvore geradora mínima e rede desconexa.
- [ ] Módulo 9: conferir os totais do relatório com o resultado da otimização.
- [ ] Verificar o funcionamento dos módulos 1 a 5 após a integração.
- [ ] Registrar dimensões reais das redes, origem dos dados, exemplos reais de entrada e saída e resultados dos testes.

## Bibliotecas utilizadas

A base do T1 utiliza a biblioteca padrão de C (`stdio.h`, `stdlib.h` e `string.h`). As estruturas e os algoritmos centrais do T2 serão implementados pela dupla. Bibliotecas adicionais de apoio, se utilizadas, serão documentadas e justificadas.
