#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
using namespace std;

struct ai_move {
	ai_move() {};
	ai_move(int score) : score(score) {};
	int x;
	int y;
	int score;
};

class HexBoard
{
public:
	// Blank player counts as empty space
	enum Player { RED, BLUE, BLANK };

	HexBoard(int n = 10); // creates an NxN hexboard

	HexBoard(const HexBoard &hex_board);

	// Push a certain player into an index within the board.
	void make_index(Player player, int row, int col);

	vector<pair<int, int>> legal_neighbors(int row, int col);

	Player check_winner();

	int longest_sub_length(Player player);

	int get_score(Player player, pair<int, int> cord);

	void next_move();

	Player get_node_value(int row, int col);
	Player get_node_value(pair<int, int> p);

	void print_board();

	void start_game();

	inline int V() { return n; };
private:
	enum game_state {PLAYER, COMPUTER, GAME_OVER};

	vector<vector<Player>> board;

	ai_move HexBoard::minimax(int depth, Player player, HexBoard board_copy, pair<int, int> cord = make_pair(0, 0), int alpha = -INT_MAX, int beta = INT_MAX);

	pair<int, int> current_cord;

	int n;
};

HexBoard::HexBoard(int n)
{
	// Creates an NxN hexboard that is initially filled with blank spaces
	HexBoard::n = n;
	board = vector<vector<Player>>(n, vector<Player>(n));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			board[i][j] = BLANK;
		}
	}

	HexBoard::current_cord = make_pair(0, 0);
}

HexBoard::HexBoard(const HexBoard &hex_board)
{
	n = hex_board.n;
	board = vector<vector<Player>>(hex_board.board);
	current_cord = hex_board.current_cord;
}

void HexBoard::make_index(Player player, int row, int col)
{
	//cout << row << " " << col;
	if (row < V() && row >= 0 && col < V() && col >= 0)
	{
		board[row][col] = player;
	}
}

// This function is to check which index are legal neighbors to a given index
// Min neighbors is 2 max is 6
vector<pair<int, int>> HexBoard::legal_neighbors(int row, int col)
{
	vector<pair<int, int>> container;
	if (row >= 0 && row < V() && col >= 0 && col < V())
	{
		// Set row and col as the origin point of the iterators
		for (int i = row - 1; i < row + 2; i++)
		{
			for (int j = col - 1; j < col + 2; j++)
			{
				// Here I check for legality of the index
				if ((i - row) != (j - col) && i >= 0 && i < V() && j >= 0 && j < V())
				{
					container.push_back(make_pair(i, j)); 
				}
			}
		}
	}
	return container;
}

HexBoard::Player HexBoard::get_node_value(int row, int col)
{
	return board[row][col]; // maybe swap x and y
}

HexBoard::Player HexBoard::get_node_value(pair<int, int> p)
{
	return get_node_value(p.first, p.second); // maybe swap x and y
}

