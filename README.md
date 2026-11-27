# Central de Emergências — Operação Resgate

Trabalho 2 de Algoritmos e Estruturas de Dados II (UFPel). **Integrantes:** João Pedro Guerreiro Lima e Mariana Ferreira Tica.

Sistema em C que reutiliza os módulos 1 a 5 do T1 e acrescenta planejamento de atividades, otimização de recursos, rede logística e resumo regional.

## Executar

É necessário ter **GCC no PATH**. Execute na raiz do projeto.

**Windows (PowerShell):**

```powershell
.\build.bat
```

**Linux/macOS:**

```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c -o central
./central
```

`main.c` inclui os demais `.c`; compile apenas esse arquivo. A execução foi validada no Windows com GCC 15.2.0 e não depende de internet.

## Dados e integração com o T1

`data/ocorrencias_911.csv` contém **150 chamados e 41 regiões**, da amostra do [911 Calls — Emergency Dataset](https://www.kaggle.com/datasets/mchirico/montcoalert). São usados tipo, descrição, região, endereço, CEP, coordenadas e data/hora. Os IDs são gerados no carregamento.

A **Hash com encadeamento** (223 posições) guarda os registros e as assinaturas de integridade. Três **árvores B+** indexam tipo, região e data, permitindo buscas ordenadas. Cadastro, alteração, remoção, consultas, integridade e guloso do T1 foram preservados. O T2 reutiliza esses registros, índices, classificação de categoria e função de distância.

## Menus

| Opção | Funcionalidade |
|---|---|
| 1, 3 e 5 | Central de Ocorrências, Organização e Operação Resgate do T1 |
| 2 | Consulta Rápida por ID, descrição, tipo, região, prefixo, data ou prioridade |
| 4 | Investigação: integridade dos registros e dependências/ciclos |
| 6 | Acesso direto às mesmas dependências da Investigação |
| 7 | Planejamento Estratégico: otimização e resumo regional |
| 8 | Logística: regiões, conexões, Dijkstra e Prim |
| 0 | Sair ou voltar ao menu anterior |

## Módulo 6 — Dependências

Grafo direcionado em matriz: vértices são atividades e `A -> B` significa que A precisa terminar antes de B. O cenário simulado tem **30 atividades e 54 dependências**, com cadastro e remoção de arestas.

**Kahn** começa pelas atividades com grau de entrada zero e libera as sucessoras usando uma fila. Produz uma ordem válida ou informa ciclo quando processa menos que o total. A matriz simplifica o cadastro das dependências na escala usada.

## Módulo 7 — Otimização

**Mochila 0/1 por programação dinâmica:** seleciona cada ocorrência no máximo uma vez, maximizando benefício sem ultrapassar o tempo informado (**0 a 1440 minutos**). Pode filtrar por região. Custos e benefícios são simulados: **EMS 30 min/4 pontos; Fire 45/3; Traffic 20/2; outras 15/1**.

`dp[i][c]` guarda o melhor benefício com os primeiros `i` itens e capacidade `c`. Compara excluir o item com incluí-lo, se couber, usando a linha anterior. Os casos-base são zero; a reconstrução identifica os IDs escolhidos. Isso encontra o ótimo do modelo discreto. A saída mostra selecionadas, excluídas, tempo e benefício. Em empate, deixa o item atual de fora; os candidatos são ordenados por ID.

A simulação não substitui protocolos oficiais de priorização e atendimento de emergências.

## Módulo 8 — Rede logística

As duas técnicas escolhidas são **Dijkstra** e **Prim**, sobre um grafo não direcionado e ponderado em matriz, com **41 vértices e 82 arestas**. Cada região vira um vértice posicionado pela média das coordenadas dos seus chamados.

As conexões são **simuladas**: ordenar as regiões alfabeticamente e ligar cada índice `i` a `(i+1) % V` e `(i+2) % V`, sem laços nem duplicatas. O anel garante conectividade e as demais arestas oferecem alternativas. Os pesos reaproveitam `111*|latA-latB| + 85*|lngA-lngB|`, aproximação de distância do T1. São armazenados em centésimos de km e exibidos em km. Essa topologia não representa vias reais.

**Dijkstra** usa custos não negativos e predecessores para encontrar e reconstruir o menor caminho. **Prim** escolhe as conexões de menor custo para ligar todos os vértices sem ciclos; informa quando a rede é desconexa. A mesma matriz simplifica as duas implementações. Na Logística, as opções 1/2 listam regiões/conexões, 3 consulta caminho e 4 consulta árvore mínima.

## Módulo 9 — Resumo regional

Mostra como os recursos selecionados foram distribuídos: busca na Hash os IDs escolhidos pela mochila e agrupa em vetor a quantidade, os minutos e o benefício por região. Reutiliza exatamente a seleção do módulo 7, sem otimizar novamente. Exibe todos os empates entre as regiões com mais atendimentos e trata seleção vazia. O relatório aparece automaticamente após otimizar.

## Exemplos executados

| Entrada | Resultado |
|---|---|
| Principal `6`, submenu `6` | Ordem válida das 30 atividades; adicionar `30 -> 1` pela opção 4 e verificar novamente detecta ciclo |
| `7 -> 1`, `NORRISTOWN`, `60` minutos | IDs 4 e 19 selecionados, 60 minutos e 8 pontos; 12 candidatas excluídas; resumo regional com os mesmos totais |
| `8 -> 3`, origem `1`, destino `22` | Caminho ABINGTON a NORRISTOWN, custo 191,68 km na rede simulada |
| `8 -> 4` | Árvore mínima com 40 arestas e soma de 719,23 km |

## Complexidade e implementação

| Operação | Tempo | Espaço auxiliar |
|---|---|---|
| Hash | Médio `O(1)`, pior `O(n)` | `O(1)` por operação, além dos nós |
| B+: faixa/prefixo | `O(h + F)` | `O(1)`, além da saída |
| Guloso | `O(n²)` com consultas médias `O(1)` na Hash | `O(n)` |
| Kahn, Dijkstra e Prim com matriz | `O(V²)` | `O(V)` |
| Mochila 0/1 | `O(n*C)` | `O(n*C)` |
| Resumo regional | `O(n + s*r + r log r)` com consultas médias `O(1)` na Hash | `O(s)` |

`n`: registros/candidatas; `V`: vértices; `C`: capacidade; `h`: altura da B+; `F`: folhas percorridas; `s`: selecionadas; `r`: regiões da seleção. As matrizes ocupam `O(V²)`. A mochila é pseudopolinomial em C. A B+ do T1 mantém remoção simplificada, sem fusão ou redistribuição de nós.

Os algoritmos estão separados em `kahn`, `mochila`, `grafo`, `dijkstra` e `prim`, com `.c` em `src/` e `.h` em `include/`. Usa-se apenas a biblioteca padrão de C para entrada/saída, memória, strings, limites, validação numérica e ordenação de nomes/IDs. Os algoritmos centrais são implementados no projeto; `qsort` é apenas apoio.

## Testes e limites

**Quatro suítes C e 23 testes de terminal passaram** no Windows, inclusive numa cópia limpa: ciclos; mochila versus força bruta; Dijkstra versus Floyd-Warshall; Prim versus Kruskal; totais do resumo; cadastro/alteração/remoção e índices B+. Os algoritmos de referência ficam somente nos testes. Os comandos completos estão em [APRESENTACAO.md](APRESENTACAO.md#como-repetir-os-testes).

Alterações ficam em memória. A rede é fixa na sessão. A carga do CSV e várias listagens do T1 limitam-se a 1000 registros; os grafos suportam 100 vértices. O CSV atende ao formato simples da amostra e a data é validada pelo formato.

[Estudo e demonstração](APRESENTACAO.md) · [Planejamento e checks](PLANEJAMENTO.md)
