# Planejamento do T2 da Central de Emergências

Mariana e João devem concluir os módulos 8 e 9, validar a integração com a base existente e preparar a demonstração até **27/11/2026**, prazo informado por Mariana. O horário de entrega ainda precisa ser conferido. Os módulos 6 e 7 já possuem implementação e devem ser preservados e validados.

## Base da análise

- Repositório: https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2
- Referência analisada: `main`, commit `d6062fdc4ec9a0592ef6b1e0a8ef32271db7cb05`, consultado em 26/11/2026.
- Enunciado: PDF fornecido por Mariana, 8 páginas, “T2 - Central de Emergências: Operação Resgate”.
- A análise inicial usou o enunciado, os arquivos, as branches e o histórico de commits. A execução local da T01, em 26/11/2026, está registrada abaixo; os testes das etapas futuras continuam pendentes.
- O PDF registra entrega em 25/11/2026 às 12h e apresentação em 25/11. Para este plano, vale o novo prazo de entrega informado por Mariana, 27/11. Confirmar separadamente a data da apresentação.

## Requisitos do trabalho

Os itens abaixo são exigências do enunciado. As escolhas de algoritmos, arquivos e divisão de tarefas aparecem nas seções seguintes.

| Área | O que precisa ser entregue |
|---|---|
| Base do T1 | Reutilizar e expandir o sistema existente, com suas funcionalidades e dados. |
| Módulo 6 | Grafo direcionado de atividades e dependências; cadastro; ordenação topológica; indicação de ciclos e de impossibilidade de ordenar todas as atividades. |
| Módulo 7 | Programação dinâmica com decisão de incluir ou não cada ocorrência; uma capacidade principal; custos e benefícios definidos; seleção ótima, itens selecionados e excluídos e justificativa. |
| Módulo 8 | Implementar duas de três técnicas: caminhos mínimos, árvore geradora mínima ou fluxo máximo. Exibir conexões/caminho e custos conforme a técnica, justificar a modelagem e testar. |
| Módulo 9 | Funcionalidade complementar integrada; necessidade, descrição, implementação e exemplo com resultados. Não exige novo algoritmo complexo. Vale 10% da nota de código. |
| Interação | Reunir funcionalidades em Investigação, Planejamento Estratégico, Logística e Consulta Rápida, reutilizando operações existentes. Os exemplos do PDF não obrigam a implementar todas as consultas sugeridas. |
| Estruturas centrais | Implementadas pela dupla. Bibliotecas auxiliares são permitidas, com justificativa, sem substituir os objetivos do trabalho. |
| Relatório | README com execução, funcionalidades, estruturas, algoritmos, justificativas, complexidade, integração T1/T2, fontes de dados e exemplos de entradas e saídas. |
| Dados | Entregar os arquivos utilizados pelo sistema, quando aplicável. Informar dimensões das estruturas e demonstrar situações diferentes nos testes. |
| Apresentação | Demonstrar funcionamento e decisões. A avaliação é individual e os dois precisam dominar todo o código. |

O PDF **recomenda**, quando compatível com a modelagem, pelo menos 30 vértices e 50 arestas para os problemas de redes. Não é uma obrigação universal para todo grafo. Para o módulo 8, adotar essa dimensão como meta prática de demonstração.

A avaliação combina código (80%) e apresentação (20%) por média harmônica ponderada. Reservar tempo para os dois explicarem os algoritmos é parte da entrega.

## Situação atual

| Parte | Evidência nos arquivos | Próxima ação |
|---|---|---|
| Módulos 1 a 5 | CSV, Hash, B+, cadastro, consultas, integridade e guloso presentes. | Testar regressão após integrar os novos módulos. |
| Módulo 6 | `dependencias.c/.h`: matriz direcionada, Kahn, cadastro/remoção de dependências, cenário de 30 atividades e 54 arestas. | Validar ordem e ciclos; alinhar documentação e acesso ao modo Investigação. |
| Módulo 7 | `otimizacao.c/.h`: mochila 0/1, reconstrução, filtro por região e parâmetros simulados. | Validar resultado e fornecer seleção ao módulo 9. |
| Módulo 8 | T02 definiu a rede de 41 regiões/82 arestas e criou localmente o contrato `include/rede.h`; algoritmos ainda não implementados. | Implementar construção da rede e Dijkstra na T03; Prim na T04. |
| Módulo 9 | Resumo regional proposto no README; implementação ausente. | Reutilizar o resultado da mochila e agrupar por região. |
| Testes | O arquivo citado não existia na versão remota analisada. Na T01 foi adicionado localmente `tests/teste_algoritmos.c`, executado com sucesso no Windows. | Expandir a validação para os módulos 8 e 9 e repetir a regressão na T07. |
| Documentação | O roteiro de menus foi corrigido no README local durante a T01. | Manter as opções 6 e 7 para os módulos novos; atualizar novamente ao integrar a Logística. |

