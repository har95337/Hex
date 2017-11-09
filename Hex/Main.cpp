#include <vector>
#include <functional>
#include <iostream>
#include <string>
using namespace std;
class HexBoard
{
public:
	enum Player { RED, BLUE, BLANK };

	HexBoard(int n = 10); // creates an NxN hexboard

	void make_index(Player player, int row, int col);

	vector<pair<int, int>> legal_neighbors(int row, int col);

	Player check_winner();

	//bool DFS();

	Player get_node_value(int row, int col);

	void print_board();

	inline int V() { return n; };
private:
	vector<vector<Player>> board;

	int n;
};

HexBoard::HexBoard(int n)
{
	HexBoard::n = n;
	board = vector<vector<Player>>(n, vector<Player>(n));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			board[i][j] = BLANK;
		}
	}
}

void HexBoard::make_index(Player player, int row, int col)
{
	board[row][col] = player;
}

vector<pair<int, int>> HexBoard::legal_neighbors(int row, int col)
{
	vector<pair<int, int>> container;
	if (row >= 0 && row < V() && col >= 0 && col < V())
	{
		for (int i = row - 1; i < row + 2; i++)
		{
			for (int j = col - 1; j < col + 2; j++)
			{
				if ((i - row) != (j - col) && i >= 0 && i < V() && j >= 0 && j < V())
				{
					container.push_back(make_pair(i, j)); // Maybe switch i and j
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

HexBoard::Player HexBoard::check_winner()
{
	pair<bool, bool> winner = make_pair(false, false);
	vector<pair<int, int>> north_to_south;
	vector<pair<int, int>> west_to_east;

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
	// North to South
	for (int i = 0; i < V(); i++)
	{
		pair<int, int> checker = make_pair(V() - 1, i);
		if ((find(visited_NS.begin(), visited_NS.end(), checker) != visited_NS.end()))
		{
			return RED;
		}
	}

	// EAST to WEST
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

void HexBoard::print_board()
{
	const string indent = " ";
	string offset = "";
	for (int i = 0; i < board.size(); i++)
	{
		cout << offset << i << " ";
		offset += indent;
		for (int j = 0; j < board.size(); j++)
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

int main()
{
	HexBoard board_a;
	HexBoard board_b;
	HexBoard board_c;
	for (int i = 0; i < board_a.V(); i++)
	{
		board_a.make_index(board_a.RED, i, 0);
		board_b.make_index(board_b.BLUE, 0, i);
	}

	for (int i = 1; i < board_c.V(); i++)
	{
		board_c.make_index(board_c.RED, i, 0);
	}


	cout << "Board A" << endl;
	board_a.print_board();

	cout << "Board B" << endl;
	board_b.print_board();

	cout << "Board C" << endl;
	board_c.print_board();

	if (board_a.check_winner() == board_a.RED && board_a.check_winner() != board_a.BLUE)
	{
		cout << "Winner of Board A is RED!" << endl;
	}
	if (board_b.check_winner() == board_b.BLUE && board_b.check_winner() != board_b.RED)
	{
		cout << "Winner of Board B is BLUE!" << endl;
	}
	if (board_c.check_winner() == board_c.BLANK)
	{
		cout << "NO CURRENT WINNER FOR BOARD C!" << endl;
	}
	getchar();
	return 0;
}