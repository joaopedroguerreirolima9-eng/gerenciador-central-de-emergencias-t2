# Central de Emergências — Operação Resgate

Trabalho 2 de Algoritmos e Estruturas de Dados II (UFPel).

**Integrantes:** João Pedro Guerreiro Lima e Mariana Ferreira Tica.

O sistema em C amplia o T1 com dependências entre atividades, seleção de ocorrências sob uma capacidade de atendimento, análise da rede logística e resumo regional da seleção. Os módulos 1 a 5 foram preservados; os módulos 6 a 9 estão integrados aos menus.

## Compilação e execução

É necessário ter GCC no PATH. Execute os comandos na raiz do projeto, onde estão `src/`, `include/` e `data/`.

**Windows (PowerShell):**

```powershell
.\build.bat
```

O script compila e executa. Para compilar com verificação de avisos:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Iinclude src/main.c -o central.exe
.\central.exe
```

**Linux/macOS:**

```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c -o central
./central
```

A validação foi realizada no Windows com GCC 15.2.0. O programa inclui as implementações `.c` em `main.c`, como no T1; compile apenas essa unidade, evitando definições duplicadas. Não há API, banco externo ou dependência de rede na execução.

## Dados e estruturas do T1

O arquivo `data/ocorrencias_911.csv` contém **150 registros**, com **41 regiões**, da amostra usada no T1. A fonte é o [911 Calls — Emergency Dataset](https://www.kaggle.com/datasets/mchirico/montcoalert), com chamados da região de Montgomery County, Pensilvânia. Os campos usados são tipo, descrição, região, endereço, CEP, latitude, longitude e data/hora. O CSV não possui ID; o carregamento gera identificadores sequenciais.

A Tabela Hash com encadeamento guarda as ocorrências completas e suas assinaturas de integridade. São 223 posições, com fator de carga inicial `150/223 ≈ 0,67`. Três árvores B+ guardam os índices de tipo, região e data, com pares `(chave, ID)`. Cada nó comporta até quatro chaves, conforme a convenção de ordem adotada no T1. O ID desempata valores iguais.

| Módulo do T1 | Operações preservadas |
|---|---|
| 1 — Central de Ocorrências | Cadastrar, consultar, alterar, remover e listar; alterações atualizam os índices e a assinatura |
| 2 — Consulta Rápida | ID, palavra da descrição, tipo, região, prefixo, intervalo de datas e prioridade simulada |
| 3 — Organização | Listar por região/categoria, em ordem cronológica ou por região |
| 4 — Integridade | Assinaturas, registros inconsistentes, duplicatas, conflitos, comparação com o CSV e simulação de alteração sem registro |
| 5 — Operação Resgate | Guloso por proximidade ou por categoria e proximidade, com limite de deslocamento |

A Hash permite recuperar registros por ID; as folhas ligadas da B+ atendem consultas e listagens ordenadas. A busca por palavra no meio da descrição permanece sequencial. O guloso do T1 continua disponível e fornece a classificação e a função de distância reutilizadas no T2.

## Menus e modos de interação

| Opção principal | Acesso |
|---|---|
| 1 | Central de Ocorrências |
| 2 | Consulta Rápida |
| 3 | Organização das Ocorrências |
| 4 | Investigação: submenu 1 para integridade; submenu 2 para dependências e ciclos |
| 5 | Operação Resgate do T1 |
| 6 | Acesso direto ao Planejamento Operacional, com as mesmas dependências da opção 4 |
| 7 | Planejamento Estratégico: otimização, resumo regional e consulta dos dados simulados |
| 8 | Logística: regiões, conexões, Dijkstra e Prim |
| 0 | Sair |

Investigação reúne verificações dos módulos 4 e 6. Planejamento Estratégico reúne os módulos 7 e 9. Logística apresenta as consultas do módulo 8. Consulta Rápida reutiliza a Hash e os índices do T1 e permite consultar os parâmetros usados na otimização. `0` retorna ao menu anterior.

## Módulo 6 — Dependências de atendimento

Cada vértice representa uma atividade operacional; `A -> B` significa que A deve terminar antes de B. O grafo é direcionado, representado por matriz de adjacência, com capacidade para 100 atividades. O cenário inicial simulado tem **30 atividades e 54 dependências**. É possível cadastrar atividades, incluir/remover dependências, listar o cenário e restaurá-lo.

A ordenação topológica usa **Kahn**: calcula os graus de entrada, coloca na fila as atividades de grau zero e libera as seguintes conforme suas dependências são concluídas. Se processar menos atividades que o total, existe ciclo e não há ordem válida para todas. A lista de bloqueadas pode conter tanto atividades do ciclo quanto atividades que dependem dele.

A matriz facilita cadastrar e consultar arestas nessa escala. Kahn foi escolhido porque produz uma sequência válida e detecta a impossibilidade de ordenar o grafo.

**Exemplo:** menu `6`, opção `6`: ordena as 30 atividades. Pela opção `4`, adicionar `30 -> 1`; verificar novamente detecta ciclo. Pela opção `5`, remover essa dependência e verificar restaura a ordem válida. A opção `7` restaura o cenário inicial.

## Módulo 7 — Otimização de recursos

O modelo é a **Mochila 0/1**: cada ocorrência pode ser escolhida uma única vez. A capacidade principal é o tempo disponível, entre **0 e 1440 minutos**, e o objetivo é maximizar a soma dos benefícios. O usuário pode filtrar por região ou considerar todas.

Os custos e benefícios não existem no CSV; são parâmetros didáticos definidos pela classificação de categoria do T1:

| Categoria | Custo em minutos | Benefício em pontos |
|---|---:|---:|
| EMS | 30 | 4 |
| Fire | 45 | 3 |
| Traffic | 20 | 2 |
| Outras | 15 | 1 |

O estado `dp[i][c]` guarda o melhor benefício usando os primeiros `i` itens com capacidade `c`. A decisão é deixar o item de fora ou incluí-lo, quando couber:

```text
dp[i][c] = dp[i-1][c]                         se custo[i] > c
dp[i][c] = max(dp[i-1][c], beneficio[i] + dp[i-1][c-custo[i]])  caso contrário
```

Os casos-base valem zero. Depois, a reconstrução percorre a tabela de trás para frente para identificar os IDs selecionados. Os candidatos são ordenados por ID; em empate de benefício, não se inclui o item da linha atual. A saída lista selecionadas, não selecionadas, tempo utilizado, saldo e benefício. A programação dinâmica encontra uma solução ótima para esse modelo discreto.

**Exemplo executado:** `7 -> 1`, região `NORRISTOWN`, capacidade `60`:

```text
Selecionadas: IDs 4 e 19
Selecionadas: 2 de 14 | Tempo usado: 60/60 min | Restante: 0 min | Beneficio maximo: 8
```

As outras 12 candidatas são listadas como não selecionadas. A simulação não substitui protocolos oficiais de priorização ou atendimento de emergências.

## Módulo 8 — Rede de emergências

Foram escolhidas as duas técnicas **caminhos mínimos (Dijkstra)** e **árvore geradora mínima (Prim)**. Elas compartilham um grafo não direcionado e ponderado, com custos não negativos e matriz de adjacência. O enunciado pede duas entre três técnicas; fluxo máximo não faz parte desta escolha.

Cada uma das **41 regiões** da Hash vira um vértice. Sua posição é a média das coordenadas dos chamados daquela região, e não a localização de uma base ou hospital. Os nomes são ordenados alfabeticamente. Cada índice `i` é ligado a `(i + 1) % V` e `(i + 2) % V`, sem laços nem duplicatas. A regra gera **82 arestas**: o anel garante conectividade e as demais conexões fornecem alternativas. A amostra supera a recomendação de 30 vértices e 50 arestas.

A topologia é um cenário simulado para exercitar os algoritmos; a ordem alfabética não indica proximidade ou existência de vias reais. Os pesos reaproveitam a aproximação do T1 entre as coordenadas médias:

```text
distancia_km = 111 * |latA - latB| + 85 * |lngA - lngB|
custo = (int)(distancia_km * 100 + 0.5)
```

O custo inteiro representa centésimos de quilômetro; `1334` é exibido como `13.34 km`. `-1` representa ausência de aresta, e zero é um peso válido. Os IDs exibidos começam em 1; os índices internos começam em 0. A rede é construída ao iniciar a sessão e não é reconstruída após cadastrar, alterar ou remover ocorrências.

**Dijkstra** seleciona o vértice não visitado com menor distância acumulada e relaxa suas conexões. O vetor de predecessores reconstrói o percurso. Custos não negativos permitem confirmar a menor distância a cada passo. O menu informa destino inalcançável; origem igual ao destino custa zero.

**Prim** escolhe a conexão mais barata entre a árvore em construção e um vértice ainda fora dela. Começa no índice 0. A solução conecta todos os vértices com `V - 1` arestas, sem ciclos e com menor custo total. Rede desconexa não gera uma solução completa; uma árvore parcial é descartada. Um vértice isolado, sozinho, tem árvore válida de custo zero.

A matriz mantém as duas implementações simples para a escala adotada. Em empate, seleciona-se o menor índice e o predecessor só é atualizado quando o custo melhora.

| Opção dentro da Logística (`8`) | Resultado |
|---|---|
| 1 | Lista regiões e IDs |
| 2 | Lista cada conexão uma vez, com custo em km |
| 3 | Solicita origem e destino e exibe caminho mínimo e custo; `0` cancela |
| 4 | Exibe as conexões da árvore geradora mínima e seu custo total |

**Exemplos executados:** `8 -> 3 -> 1 -> 22` encontra o caminho de ABINGTON a NORRISTOWN com **191,68 km** na rede simulada. A opção `4` seleciona **40 arestas**, com soma de **719,23 km**. Esses valores descrevem a topologia didática, não rotas rodoviárias reais.

## Módulo 9 — Resumo regional

A necessidade adicional é mostrar como a seleção da mochila distribui os atendimentos entre as regiões. O resumo recupera cada ocorrência selecionada pelo ID na Hash e acumula quantidade, minutos e benefício num vetor de regiões. Usa os mesmos itens e a mesma seleção do módulo 7, sem executar outra otimização.

O relatório aparece logo após otimizar, antes de liberar os vetores. As regiões são ordenadas pelo nome; todas as empatadas com mais atendimentos são destacadas. Região vazia aparece como `(sem regiao)`. Seleção vazia tem totais zero, sem região líder. Os totais regionais devem fechar com os totais da mochila.

**Saída executada após NORRISTOWN/60:**

```text
Regiao: NORRISTOWN | Selecionadas: 2 | Tempo: 60 min | Beneficio: 8 pontos
Total do resumo: 2 ocorrencia(s) | 60 min | 8 pontos
Regiao(oes) com mais atendimentos (2):
- NORRISTOWN
```

Com região em branco e capacidade 60, HATFIELD TOWNSHIP e NEW HANOVER têm uma selecionada cada e aparecem empatadas. O total também é 2 ocorrências, 60 minutos e 8 pontos.

## Organização dos arquivos

Cada técnica tem seu `.c` em `src/` e seu `.h` em `include/`.

| Arquivo | Responsabilidade |
|---|---|
| `hash`, `btree`, `indices` | Armazenamento e índices do T1 |
| `csv`, `entrada`, `ocorrencia.h` | Leitura, interação e registro |
| `central`, `consultas`, `integridade`, `greedy` | Funcionalidades dos módulos 1 a 5 |
| `dependencias`, `kahn` | Cadastro/menu do grafo direcionado e ordenação topológica |
| `mochila`, `otimizacao` | Programação dinâmica e interação do planejamento |
| `grafo`, `dijkstra`, `prim`, `rede` | Construção da matriz, algoritmos e menu de Logística |
| `resumo` | Agrupamento regional da seleção |
| `main.c` | Carregamento e integração dos modos |

## Complexidade e bibliotecas

| Operação | Tempo | Espaço auxiliar |
|---|---|---|
| Hash: busca/inserção/remoção | Médio `O(1)`; pior caso `O(n)` | `O(1)` por operação, além dos nós armazenados |
| B+: busca por faixa/prefixo | `O(h + F)` | `O(1)`, além da saída |
| Guloso: escolha repetida entre candidatas | `O(n²)` com buscas médias `O(1)` na Hash | `O(n)` |
| Kahn, com matriz | `O(V²)` | `O(V)` |
| Mochila 0/1 | `O(n * C)` | `O(n * C)` |
| Dijkstra, com matriz | `O(V²)` | `O(V)` |
| Prim, com matriz | `O(V²)` | `O(V)` |
| Resumo regional | `O(n + s * r + r log r)` com buscas médias `O(1)` na Hash | `O(s)` |

`n`: candidatas/registros; `V`: vértices; `C`: capacidade; `h`: altura da B+; `F`: folhas percorridas; `s`: selecionadas; `r`: regiões da seleção. As matrizes dos grafos ocupam `O(V²)` além dos vetores auxiliares. A mochila é pseudopolinomial em relação à capacidade. Colisões da Hash podem aumentar o custo das operações que a consultam.

Usa-se apenas a biblioteca padrão de C: entrada/saída, memória, strings, limites numéricos, `strtol`/`errno`, `isfinite` e `qsort`. A ordenação de nomes e IDs é apoio à preparação/apresentação; Kahn, mochila, Dijkstra e Prim são implementados diretamente no projeto. Floyd-Warshall, Kruskal e força bruta aparecem somente nos testes, como referências independentes.

## Testes

Validados no Windows com GCC 15.2.0, sem avisos em `-std=c11 -Wall -Wextra -Werror`.

| Suíte | Conferências principais |
|---|---|
| `teste_algoritmos.c` | Kahn, ciclos, mochila, reconstrução, 200 comparações com força bruta e NORRISTOWN/60 |
| `teste_rede.c` | Construção, dados inválidos, Dijkstra versus Floyd-Warshall em 20 grafos e 1.681 pares do CSV; Prim versus Kruskal, conectividade, ciclos e rede desconexa |
| `teste_resumo.c` | Totais, empates, seleção vazia, região sem nome e 21 capacidades sobre a base |
| `teste_regressao.c` | 160 registros colidindo na Hash, índices B+ após alterações/remoções/reinserções, integridade e reutilização do T1 |
| `teste_menus.py` | 23 testes de terminal com cadastro, consultas, alterações, remoção, integridade, guloso e integração dos módulos novos |

Para executar tudo no PowerShell, a partir da raiz:

```powershell
gcc -std=c11 -Wall -Wextra -Werror -Iinclude src/main.c -o central.exe
foreach ($nome in 'teste_algoritmos', 'teste_rede', 'teste_resumo', 'teste_regressao') {
    gcc -std=c11 -Wall -Wextra -Werror -Iinclude "tests/$nome.c" -o "$nome.exe"
    if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao de $nome" }
    & ".\$nome.exe"
    if ($LASTEXITCODE -ne 0) { throw "Falha em $nome" }
}
python tests/teste_menus.py
```

Python 3 é necessário apenas para automatizar os testes de terminal, usando sua biblioteca padrão. Os testes C usam `assert`: compile sem `-DNDEBUG`. CSVs alternativos são criados em diretórios temporários, preservando a amostra versionada.

## Limites da versão

- Alterações ficam em memória e são perdidas ao sair. O CSV inicial não é reescrito.
- A carga do CSV e várias listagens do T1 têm limite de 1000 registros; a amostra entregue tem 150.
- Os grafos suportam 100 vértices/atividades. A rede logística fica fixa durante a sessão.
- O leitor CSV atende ao formato simples da amostra; não implementa todos os recursos de CSV com campos entre aspas.
- A validação de data confere o formato, não o calendário completo.
- A remoção da B+ do T1 não redistribui nem junta nós. Pode deixar folhas pouco ocupadas; por isso a análise de busca usa altura e folhas percorridas, sem prometer ocupação mínima após remoções.
- A assinatura de integridade é uma verificação didática de conteúdo, sem garantia criptográfica.

## Conferência com o enunciado

A revisão considerou as oito páginas do PDF fornecido.

| Exigência | Implementação/evidência |
|---|---|
| Expandir o T1 | Hash, B+, CSV e módulos 1 a 5 reutilizados e validados |
| Módulo 6: dependências e ordenação topológica | Matriz direcionada, cadastro, Kahn e teste com ciclo |
| Módulo 7: programação dinâmica, capacidade, selecionadas e excluídas | Mochila 0/1, tempo/benefício definidos, reconstrução e listagem |
| Módulo 8: duas técnicas de rede | Dijkstra e Prim, percursos/conexões e custos exibidos |
| Módulo 9: consulta complementar integrada | Resumo regional, necessidade e exemplos acima |
| Quatro modos de interação | Investigação, Planejamento, Logística e Consulta Rápida |
| Dimensões e testes | 30/54 nas dependências, 41/82 na rede e suítes reproduzíveis |
| Código, relatório, dados e demonstração | `src/`, `include/`, este README, CSV e roteiro em `APRESENTACAO.md` |

O planejamento e seus checks estão em `PLANEJAMENTO.md`. O roteiro de demonstração e estudo está em `APRESENTACAO.md`; o domínio individual do código precisa ser praticado pelos dois integrantes.

Repositórios: [T2](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2) e [T1](https://github.com/marianatica/gerenciador-central-de-emergencias).
