"""Testes pelo terminal. Compilar central.exe (ou central) antes de executar."""

import csv
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


RAIZ = Path(__file__).resolve().parents[1]
EXECUTAVEL = RAIZ / ("central.exe" if (RAIZ / "central.exe").exists() else "central")


class TestesMenus(unittest.TestCase):
    def executar(self, entrada, pasta=RAIZ):
        processo = subprocess.run(
            [str(EXECUTAVEL)], input=entrada, text=True, capture_output=True,
            cwd=pasta, timeout=10, encoding="utf-8",
        )
        self.assertEqual(processo.returncode, 0, processo.stderr)
        self.assertIn("Ate logo!", processo.stdout)
        return processo.stdout

    def test_abrir_e_retornar_dos_oito_acessos(self):
        titulos = {1: "MODULO 1", 2: "MODULO 2", 3: "MODULO 3",
                   4: "MODO INVESTIGACAO", 5: "MODULO 5", 6: "MODULO 6",
                   7: "MODO PLANEJAMENTO ESTRATEGICO", 8: "MODO LOGISTICA"}
        for opcao, titulo in titulos.items():
            with self.subTest(opcao=opcao):
                self.assertIn(titulo, self.executar(f"{opcao}\n0\n0\n"))

    def test_investigacao_reune_integridade_e_dependencias(self):
        texto = self.executar("4\n1\n0\n2\n6\n0\n0\n0\n")
        self.assertIn("MODULO 4: INVESTIGACAO", texto)
        self.assertIn("MODULO 6: DEPENDENCIAS", texto)
        self.assertIn("Sem ciclos. Ordem valida de execucao:", texto)

    def test_dependencias_compartilhadas_com_acesso_direto(self):
        texto = self.executar("4\n2\n4\n30\n1\n0\n0\n6\n6\n5\n30\n1\n6\n0\n0\n")
        self.assertIn("Ciclo detectado!", texto)
        self.assertIn("Dependencia removida.", texto)
        self.assertIn("Sem ciclos. Ordem valida de execucao:", texto)

    def test_listar_regioes_e_conexoes(self):
        texto = self.executar("8\n1\n2\n0\n0\n")
        self.assertIn("Rede simulada: 41 regioes, 82 conexoes.", texto)
        self.assertIn("1 - ABINGTON", texto)
        self.assertIn("41 - WHITPAIN", texto)
        arestas = re.findall(r"\[\d+\] .+? -- \[\d+\] .+? \| \d+\.\d{2} km", texto)
        self.assertEqual(len(arestas), 82)
        self.assertIn("82 conexoes nao direcionadas.", texto)

    def test_dijkstra_e_prim_pelo_menu(self):
        texto = self.executar("8\n3\n1\n22\n4\n0\n0\n")
        self.assertIn("Caminho minimo (Dijkstra):", texto)
        self.assertIn("[1] ABINGTON ->", texto)
        self.assertIn("[22] NORRISTOWN", texto)
        self.assertIn("Custo total do caminho: 191.68 km", texto)
        self.assertIn("Arvore geradora minima (Prim):", texto)
        self.assertIn("Arestas selecionadas: 40 | Custo total da arvore: 719.23 km", texto)
        arestas = re.findall(r"\[\d+\] .+? -- \[\d+\] .+? \| \d+\.\d{2} km", texto)
        self.assertEqual(len(arestas), 40)

    def test_origem_igual_destino_e_entradas_invalidas(self):
        texto = self.executar("8\n3\n1\n1\n3\n42\n1\n3\n-2147483648\n1\n3\n0\n99\n0\n0\n")
        self.assertIn("Custo total do caminho: 0.00 km", texto)
        self.assertEqual(texto.count("IDs invalidos."), 2)
        self.assertIn("Consulta cancelada.", texto)
        self.assertIn("Opcao invalida.", texto)

    def test_planejamento_com_resumo_e_consulta_rapida(self):
        texto = self.executar("7\n1\nNORRISTOWN\n60\n0\n2\n1\n4\n0\n0\n")
        self.assertIn("Total do resumo: 2 ocorrencia(s) | 60 min | 8 pontos", texto)
        self.assertIn("MODULO 2: CONSULTA RAPIDA", texto)

    def test_fim_da_entrada_retorna_e_sai(self):
        texto = self.executar("8\n3\n")
        self.assertIn("Consulta cancelada.", texto)

    def test_cadastro_nao_reconstroi_cenario_logistico(self):
        entrada = ("1\n1\nEMS: TESTE\nCenario T06\nREGIAO_NOVA\nRua Teste\n00000\n"
                   "2026-11-27 10:00:00\n40\n-75\n0\n8\n1\n0\n0\n")
        texto = self.executar(entrada)
        self.assertIn("Ocorrencia cadastrada com o id 151.", texto)
        self.assertIn("Rede simulada: 41 regioes, 82 conexoes.", texto)
        self.assertNotIn("REGIAO_NOVA", texto)

    def test_csv_vazio_e_rede_invalida_preservam_sistema(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            leitor = csv.DictReader(arquivo)
            campos = leitor.fieldnames
            linha = next(leitor)
        for invalida in (False, True):
            with self.subTest(rede_invalida=invalida), tempfile.TemporaryDirectory() as pasta:
                dados = Path(pasta) / "data"
                dados.mkdir()
                with (dados / "ocorrencias_911.csv").open("w", encoding="utf-8", newline="") as arquivo:
                    escritor = csv.DictWriter(arquivo, fieldnames=campos)
                    escritor.writeheader()
                    if invalida:
                        linha["twp"] = ""
                        escritor.writerow(linha)
                if invalida:
                    texto = self.executar("1\n5\n0\n8\n0\n", pasta)
                    self.assertIn("1 ocorrencias carregadas", texto)
                    self.assertIn("MODULO 1: CENTRAL", texto)
                    self.assertIn("Logistica indisponivel:", texto)
                else:
                    texto = self.executar("8\n1\n3\n4\n0\n0\n", pasta)
                    self.assertIn("Nenhuma regiao na rede.", texto)
                    self.assertIn("Rede vazia; nao ha regioes para consultar.", texto)
                    self.assertIn("Rede vazia; nao ha arvore geradora.", texto)


    def test_cadastro_alteracao_remocao_atualizam_todos_os_indices(self):
        data = "2026-11-27 10:00:00"
        nova_data = "2026-11-28 11:00:00"
        cadastro = ("1\n1\nEMS: TESTE_REGRESSAO\nDESCRICAO_UNICA_T07\nREGIAO_TESTE_T07\n"
                    f"RUA_T07\n00000\n{data}\n40\n-75\n0\n")
        consultas = ("2\n3\nEMS: TESTE_REGRESSAO\n4\nREGIAO_TESTE_T07\n"
                     f"5\nEMS: TESTE_REGRESSAO\n6\n{data}\n{data}\n2\nDESCRICAO_UNICA_T07\n0\n")
        alteracoes = ("1\n3\n151\n3\nREGIAO_ATUALIZADA_T07\n3\n151\n1\nFire: T07\n"
                      f"3\n151\n6\n{nova_data}\n0\n")
        consultas_novas = ("2\n4\nREGIAO_TESTE_T07\n4\nREGIAO_ATUALIZADA_T07\n"
                           f"3\nEMS: TESTE_REGRESSAO\n3\nFire: T07\n6\n{data}\n{data}\n"
                           f"6\n{nova_data}\n{nova_data}\n1\n151\n0\n"
                           "7\n1\nREGIAO_ATUALIZADA_T07\n45\n0\n")
        remocao = ("1\n4\n151\n1\n0\n2\n1\n151\n4\nREGIAO_ATUALIZADA_T07\n"
                   f"3\nFire: T07\n6\n{nova_data}\n{nova_data}\n0\n"
                   "7\n1\nREGIAO_ATUALIZADA_T07\n45\n0\n1\n5\n0\n0\n")
        texto = self.executar(cadastro + consultas + alteracoes + consultas_novas + remocao)
        self.assertIn("Ocorrencia cadastrada com o id 151.", texto)
        self.assertEqual(texto.count("Ocorrencia 151 alterada."), 3)
        antes = texto.split("--- Alterar ocorrencia ---", 1)[0]
        self.assertEqual(antes.count("1 ocorrencia(s) encontrada(s)."), 5)
        depois = texto.rsplit("Ocorrencia 151 alterada.", 1)[1].split("--- Remover ocorrencia ---")[0]
        self.assertEqual(depois.count("Nenhuma ocorrencia encontrada."), 3)
        self.assertEqual(depois.count("1 ocorrencia(s) encontrada(s)."), 3)
        self.assertIn("Total do resumo: 1 ocorrencia(s) | 45 min | 3 pontos", depois)
        removida = texto.split("Ocorrencia 151 removida.", 1)[1]
        self.assertIn("Ocorrencia 151 nao encontrada.", removida)
        self.assertEqual(removida.count("Nenhuma ocorrencia encontrada."), 3)
        self.assertIn("Nenhuma ocorrencia na regiao informada.", removida)
        self.assertNotIn("=== MODULO 9:", removida)
        self.assertIn("Total: 150 ocorrencia(s)", removida)

    def test_cancelamentos_preservam_ocorrencia_e_assinatura(self):
        texto = self.executar("1\n3\n4\n0\n4\n4\n0\n2\n4\n0\n4\n1\n1\n4\n0\n0\n0\n")
        self.assertIn("Alteracao cancelada.", texto)
        self.assertIn("Remocao cancelada.", texto)
        self.assertIn("Ocorrencia 4 integra:", texto)
        self.assertNotIn("Ocorrencia 4 removida.", texto)

    def test_tipo_obrigatorio_tambem_na_alteracao(self):
        texto = self.executar("1\n3\n4\n1\n\n2\n4\n0\n0\n")
        self.assertIn("O tipo e obrigatorio. Alteracao cancelada.", texto)
        self.assertNotIn("Ocorrencia 4 alterada.", texto)
        self.assertEqual(texto.count("Tipo:      EMS: CARDIAC EMERGENCY"), 2)

    def test_inteiros_fora_da_faixa_nao_viram_opcoes_validas(self):
        for valor in ("4294967297", "2147483648", "-2147483649", "9" * 45, "0" * 49 + "1"):
            with self.subTest(valor=valor):
                texto = self.executar(valor + "\n0\n")
                self.assertIn("Digite um numero inteiro", texto)
                self.assertNotIn("=== MODULO 1:", texto)

    def test_cadastro_invalido_nao_consume_id(self):
        texto = self.executar("1\n1\n\n1\nEMS: TESTE\nDESC\nREGIAO\nRUA\n00000\n27/11/2026\n"
                              "1\nEMS: VALIDA\nDESC\nREGIAO\nRUA\n00000\n2026-11-27 10:00:00\n40\n-75\n5\n0\n0\n")
        self.assertIn("O tipo e obrigatorio. Cadastro cancelado.", texto)
        self.assertIn("Data invalida:", texto)
        self.assertIn("Total: 151 ocorrencia(s)", texto)
        self.assertEqual(texto.count("Ocorrencia cadastrada com o id 151."), 1)

    def test_consultas_e_ordenacoes_conferem_com_csv(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            linhas = list(csv.DictReader(arquivo))
        casos = [
            ("2\n4\nNORRISTOWN\n0\n0\n", [i + 1 for i, r in enumerate(linhas) if r["twp"] == "NORRISTOWN"]),
            ("2\n3\n" + linhas[0]["title"] + "\n0\n0\n", [i + 1 for i, r in enumerate(linhas) if r["title"] == linhas[0]["title"]]),
            ("2\n5\nEMS\n0\n0\n", [i + 1 for i, r in enumerate(linhas) if r["title"].startswith("EMS")]),
            ("2\n2\nStation 308A\n0\n0\n", [i + 1 for i, r in enumerate(linhas) if "Station 308A" in r["desc"]]),
            ("2\n6\n2015-12-10 17:00:00\n2015-12-10 18:00:00\n0\n0\n", [i + 1 for i, r in enumerate(linhas) if "2015-12-10 17:00:00" <= r["timeStamp"] <= "2015-12-10 18:00:00"]),
            ("3\n3\n0\n0\n", sorted(range(1, len(linhas) + 1), key=lambda i: (linhas[i - 1]["timeStamp"], i))),
            ("3\n4\n0\n0\n", sorted(range(1, len(linhas) + 1), key=lambda i: (linhas[i - 1]["twp"], i))),
        ]
        for entrada, esperados in casos:
            with self.subTest(entrada=entrada):
                texto = self.executar(entrada)
                ids = [int(i) for i in re.findall(r"(?<!\d)(\d+) \| \d{4}-\d{2}-\d{2}", texto)]
                if entrada.startswith("2\n5\n"):
                    esperados = sorted(esperados, key=lambda i: (linhas[i - 1]["title"], i))
                elif entrada.startswith("2\n6\n"):
                    esperados = sorted(esperados, key=lambda i: (linhas[i - 1]["timeStamp"], i))
                elif not entrada.startswith("3"):
                    esperados = sorted(esperados)
                self.assertEqual(ids, esperados)

    def test_integridade_detecta_simulacao_sem_registro(self):
        texto = self.executar("4\n1\n6\n4\nREGIAO_ADULTERADA\n1\n4\n2\n5\n0\n0\n0\n")
        self.assertIn("a ocorrencia 4 foi alterada sem passar pelo sistema", texto)
        self.assertIn("150 ocorrencia(s) verificada(s): 1 alterada(s) sem registro.", texto)
        self.assertIn("iguais: 149 | diferentes: 1 | removidas: 0", texto)

    def test_guloso_por_distancia_e_por_prioridade(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            linhas = list(csv.DictReader(arquivo))
        def distancia(lat, lng, r):
            return abs(lat - float(r["lat"])) * 111 + abs(lng - float(r["lng"])) * 85
        def prioridade(r):
            return next((i for i, p in enumerate(("EMS", "Fire", "Traffic"), 1) if r["title"].startswith(p)), 4)
        for criterio in (1, 2):
            for limite in (0, 1):
                with self.subTest(criterio=criterio, limite=limite):
                    pendentes = [(i + 1, r) for i, r in enumerate(linhas) if r["twp"] == "NORRISTOWN"]
                    lat, lng, percorrido = 40.1215, -75.3399, 0.0
                    esperados = []
                    while pendentes:
                        melhor = min(pendentes, key=lambda x: ((prioridade(x[1]) if criterio == 2 else 0), distancia(lat, lng, x[1]), x[0]))
                        trecho = distancia(lat, lng, melhor[1])
                        if limite > 0 and percorrido + trecho > limite:
                            break
                        percorrido += trecho
                        esperados.append(melhor[0])
                        lat, lng = float(melhor[1]["lat"]), float(melhor[1]["lng"])
                        pendentes.remove(melhor)
                    texto = self.executar(f"5\n{criterio}\nNORRISTOWN\n{limite}\n0\n0\n")
                    obtidos = [int(i) for i in re.findall(r"\d+\. id\s+(\d+) \|", texto)]
                    self.assertEqual(obtidos, esperados)
                    self.assertIn(f"{len(esperados)} de 14 ocorrencia(s) atendida(s)", texto)

    def test_prioridades_da_consulta_conferem_com_csv(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            linhas = list(csv.DictReader(arquivo))
        for beneficio in range(1, 5):
            esperados = []
            for i, linha in enumerate(linhas):
                prioridade = next((p for p, nome in enumerate(("EMS", "Fire", "Traffic"), 1) if linha["title"].startswith(nome)), 4)
                if 5 - prioridade == beneficio:
                    esperados.append(i + 1)
            texto = self.executar(f"2\n7\n{beneficio}\n0\n0\n")
            self.assertEqual([int(i) for i in re.findall(r"(?<!\d)(\d+) \| ", texto)], esperados)

    def test_integridade_duplicatas_e_conflitos(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            leitor = csv.DictReader(arquivo)
            campos, base = leitor.fieldnames, next(leitor)
        base.update(desc="IGUAL", addr="RUA_DUPLICADA", timeStamp="2026-11-27 10:00:00")
        with tempfile.TemporaryDirectory() as pasta:
            dados = Path(pasta) / "data"
            dados.mkdir()
            with (dados / "ocorrencias_911.csv").open("w", encoding="utf-8", newline="") as arquivo:
                escritor = csv.DictWriter(arquivo, fieldnames=campos)
                escritor.writeheader()
                escritor.writerow(base)
                escritor.writerow(base)
                escritor.writerow(dict(base, desc="DIFERENTE"))
            texto = self.executar("4\n1\n4\n5\n0\n0\n0\n", pasta)
            self.assertIn("1 duplicada(s) e 1 conflito(s) de versao.", texto)
            self.assertIn("iguais: 3 | diferentes: 0 | removidas: 0", texto)

    def test_csv_ignora_linhas_incompativeis_e_mantem_ids(self):
        with (RAIZ / "data/ocorrencias_911.csv").open(encoding="utf-8-sig", newline="") as arquivo:
            leitor = csv.DictReader(arquivo)
            campos, primeira, segunda = leitor.fieldnames, next(leitor), next(leitor)
        with tempfile.TemporaryDirectory() as pasta:
            dados = Path(pasta) / "data"
            dados.mkdir()
            with (dados / "ocorrencias_911.csv").open("w", encoding="utf-8", newline="") as arquivo:
                escritor = csv.DictWriter(arquivo, fieldnames=campos)
                escritor.writeheader()
                arquivo.write("linha,invalida\n")
                escritor.writerow(primeira)
                escritor.writerow(dict(primeira, timeStamp="errada"))
                escritor.writerow(segunda)
            texto = self.executar("1\n5\n0\n7\n1\n\n60\n0\n0\n", pasta)
            self.assertIn("2 linha(s) do CSV ignorada(s)", texto)
            self.assertIn("Total: 2 ocorrencia(s)", texto)
            self.assertIn("Total do resumo: 2 ocorrencia(s) | 60 min | 8 pontos", texto)

    def test_limite_negativo_no_guloso_e_rejeitado(self):
        texto = self.executar("5\n1\nNORRISTOWN\n-1\n0\n0\n")
        self.assertIn("Limite invalido:", texto)
        self.assertNotIn("Ordem de atendimento", texto)

    def test_arquivo_ausente_retorna_erro_claro(self):
        with tempfile.TemporaryDirectory() as pasta:
            p = subprocess.run([str(EXECUTAVEL)], input="0\n", text=True, capture_output=True, cwd=pasta, timeout=10)
            self.assertEqual(p.returncode, 1)
            self.assertIn("nao foi possivel abrir o arquivo", p.stdout)


if __name__ == "__main__":
    unittest.main(verbosity=2)