Na versão remota analisada, o README afirmava integração do módulo 6 ao modo Investigação, embora a `main.c` chame `integridade_menu` e `dependencias_menu` separadamente. A T01 corrigiu a descrição no README local. A organização dos acessos será revisada na T06.

## Padrões observados no projeto

### Organização e compilação

- Linguagem C, interface de terminal, sem framework externo.
- `include/`: contratos públicos, tipos e constantes em `.h` com guardas `#ifndef`/`#define`/`#endif`.
- `src/`: implementações `.c` por responsabilidade; `data/`: dados de entrada.
- `main.c` inclui diretamente os demais arquivos `.c`. O build compila **somente `src/main.c`**. Compilar `src/*.c` junto com essa main duplicaria definições.
- Windows: `build.bat` executa `gcc -Iinclude src\main.c -o central.exe`, verifica falha e inicia o programa.
- Linux/macOS: `gcc -Iinclude src/main.c -o central` e `./central`.
- Executar pela raiz, pois o caminho de entrada é relativo: `data/ocorrencias_911.csv`.
- Para este prazo, manter o modelo de compilação. Uma migração para compilação separada pode ser tratada depois.

### Código e interação

- Funções e variáveis em português, normalmente `snake_case`; prefixos de módulo como `hash_`, `btree_`, `dependencias_` e `plano_`.
- Tipos como `Ocorrencia`, `TabelaHash`, `Indices`, `Dependencias` e `ItemPlano`; constantes em maiúsculas.
- Funções internas marcadas `static`; assinaturas públicas no cabeçalho.
- Indentação predominante de 4 espaços. A base do T1 usa chaves na mesma linha em muitos arquivos; os novos módulos usam chaves na linha seguinte. Não há um estilo único de chaves: seguir o arquivo alterado.
- Comentários explicativos em português e mensagens de terminal sem acentos, padrão também registrado no histórico de refatoração do T1. O README usa português com acentos.
- Menus numéricos, `0` para voltar/sair, leitura centralizada em `ler_texto`, `ler_inteiro` e `ler_decimal`.
- Uso de ponteiros para compartilhar estruturas; `const` em operações de leitura onde já adotado; verificações de alocação e liberação de memória.
- Como os `.c` são incluídos na mesma unidade de compilação, até funções `static` de módulos diferentes podem colidir por nome. Nos arquivos novos, usar prefixos próprios.

### Dados e algoritmos

- Hash com encadeamento é o armazenamento principal; B+ mantém índices de tipo, região e data, guardando chaves e IDs.
- `Ocorrencia` preserva os campos do dataset; IDs iniciais são gerados pela ordem de leitura. Custos/benefícios simulados ficam em `ItemPlano`, calculados por `plano_item`.
- Alterações autorizadas de ocorrências atualizam índices e assinatura de integridade. Não alterar a Hash diretamente em funcionalidades comuns.
- Capacidade da mochila: 0 a 1440 minutos. EMS: 30 min/4 pontos; Fire: 45/3; Traffic: 20/2; outras: 15/1.
- Candidatos da mochila são ordenados por ID; em empate de benefício, a reconstrução não inclui o item da linha atual.
- O cenário de dependências usa IDs exibidos a partir de 1 e posições internas a partir de 0.
- A remoção da B+ existente não faz rebalanceamento; não apresentar essa implementação como remoção completa com manutenção de ocupação mínima.
- `.gitignore` exclui objetos, executáveis, cobertura, dataset completo `data/911.csv` e exportação `data/base_exportada.csv`. A amostra é versionada.

### Commits e branches

O padrão predominante observado é `tipo: descricao`, com tipo em inglês e descrição curta em português. Há exceções antigas, como `first commit`, `Update README.md` e títulos sem prefixo; não são o padrão a repetir.