// This function uses a version of Depth First Search to see
// if there is a winner by checking if an index from one of the 
// start locations connects with one of the end locations
HexBoard::Player HexBoard::check_winner()
{
	// Create 2 vectors of pairs that contain coordinates
	vector<pair<int, int>> north_to_south;
	vector<pair<int, int>> west_to_east;

	// Create sister pairs to keep track of where the algorithm has gone
	vector<pair<int, int>> visited_NS;
	vector<pair<int, int>> visited_WE;
	for (int i = 0; i < V(); i++)
	{
		if (get_node_value(0, i) == RED)
		{
			north_to_south.push_back(make_pair(0, i));
		}

		if (get_node_value(i, 0) == BLUE)
		{
			west_to_east.push_back(make_pair(i, 0));
		}
	}

	// Begin DFS checking for North to South, checking for a red winner
	while (!north_to_south.empty())
	{
		pair<int, int> next = north_to_south.back();
		bool visited = (find(visited_NS.begin(), visited_NS.end(), next) != visited_NS.end());
		north_to_south.pop_back();
		if (!visited)
		{
			visited_NS.push_back(next);
			vector<pair<int, int>> w = legal_neighbors(next.first, next.second);
			while (!w.empty())
			{
				bool checker = (find(visited_NS.begin(), visited_NS.end(), w.back()) != visited_NS.end());
				if ((get_node_value(w.back().first, w.back().second) == RED) && !checker)
				{
					north_to_south.push_back(w.back());
				}
				w.pop_back();
			}
		}
	}

	// Same as above except with checking blue
	while (!west_to_east.empty())
	{
		pair<int, int> next = west_to_east.back();
		bool visited = (find(visited_WE.begin(), visited_WE.end(), next) != visited_WE.end());
		west_to_east.pop_back();
		if (!visited)
		{
			visited_WE.push_back(next);
			vector<pair<int, int>> w = legal_neighbors(next.first, next.second);
			while (!w.empty())
			{
				bool checker = (find(visited_WE.begin(), visited_WE.end(), w.back()) != visited_WE.end());
				if ((get_node_value(w.back().first, w.back().second) == BLUE) && !checker)
				{
					west_to_east.push_back(w.back());
				}
				w.pop_back();
			}
		}
	}
	// Checking if the DFS list contains the begining and the end
	for (int i = 0; i < V(); i++)
	{
		pair<int, int> checker = make_pair(V() - 1, i);
		if ((find(visited_NS.begin(), visited_NS.end(), checker) != visited_NS.end()))
		{
			return RED;
		}
	}

	// / Checking if the DFS list contains the begining and the ends
	for (int i = 0; i < V(); i++)
	{
		pair<int, int> checker = make_pair(i, V() - 1);
		if ((find(visited_WE.begin(), visited_WE.end(), checker) != visited_WE.end()))
		{
			return BLUE;
		}
	}
	return BLANK;
}

// This function takes the true sublength of the hex_board
// If a sub-length is becoming large towards the direction of that player's win condition
// The longest_sub length will be updated.
int HexBoard::longest_sub_length(Player player)
{
	vector <vector <pair <int, int> > > differing_paths;

	vector<pair <int, int> > all_hexes;
	for (int i = 0; i < V(); i++)
	{
		for (int j = 0; j < V(); j++)
		{
			all_hexes.push_back(make_pair(i, j));
		}
	}

	int longest = 0;

	while (!all_hexes.empty())
	{
		vector <pair<int, int>> current_nodes;
		current_nodes.push_back(all_hexes.back());
		all_hexes.pop_back();

		int lowest = V();
		int highest = 0;

		while (!current_nodes.empty())
		{
			pair<int, int> current_node = current_nodes.back();
			current_nodes.pop_back();

			if (get_node_value(current_node) == player)
			{
				// Pos updates to be first for RED player
				int pos = player == RED ? current_node.first : current_node.second;

				lowest = lowest > pos ? pos : lowest;
				highest = highest < pos ? pos : highest;

				auto v = legal_neighbors(current_node.first, current_node.second);

				for (unsigned int i = 0; i < v.size(); i++)
				{
					if (get_node_value(v[i]) == player)
					{
						auto all_hexes_pos = find(all_hexes.begin(), all_hexes.end(), v[i]);
						if (all_hexes_pos != all_hexes.end())
						{
							current_nodes.push_back(v[i]);
							all_hexes.erase(all_hexes_pos);
						}
					}
				}
			}
		}
		int length = highest - lowest + 1;
		longest = longest > length ? longest : length;
	}
	return longest;
}

// Calculates the next move for the AI player
// Uses minimax to try and determine this.
void HexBoard::next_move()
{
	HexBoard *board_copy(this);

	ai_move best_move = minimax(4, BLUE, *board_copy, current_cord);

	current_cord = make_pair(best_move.x, best_move.y);


	cout << "Blue moving at (" << best_move.x << ", " << best_move.y << ") score = " << best_move.score << endl;

	make_index(BLUE ,best_move.x, best_move.y);
}

