#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/graphviz.hpp>
#include <vector>
#include <random>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <map>

using namespace std;
using namespace boost;
using std::vector;
using std::map;

typedef adjacency_list<vecS, vecS, undirectedS, int, int> Graph;

template <class Palette, class Color>
class color_writer {
public:
    color_writer(Palette palette, Color& color):
    palette_(palette),
    color_(color)
    {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        assert(color_[v] < 6);
        out << "[color=\"" << palette_[color_[v]] << "\"]";
    }
private:
    Palette palette_;
    Color& color_;
};

Graph g;

void random_graph(Graph& g, int nb_vertex, int perc) {
    // Ajout de nb_vertex sommets
    for(int i = 0; i < nb_vertex; ++i)
        add_vertex(0, g);
    
    // Générateur de nombres pseudo-aléatoires
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> dis(0, nb_vertex - 1);
    std::uniform_int_distribution<unsigned long> dis2(1, nb_vertex * perc / 100);
    int j = 0;
    // Ajout d'arêtes
    for (auto pair_it = vertices(g);
         pair_it.first != pair_it.second;
         ++pair_it.first) {
        int nb_edge = dis2(gen);
        for(int i = 0; i < nb_edge; ++i) {
            auto vertex1 = *pair_it.first;
            auto vertex2 = dis(gen);
            if(vertex2 != vertex1)
            {
                add_edge(vertex1, vertex2, j++, g);
            }
        }
    }
}

int coloration(const Graph& g, vector<int>& color) {
    vector<int> vertex(num_vertices(g));
    std::generate_n(vertex.begin(), vertex.size(), []() -> int { static int i = 0; return i++; });
    std::cerr << "sorting" << '\n';
    std::sort(vertex.begin(), vertex.end(),
              [&g](auto a, auto b) { return out_degree(a, g) > out_degree(b, g); }
              );
    std::cerr << "sorted" << '\n';
    color.resize(num_vertices(g), -1);
    int k;
    int colored = num_vertices(g);
    for(k = 0; colored != 0; ++k) {
        std::cerr << "color : " << k << '\n';
        for(auto x : vertex) {
            if(color[x] == -1) {
                bool same_color = false;
                for (auto pair_it = adjacent_vertices(x, g);
                     pair_it.first != pair_it.second;
                     ++pair_it.first) {
                    if(color[*pair_it.first] == k) {
                        same_color = true;
                        break;
                    }
                }
                if(!same_color) {
                    color[x] = k;
                    std::cerr << "vertex : " << x << '\n';
                    --colored;
                }
            }
        }
    }
    return k;
}

bool myfunction (graph_traits<Graph>::edge_iterator i,graph_traits<Graph>::edge_iterator j) { return (g[*i] < g[*j]); }

vector<graph_traits<Graph>::edge_iterator> kruskal (Graph& g)
{	
	bool ajout = true;
	vector<graph_traits<Graph>::edge_iterator> trie;
	vector<graph_traits<Graph>::edge_iterator> choisi;

	for (auto pair_it = edges(g); pair_it.first != pair_it.second; ++pair_it.first)
        //cout << "arretes :" << *pair_it.first << "  poids : " << g[*pair_it.first] << endl;
	{
		trie.push_back(pair_it.first);
	}
	sort(trie.begin(), trie.end(), myfunction);
	int i = 1;
	for (auto pair_it = trie.begin(); pair_it != trie.end(); ++pair_it)
	{
		//cout << g[**pair_it] << endl;
		//choisi.push_back(*pair_it);
		auto s1 = source(**pair_it, g);
		auto s2 = target(**pair_it, g);
		//cout << g[s1] << endl;
		if(g[s1] == 0 && g[s2] == 0)
		{
			choisi.push_back(*pair_it);
			g[s1] = g[s2] = i++;
			continue;
		}
		if((g[s1] == 0 && g[s2] != 0) || (g[s1] != 0 && g[s2] == 0))
		{
			if(g[s1] == 0)
				g[s1] = g[s2];
			else
				g[s2] = g[s1];
			choisi.push_back(*pair_it);
			continue;
		}
		if(g[s1] != 0 && g[s2] != 0)
		{
			for (auto pair_it2 = choisi.begin(); pair_it2 != choisi.end(); ++pair_it2)
			{
				auto s3 = source(**pair_it2, g);
				auto s4 = target(**pair_it2, g);
				if((g[s1] == g[s3] && g[s2] == g[s4]) || (g[s1] == g[4] && g[s2] == g[s3]))
				{
					ajout = false;
					break;
				}
			}
			if(ajout == true)
				choisi.push_back(*pair_it);
			ajout = true;
		}
	}
	return choisi;
}



int main() {
	Graph gg;
    random_graph(g, 50, 2);
    vector<graph_traits<Graph>::edge_iterator> choisi;
    vector<int> color;
    

    choisi = kruskal(g);

	for (auto pair_it = choisi.begin(); pair_it != choisi.end(); ++pair_it)
    {
    	//cout << **pair_it << endl;
    	auto s1 = source(**pair_it, g);
		auto s2 = target(**pair_it, g);
		s1 = add_vertex(gg);
		s2 = add_vertex(gg);
		add_edge(s1, s2, gg);
   	}

   	coloration(gg, color);
    const char* palette[] = { "#ff0000", "#00ff00", "#0000ff", "#ffff00", "#00ffff", "#ff00ff" };
    write_graphviz(std::cout, gg, color_writer<const char**, vector<int>>(palette, color));//, edge_writer<map<int, int>>(poids));
    
    return 0;
}