| Tipo observado | Uso | Exemplo real do histórico |
|---|---|---|
| `feat` | Funcionalidade | `feat: implementa ordenacao topologica e deteccao de ciclos` |
| `fix` | Correção | `fix: remove acesso duplicado ao modulo 5` |
| `refactor` | Reorganização interna | `refactor: disponibiliza funcoes do algoritmo guloso` |
| `docs` | Documentação | `docs: atualiza status do T2 e proposta do modulo 9` |
| `chore` | Suporte e dados | `chore: reduz amostra para 150 registros` |

As branches observadas incluem `feat/modulo-6`, `feat/modulo-7`, `feat/menu-principal` e `docs/status-t2`. O histórico mostra integração por pull requests com commits de merge. Isso é uma prática observada; não foi identificada uma norma formal de revisão. A `main` aparece sem proteção na consulta de branches.

Sugestão para a dupla: manter `feat/<assunto>`, `fix/<assunto>` e `docs/<assunto>`, com um PR por entrega coesa e revisão pelo colega. `test:` pode ser adotado para testes, mas não apareceu entre os tipos do histórico analisado. Por orientação de Mariana, as branches devem seguir os prefixos do projeto, como `feat/`, `fix/`, `docs/` e `chore/`, sem prefixo do agente.

## Decisões para concluir

### Módulo 8

**Modelagem fechada na T02:** Dijkstra + Prim sobre um grafo não direcionado e ponderado com matriz de adjacência. As duas técnicas compartilharão a representação e terão implementação `O(V²)` adequada à escala didática.

- Vértices: todas as **41 regiões distintas** das 150 ocorrências do CSV, agrupadas a partir da Hash. Usar a média das coordenadas dos chamados de cada região e ordenar os nomes por `strcmp`. Não acrescentar pontos fictícios.
- Arestas: inicializador determinístico liga cada índice `i` aos sucessores `(i + 1) % V` e `(i + 2) % V`, nos dois sentidos e sem laços/duplicatas. O anel garante conectividade e as conexões adicionais oferecem alternativas. Na amostra atual são **82 arestas**, contadas uma vez cada.
- Pesos: **centésimos de quilômetro**, usando `distancia_km` do T1 entre os pontos médios das regiões e arredondando `distancia * 100 + 0.5` para inteiro. Exibir em km dividindo por `100.0`. Coordenadas vêm do CSV; topologia e interpretação logística são simuladas, sem afirmar que as ligações são vias reais.
- Ausência de aresta e diagonal: `REDE_SEM_ARESTA = -1`. Peso zero é válido. Rejeitar pesos negativos, duplicatas, laços e valores acima de `(INT_MAX - 1) / MAX_PONTOS_REDE`. Capacidade: 100 vértices.
- A inicialização rejeitará região vazia, coordenadas inválidas e excesso de regiões; em erro, deixará a rede vazia. A rede será um cenário da sessão, construído após carregar o CSV, sem atualização automática após alterações nas ocorrências.
- Dijkstra exibe percurso e custo; origem igual ao destino produz um vértice e custo zero. Retornos previstos: sucesso, sem solução ou erro de argumentos. Falhas deixam o resultado vazio.
- Prim inicia no índice 0 e exibe arestas e custo; rede vazia/desconexa não tem solução completa. Um único vértice produz árvore válida com zero arestas. Em falha, não apresentar árvore parcial como solução completa.
- Empates: selecionar o menor índice e atualizar predecessor somente em melhoria estrita de custo.
- Contrato criado em `include/rede.h`: `Rede`, `PontoRede`, `CaminhoRede`, `ArestaRede`, `ArvoreRede`, funções de construção, algoritmos e menu. `src/rede.c` será implementado nas T03/T04 e incluído uma vez na `main.c` durante a integração.

### Módulo 9

Manter a proposta de **resumo por região das ocorrências selecionadas no módulo 7**. Mostrar quantidade selecionada, minutos, benefício e todas as regiões empatadas com maior quantidade.

O caminho mais curto é produzir o relatório imediatamente após uma otimização bem-sucedida, passando o mesmo vetor `itens`, o mesmo vetor `escolhido` e a quantidade de candidatos para uma função do módulo 9. Os vetores hoje são liberados ao final de `plano_executar`; o relatório deve consumi-los antes disso. Isso evita guardar ponteiros inválidos e evita recalcular uma seleção diferente.