// The implementation of minimax for a hexboard
ai_move HexBoard::minimax(int depth, Player player, HexBoard board_copy, pair<int, int> cord, int alpha, int beta)
{
	//auto neighbors = board_copy.legal_neighbors(cord.first, cord.second);

	vector<pair<int, int>> neighbors;
	for (int i = 0; i < V(); i++)
	{
		for (int j = 0; j < V(); j++)
		{
			neighbors.push_back(make_pair(i, j));
		}
	}

	for (int i = neighbors.size() - 1; i >= 0; i--)
	{
		if (board_copy.get_node_value(neighbors[i]) != BLANK)
		{
			neighbors.erase(neighbors.begin() + i);
		}
	}

	if (depth == 0 || neighbors.empty())
	{
		ai_move move(board_copy.get_score(player == RED ? BLUE : RED, cord));
		move.x = cord.first;
		move.y = cord.second;

		return move;
	}
	ai_move best_score;

	if (player == BLUE)
	{
		best_score = ai_move(-INT_MAX);
		for (unsigned int i = 0; i < neighbors.size(); i++)
		{
			board_copy.make_index(BLUE, neighbors[i].first, neighbors[i].second);
			ai_move v = minimax(depth - 1, RED, board_copy, neighbors[i], alpha, beta);
			board_copy.make_index(BLANK, neighbors[i].first, neighbors[i].second);
			best_score = best_score.score < v.score ? v : best_score;

			alpha = alpha > v.score ? alpha : v.score;

			if (beta <= alpha) break;
		}
	}
	else if (player == RED)
	{
		best_score = ai_move(INT_MAX);
		for (unsigned int i = 0; i < neighbors.size(); i++)
		{
			board_copy.make_index(RED, neighbors[i].first, neighbors[i].second);
			ai_move v = minimax(depth - 1, BLUE, board_copy, neighbors[i], alpha, beta);
			board_copy.make_index(BLANK, neighbors[i].first, neighbors[i].second);
			best_score = best_score.score > v.score ? v : best_score;

			beta = beta < v.score ? beta : v.score;

			if (beta <= alpha) break;
		}
	}

	return best_score;
}

// This is a naive way to get the score from looking at a board state.
int HexBoard::get_score(Player player, pair<int, int> cord)
{
	int score = longest_sub_length(player) * 5;

	auto neighbors = legal_neighbors(cord.first, cord.second);

	for (auto neighbor : neighbors)
	{
		if (get_node_value(neighbor) == player) score++;
	}

	return score;
}

void HexBoard::print_board()
{
	// This offset is intended to make the visual of the hexboard look more like
	// what we expect
	const string indent = " ";
	string offset = "";
	for (unsigned int i = 0; i < board.size(); i++)
	{
		cout << offset << i << " ";
		offset += indent;
		for (unsigned int j = 0; j < board.size(); j++)
		{
			if (board[i][j] == RED)
			{
				cout << "R";
			}
			else if (board[i][j] == BLUE)
			{
				cout << "B";
			}
			else if (board[i][j] == BLANK)
			{
				cout << "*";
			}
		}
		cout << endl;
	}
}

// Starts and runs the game using a simple state machine
void HexBoard::start_game()
{
	cout << "Welcome to HEX!\n";
	cout << "You (the human player) will attempt to go from North to South as the RED player and you will go FIRST\n";
	cout << "Your enemy is the BLUE player who will attempt to go from West to East\n";

	print_board();
	game_state current_game_state = PLAYER;
	while (current_game_state != GAME_OVER) {
		switch (current_game_state) {
		case PLAYER:
			int x, y;
			cout << "Enter a coordinate please!\n";
			cin >> x >> y;
			if (get_node_value(x, y) != BLANK)
			{
				cout << "That index is currently occupied. Skipping your turn.\n";
			}
			else {
				make_index(RED, x, y);
				print_board();
			}

			if (check_winner() == BLANK)
			{
				cout << "AI TURN IS STARTING!\n";
				current_game_state = COMPUTER;
			}
			else {
				current_game_state = GAME_OVER;
			}
			break;

		case COMPUTER:
			cout << "BLUE HAS STARTED TO SEARCH!\n";
			next_move();
			print_board();
			cout << "BLUE's SEARCH IS DONE!\n";
			if (check_winner() == BLANK)
			{
				current_game_state = PLAYER;
			}
			else {
				current_game_state = GAME_OVER;
			}
			break;

		case GAME_OVER:
			cout << "Game is Over!\n";
			print_board();
			break;
		}
	}
}

int main()
{
	HexBoard game(8);
	game.start_game();
	return 0;
}