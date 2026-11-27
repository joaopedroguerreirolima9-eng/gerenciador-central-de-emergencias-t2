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


if __name__ == "__main__":
    unittest.main(verbosity=2)