Consultar cada ocorrência selecionada pelo ID na Hash para obter sua região. Tratar região vazia com um rótulo explícito. A soma de quantidades, minutos e benefícios regionais deve coincidir com os totais da mochila. Para um agrupamento simples por vetor com busca linear, documentar `O(s * r)` no agrupamento, além das consultas à Hash, onde `s` é o número de selecionadas e `r` o número de regiões agrupadas.

Arquivos sugeridos: `include/resumo.h` e `src/resumo.c`. O contrato pode reutilizar `ItemPlano` de `otimizacao.h`; manter o cabeçalho sem dependência circular. Não tornar a impressão do módulo 7 uma segunda implementação da mochila.

## Tarefas em ordem de execução

A distribuição abaixo é uma proposta operacional. Quem implementa apresenta o código ao colega, que revisa e executa os exemplos.

- [x] **T01 — Preparar e validar a base local.** Concluída em 26/11/2026.
- [x] **T02 — Fechar a modelagem e a interface da rede.** Concluída em 26/11/2026.
- [ ] **T03 — Implementar a rede e Dijkstra.** Próxima tarefa.
- [ ] **T04 — Implementar Prim.**
- [ ] **T05 — Implementar o resumo regional.**
- [ ] **T06 — Integrar modos e menus.**
- [ ] **T07 — Executar a validação final e a regressão.**
- [ ] **T08 — Finalizar documentação, entrega e ensaio.**

Marcar uma tarefa somente depois de verificar seu critério de conclusão. Registrar o que foi executado e as pendências no histórico abaixo.

| ID | Entrega e critério de conclusão | Responsável sugerido | Depende de |
|---|---|---|---|
| T01 | Preparar cópia local atualizada; compilar a base; localizar os testes citados ou adicionar testes reproduzíveis se ausentes; registrar resultado real e corrigir o roteiro de menus. | Dupla | Nada |
| T02 | Fechar modelagem da rede: vértices, arestas, unidade, origem dos pesos e ligação com dados do T1. Definir interface de `rede.h`. | Mariana, com revisão de João | T01 |
| T03 | Implementar inicialização/validação da rede e Dijkstra. Demonstrar caminho, custo, origem=destino e destino inalcançável. | Mariana | T02 |
| T04 | Implementar Prim na mesma rede. Conferir custo, ausência de ciclos e `V-1` arestas no caso conectado; informar desconexão. | Mariana | T02; integrar com T03 |
| T05 | Definir passagem do resultado da mochila e implementar resumo regional, com totais e empates corretos. Demonstrar seleção vazia e múltiplas regiões. | João, com revisão de Mariana | T01 |
| T06 | Integrar modo Logística, acesso ao resumo e organização dos modos. Preservar acessos dos módulos 1 a 5 e alinhar README. | João, com revisão de Mariana | T03, T04, T05 |
| T07 | Disponibilizar testes reproduzíveis dos algoritmos e rodar regressão dos módulos antigos. Corrigir falhas antes da entrega. | Dupla | T06 |
| T08 | Finalizar README, exemplos reais, dimensões e arquivos de dados. Repetir build a partir de cópia limpa e ensaiar explicação dos quatro módulos novos. | Dupla | T07 |

T03/T04 e T05 podem avançar em paralelo depois de T01/T02. Para evitar conflitos, Mariana concentra alterações em `rede.c/.h`; João em `resumo.c/.h` e `otimizacao.c/.h`. João concentra a integração em `main.c` após os contratos estarem definidos. Os dois revisam o README no fechamento.

## Sequência até a entrega

**26/11:** validar a base e fechar contratos; desenvolver módulo 8 e módulo 9 nas duas frentes; integrar assim que cada parte tiver um exemplo pequeno correto.

**27/11, antes do limite real de entrega:** concluir integração, rodar testes, finalizar README e dados, testar cópia limpa e ensaiar. Como o horário não foi informado, conferir o sistema de entrega antes de planejar o trabalho até o fim do dia.

Se o tempo apertar, manter foco nos requisitos: dois algoritmos do módulo 8 corretos, módulo 9 integrado, modos acessíveis, exemplos verificáveis e preservação do T1. Evitar adicionar fluxo máximo, interface gráfica, APIs ou reestruturação geral do build neste fechamento.

