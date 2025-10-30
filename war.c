### ESTRUTURAS DE DADOS PARA REPRESENTAR O JOGO WAR ###

class Territorio:
    """
    Representa um único território no mapa (um nó do grafo).
    """
    def __init__(self, nome):
        # Propriedades do Território
        self.nome = nome
        self.proprietario = None  # Nome do jogador que o possui (ex: 'JogadorA')
        self.exercitos = 0        # Número de exércitos no território

    def __str__(self):
        """Representação em string para facilitar a visualização."""
        prop = self.proprietario if self.proprietario else "Ninguém"
        return f"{self.nome} (Exércitos: {self.exercitos}, Dono: {prop})"
    
    def adicionar_exercito(self, quantidade):
        """Adiciona exércitos ao território."""
        self.exercitos += quantidade

# -------------------------------------------------------------

class MapaWar:
    """
    Representa o mapa do jogo usando uma Lista de Adjacência (um Grafo).
    """
    def __init__(self):
        # Dicionário onde a chave é o nome do território e o valor é o objeto Territorio
        self.territorios = {}
        # Dicionário para Lista de Adjacência: Chave = Nome do Território, Valor = Set de vizinhos
        self.adjacencias = {}

    def adicionar_territorio(self, nome_territorio):
        """Adiciona um novo nó (território) ao grafo."""
        if nome_territorio not in self.territorios:
            novo_territorio = Territorio(nome_territorio)
            self.territorios[nome_territorio] = novo_territorio
            self.adjacencias[nome_territorio] = set()

    def adicionar_fronteira(self, t1_nome, t2_nome):
        """Cria uma aresta (fronteira) entre dois territórios.
        Como as fronteiras são bidirecionais, é um grafo não-direcionado.
        """
        if t1_nome in self.adjacencias and t2_nome in self.adjacencias:
            self.adjacencias[t1_nome].add(t2_nome)
            self.adjacencias[t2_nome].add(t1_nome) # Adiciona a conexão reversa
        else:
            print(f"Erro: Território '{t1_nome}' ou '{t2_nome}' não existe.")

    def obter_vizinhos(self, nome_territorio):
        """Retorna os nomes dos territórios vizinhos."""
        return self.adjacencias.get(nome_territorio, set())

    def imprimir_mapa(self):
        """Imprime o estado atual de cada território e suas conexões."""
        print("--- Estado Atual do Mapa ---")
        for nome, territorio in self.territorios.items():
            vizinhos = ", ".join(self.adjacencias[nome])
            print(f"🏙️ {territorio}")
            print(f"   Fronteiras: [{vizinhos}]")
        print("----------------------------")

# -------------------------------------------------------------

### EXEMPLO DE USO ###

# 1. Inicializa o Mapa
mapa = MapaWar()

# 2. Adiciona Territórios
mapa.adicionar_territorio("Brasil")
mapa.adicionar_territorio("Argentina")
mapa.adicionar_territorio("Peru")
mapa.adicionar_territorio("Venezuela")

# 3. Define as Fronteiras (Arestas do Grafo)
mapa.adicionar_fronteira("Brasil", "Argentina")
mapa.adicionar_fronteira("Brasil", "Peru")
mapa.adicionar_fronteira("Peru", "Venezuela")
mapa.adicionar_fronteira("Argentina", "Venezuela") # Fronteira fictícia para teste

# 4. Inicializa o Estado do Jogo (Ocupação e Exércitos)
jogador_a = "Jogador Verde"
jogador_b = "Jogador Azul"

# Ocupação inicial
mapa.territorios["Brasil"].proprietario = jogador_a
mapa.territorios["Brasil"].adicionar_exercito(5)

mapa.territorios["Argentina"].proprietario = jogador_b
mapa.territorios["Argentina"].adicionar_exercito(3)

mapa.territorios["Peru"].proprietario = jogador_a
mapa.territorios["Peru"].adicionar_exercito(2)


# 5. Imprime o Mapa
mapa.imprimir_mapa()

# 6. Exemplo de consulta (para a lógica de ataque)
territorio_ataque = "Brasil"
print(f"\n--- Lógica de Ataque ---")
vizinhos = mapa.obter_vizinhos(territorio_ataque)
print(f"Territórios que fazem fronteira com {territorio_ataque}: {vizinhos}")

for vizinho in vizinhos:
    vizinho_obj = mapa.territorios[vizinho]
    if vizinho_obj.proprietario != mapa.territorios[territorio_ataque].proprietario:
        print(f"Pode atacar {vizinho_obj.nome} (Dono: {vizinho_obj.proprietario})!")