## Validação para cada entrega

| Parte | Casos essenciais | Como conferir |
|---|---|---|
| Kahn | Cenário inicial; ciclo; laço; vértice isolado; ID inválido; duplicata. | Em ordem válida, cada aresta A -> B deve ter A antes de B. Em ciclo, não afirmar ordem completa. Vértices bloqueados podem incluir dependentes do ciclo. |
| Mochila | Capacidade zero; insuficiente; limite; região vazia/inexistente; empate; conjunto pequeno. | Somar custo e benefício dos IDs reconstruídos; custo não excede capacidade; comparar casos pequenos com enumeração de subconjuntos. |
| Dijkstra | Triângulo; origem=destino; sem caminho; entrada inválida; peso negativo; peso zero se suportado. | A-B=4, B-C=3, A-C=10: caminho A-B-C custa 7. Conferir também um segundo grafo, sem depender só desse exemplo. |
| Prim | Triângulo; grafo maior; desconexão; empate de pesos; um vértice. | No triângulo, custo 7; na rede conectada, `V-1` arestas, todos os vértices alcançados e nenhum ciclo. Conferir custo esperado em grafo pequeno. |
| Resumo | Uma e várias regiões; nenhuma selecionada; empates; região sem nome. | Totais regionais iguais aos totais da seleção que gerou o relatório; listar todos os empates. |
| Integração T1/T2 | Carregar CSV; buscar por ID/região; cadastrar/alterar/remover; consultar índices; executar guloso; entrar e sair de cada modo. | Dados e índices coerentes, menus sem perda de acesso e retorno correto com `0`. |

O exemplo de `NORRISTOWN` com capacidade 60 foi reproduzido na T01: IDs 4 e 19, 60 minutos e 8 pontos. Repetir na versão final após as integrações.

Compilação de verificação sugerida, preservando o build atual:

```powershell
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c -o central.exe
.\central.exe
```

Acrescentar instrução de compilação dos testes ao README apenas depois de os respectivos arquivos estarem versionados e o comando ter sido executado. Não afirmar teste em Windows ou Linux sem execução nessa plataforma.

## Fluxo de trabalho proposto

1. Atualizar a `main` antes de iniciar cada tarefa e criar branch de escopo claro.
2. Combinar contrato dos cabeçalhos antes de alterar chamadas compartilhadas.
3. Fazer commits pequenos que deixem uma entrega compreensível e compilável.
4. Abrir PR com problema resolvido, comportamento, arquivos/dados necessários e validação executada.
5. O colega revisa e explica o algoritmo de volta; corrigir problemas e integrar pelo fluxo de PR já utilizado.
6. Atualizar a lista de tarefas e o README conforme o que realmente entrou na `main`.

Exemplos de commits novos compatíveis com o padrão observado:

```text
feat: implementa rede logistica e caminhos minimos
feat: implementa arvore geradora minima com Prim
feat: adiciona resumo regional das ocorrencias selecionadas
fix: alinha acessos dos modos ao menu principal
docs: registra testes e exemplos finais do T2
```

## Checklist de conclusão

- [ ] Prazo e horário final conferidos; data da apresentação confirmada.
- [ ] Módulos 1 a 7 preservados e validados na versão final.
- [ ] Dijkstra e Prim integrados com caminho/conexões e custos corretos.
- [ ] Rede e pesos documentados, incluindo dimensões e caráter simulado.
- [ ] Resumo regional usa exatamente a seleção da mochila e fecha os totais.
- [ ] Modos de interação acessíveis e numeração consistente com o README.
- [ ] Testes reproduzíveis presentes e resultados registrados.
- [ ] README cobre todos os entregáveis do enunciado.
- [ ] Dados necessários versionados; compilação e execução funcionam a partir de cópia limpa.
- [ ] Mariana e João conseguem explicar Kahn, mochila, Dijkstra, Prim e integração do resumo.

## Histórico das tarefas concluídas

### T01 concluída em 26 de novembro

- Cópia local: esta pasta do repositório, `gerenciador-central-de-emergencias-t2/`. Origem e commit conferidos com a versão analisada. Branch de trabalho: `chore/validacao-inicial`.
- Compilação no Windows com GCC 15.2.0: `gcc -std=c11 -Wall -Wextra -Iinclude src/main.c -o central.exe`, concluída sem avisos.
- O arquivo de testes citado não foi encontrado na `main`, nas branches dos módulos 6 e 7 nem no histórico disponível. Foi criado `tests/teste_algoritmos.c` com verificações de precedência de Kahn, ciclos, laços, IDs, vértice isolado, limites e desempate da mochila e 200 comparações determinísticas com enumeração de subconjuntos, conferindo também a reconstrução.
- O teste de integração carregou 150 ocorrências do CSV, consultou NORRISTOWN pela B+ e validou na Hash a seleção dos IDs 4 e 19: 60 minutos e 8 pontos. O executável de testes terminou com `Todos os testes passaram.`.
- Testes pelo terminal: inicialização/saída; abertura/retorno dos sete menus; Kahn antes/depois de inserir e remover 30 -> 1; mochila NORRISTOWN/60. Todos passaram.
- README local corrigido: numeração dos menus, acessos separados à investigação e às dependências, comandos de testes e resultados reais no Windows.
- Nenhum algoritmo de produção foi alterado. Não houve commit, push ou PR nesta etapa. A revisão por João e a regressão completa de cadastro/alteração/remoção continuam para as etapas de integração e fechamento.
- Próxima ação: T02, definir a rede logística e o contrato de `rede.h` antes de implementar os algoritmos do módulo 8.

### T02 concluída em 26 de novembro

- Definidos vértices, coordenadas regionais, topologia simulada, unidade dos pesos, regras de validação, integração com a Hash e comportamento dos algoritmos em casos especiais. O README local contém a modelagem completa.
- Criado `include/rede.h`, com tipos e assinaturas para as implementações das T03/T04. Nenhum algoritmo ou menu de produção foi alterado nesta etapa.
- Conferência independente sobre o CSV: 150 registros, 41 regiões com nomes e coordenadas válidos, 82 arestas sem laços/duplicatas e conectividade de todos os vértices.
- Os pesos simulados calculados pela regra definida variam de 378 a 6490 centésimos de km (3,78 a 64,90 km). Exemplo: ABINGTON–AMBLER tem peso 1334 (13,34 km). Estes números validam a modelagem; a geração em C ainda será implementada.
- A regra de conexões também foi conferida para 0 a 5 regiões, incluindo rede vazia, vértice único e eliminação de duplicatas nas redes pequenas.
- Cabeçalho compilado isoladamente e com inclusão repetida usando GCC, `-std=c11 -Wall -Wextra -Werror -fsyntax-only`, sem erros ou avisos. Verificado também que o limite acumulado dos custos fica abaixo de `INT_MAX`.
- Próxima ação: T03, implementar `src/rede.c` com inicialização, validação e Dijkstra, e comparar resultados com casos pequenos conhecidos. Prim permanece na T04; integração dos menus na T06.

## Registro em Git

- T01: commit `7b0ab7b`, `chore: adiciona validacao reproduzivel dos modulos 6 e 7`, na branch `chore/validacao-inicial`.
- T02: branch `feat/modelagem-rede`, criada a partir da T01 para manter os testes disponíveis; commit com a mensagem `feat: define modelagem e contrato da rede logistica`.
- Este planejamento passa a ser versionado na raiz do repositório. Manter aqui os checks e o histórico das próximas tarefas.
- Os registros de execução acima descrevem o estado ao concluir cada etapa. Os commits foram preparados posteriormente, a pedido de Mariana. Nenhum push ou PR foi realizado neste fechamento.

## Referências verificáveis

- [Arquivos na versão analisada](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2/tree/d6062fdc4ec9a0592ef6b1e0a8ef32271db7cb05)
- [README na versão analisada](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2/blob/d6062fdc4ec9a0592ef6b1e0a8ef32271db7cb05/README.md)
- [Menu principal na versão analisada](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2/blob/d6062fdc4ec9a0592ef6b1e0a8ef32271db7cb05/src/main.c)
- [Histórico de commits até a versão analisada](https://github.com/joaopedroguerreirolima9-eng/gerenciador-central-de-emergencias-t2/commits/d6062fdc4ec9a0592ef6b1e0a8ef32271db7cb05)
- PDF local: `C:\Users\Mariana\Downloads\T2 -  Central de Emergências_ Operação Resgate (1).pdf`, especialmente páginas 2 a 6 para requisitos e 7 a 8 para avaliação e entrega.
