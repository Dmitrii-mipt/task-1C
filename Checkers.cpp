#include <iostream>
#include <vector>
#include <map>

// const int field_size = 8;

struct Node {
    char color = '.'; // 'w' - white, 'b' - black, '.' - no figure, 'A' - 'H' and '1' - '8' name coordinates
    bool is_queen = false;

    explicit Node (char new_color): color(new_color) {
    }
};

// создаем пустое поле для игры
std::vector<std::vector<Node>> MakeField(const int field_size) {
    std::vector<std::vector<Node>> field(field_size + 1);
    char count = '0' + field_size;
    char let = 'A';
    for (int i = 0; i <= field_size; ++i) {
        field[i].reserve(field_size + 1);
        for (int j = 0; j <= field_size; ++j) {
            if (i == 0 && j == 0) {
                field[i].emplace_back(' ');
            } else if (i == 0) {
                field[i].emplace_back(let);
                ++let;
            } else if (j == 0) {
                field[i].emplace_back(count);
                --count;
            } else {
                field[i].emplace_back('.');
            }
        }
    }
    return field;
}

// заполняем поле черными и белыми пешками
std::vector<std::vector<Node>> BeginGame(const int field_size) {
    std::vector<std::vector<Node>> field = MakeField(field_size);
    for (int i = 1; i <= field_size; ++i) {
        for (int j = 1; j <= field_size; ++j) {
            if (i <= 3 && (i + j) % 2 != 0) {
                field[i][j].color = 'b';
            } else if (i >= 6 && (i + j) % 2 != 0) {
                field[i][j].color = 'w';
            }
        }
    }
    return field;
}

class Checkers {
public:

    explicit Checkers(const int size, char new_player_color): field(BeginGame(size)), field_size(size) {
        player_color = new_player_color;
        bot_color = (player_color == 'b') ? 'w' : 'b';
        count_figures_player = size * size / 4;
        count_figures_bot = size * size / 4;
    }

    // описание хода игрока
    bool PlayerTurn(const std::string& prev_coordinates, const std::string& new_coordinates);

    // описание хода бота
    void BotTurn();

    // вывод текущей версии поля
    void FieldOutput() const;

    [[nodiscard]] int CountFiguresPlayer() const {
        return count_figures_player;
    }

    [[nodiscard]] int CountFiguresBot() const {
        return count_figures_bot;
    }

private:
    // проверки того, в какую сторону может сходить бот и в какой стороне может срубить
    void BotCheckUpLeft(int row, int column, bool& up_left_go, bool& up_left_cut) const;
    void BotCheckUpRight(int row, int column, bool& up_right_go, bool& up_right_cut) const;
    void BotCheckDownLeft(int row, int column, bool& down_left_go, bool& down_left_cut) const;
    void BotCheckDownRight(int row, int column, bool& down_right_go, bool& down_right_cut) const;

    struct Go {
        int row;
        int column;
        int new_row;
        int new_column;

        Go(int index_row, int index_col, int new_index_row, int new_index_col): row(index_row), column(index_col),
        new_row(new_index_row), new_column(new_index_col) {
        }
    };

    struct Cut {
        int row;
        int column;
        int new_row;
        int new_column;

        Cut(int index_row, int index_col, int new_index_row, int new_index_col): row(index_row), column(index_col),
        new_row(new_index_row), new_column(new_index_col) {
        }
    };

    static void AddCoordinate(std::vector<Go>& go, std::vector<Cut>& cut, bool is_go, bool is_cut,
                              int row, int column, int row_go, int column_go, int row_cut, int column_cut);

    // обновление поля
    void FieldUpdate(const std::string& prev_coordinates, const std::string& new_coordinates,
                     char color, bool is_cut_down, const std::string& coordinates_cut);

    void FieldUpdate(int row, int column, int new_row, int new_column, char color, bool is_cut);

    // Проверка того, что координата находится внутри поля
    [[nodiscard]] bool InsideBorder(const std::string& coordinates) const;

    // Проверка того, что ход был сделан правильно: есть координаты внутри поля, игрок ходил своей фигурой
    // и сходил на пустое место
    bool IsRightTurn(const std::string& prev_coordinates, const std::string& new_coordinates, char my_color);

    int LetterToSecondCoordinate(const std::string& coordinates) {
        return letter_coordinates[coordinates[0]];
    }

    int CountToFirstCoordinate(const std::string& coordinates) {
        return count_coordinates[coordinates[1]];
    }

    std::vector<std::vector<Node>> field;
    const int field_size = 8;
    char player_color = 'w';
    char bot_color = 'b';
    char empty_color = '.';
    int count_figures_player = 16;
    int count_figures_bot = 16;
    std::map<char, int> letter_coordinates = {{'A', 1}, {'B', 2}, {'C', 3}, {'D', 4},
                                        {'E', 5}, {'F', 6}, {'G', 7}, {'H', 8}};

    std::map<char, int>  count_coordinates = {{'8', 1}, {'7', 2}, {'6', 3}, {'5', 4},
                                               {'4', 5}, {'3', 6}, {'2', 7}, {'1', 8}};
};

bool Checkers::PlayerTurn(const std::string& prev_coordinates, const std::string& new_coordinates) {
    bool is_cut_down = false;
    std::string coordinates_cut;
    if (IsRightTurn(prev_coordinates, new_coordinates, player_color)) {
        if ((prev_coordinates[0] == new_coordinates[0]) || (prev_coordinates[1] == new_coordinates[1]) ||
            (player_color == 'b' && new_coordinates[1] > prev_coordinates[1] && abs(new_coordinates[1] - prev_coordinates[1]) == 1) ||
            (player_color == 'w' && new_coordinates[1] < prev_coordinates[1] && abs(new_coordinates[1] - prev_coordinates[1]) == 1)) {
            return false;
        }
        if ((abs(prev_coordinates[1] - new_coordinates[1]) == 1) &&
            (abs(prev_coordinates[0] - new_coordinates[0]) == 1)) {
            FieldUpdate(prev_coordinates, new_coordinates, player_color, is_cut_down, coordinates_cut);
            FieldOutput();
            return true;
        }
        if ((abs(prev_coordinates[1] - new_coordinates[1]) > 2) ||
            (abs(prev_coordinates[0] - new_coordinates[0]) > 2)) {
            return false;
        }
        if (abs(prev_coordinates[1] - new_coordinates[1]) == 2 && abs(prev_coordinates[0] - new_coordinates[0]) == 2) {
            for (char i = 'A'; i <= 'A' + field_size; ++i) {
                if ((prev_coordinates[0] < i && new_coordinates[0] > i) ||
                    (prev_coordinates[0] > i && new_coordinates[0] < i)) {
                    coordinates_cut += i; // буквенная координата срубленной пешки
                    break;
                }
            }
            for (char i = '1'; i <= '1' + field_size; ++i) {
                if ((prev_coordinates[1] < i && new_coordinates[1] > i) ||
                    (prev_coordinates[1] > i && new_coordinates[1] < i)) {
                    coordinates_cut += i; // числовая координата срубленной пешки
                    break;
                }
            }
            if (field[CountToFirstCoordinate(coordinates_cut)][LetterToSecondCoordinate(coordinates_cut)].color != bot_color) {
                return false;
            }
            is_cut_down = true;
            --count_figures_bot;
        } else {
            return false;
        }
    } else {
        return false;
    }
    FieldUpdate(prev_coordinates, new_coordinates, player_color, is_cut_down, coordinates_cut);
    FieldOutput();
    return true;
}

void Checkers::BotCheckUpLeft(const int row, const int column, bool& up_left_go, bool& up_left_cut) const {
    int row_go = row - 1;
    int column_go = column - 1;
    if (row_go < 1 || column_go < 1 || field[row_go][column_go].color != empty_color) {
        up_left_go = false;
    } else {
        if (bot_color == 'b') {
            up_left_go = false;
        } else {
            up_left_go = true;
        }
    }
    int row_cut = row - 2;
    int column_cut = column - 2;
    if (row_cut < 1 || column_cut < 1 || field[row_go][column_go].color != player_color || field[row_cut][column_cut].color != empty_color) {
        up_left_cut = false;
    } else {
        up_left_cut = true;
    }
}

void Checkers::BotCheckUpRight(const int row, const int column, bool& up_right_go, bool& up_right_cut) const {
    int row_go = row - 1;
    int column_go = column + 1;
    if (row_go < 1 || column_go > field_size || field[row_go][column_go].color != empty_color) {
        up_right_go = false;
    } else {
        if (bot_color == 'b') {
            up_right_go = false;
        } else {
            up_right_go = true;
        }
    }
    int row_cut = row - 2;
    int column_cut = column + 2;
    if (row_cut < 1 || column_cut > field_size || field[row_go][column_go].color != player_color || field[row_cut][column_cut].color != empty_color) {
        up_right_cut = false;
    } else {
        up_right_cut = true;
    }
}

void Checkers::BotCheckDownLeft(const int row, const int column, bool& down_left_go, bool& down_left_cut) const {
    int row_go = row + 1;
    int column_go = column - 1;
    if (row_go > field_size || column_go < 1 || field[row_go][column_go].color != empty_color) {
        down_left_go = false;
    } else {
        if (bot_color == 'b') {
            down_left_go = true;
        } else {
            down_left_go = false;
        }
    }
    int row_cut = row + 2;
    int column_cut = column - 2;
    if (row_cut > field_size || column_cut < 1 || field[row_go][column_go].color != player_color || field[row_cut][column_cut].color != empty_color) {
        down_left_cut = false;
    } else {
        down_left_cut = true;
    }
}

void Checkers::BotCheckDownRight(const int row, const int column, bool& down_right_go, bool& down_right_cut) const {
    int row_go = row + 1;
    int column_go = column + 1;
    if (row_go > field_size || column_go > field_size || field[row_go][column_go].color != empty_color) {
        down_right_go = false;
    } else {
        if (bot_color == 'b') {
            down_right_go = true;
        } else {
            down_right_go = false;
        }
    }
    int row_cut = row + 2;
    int column_cut = column - 2;
    if (row_cut > field_size || column_cut < 1 || field[row_go][column_go].color != player_color || field[row_cut][column_cut].color != empty_color) {
        down_right_cut = false;
    } else {
        down_right_cut = true;
    }
}

void Checkers::BotTurn() {
    std::vector<Go> go; // координаты куда можем сходить
    std::vector<Cut> cut; // координаты куда сходить, чтобы срубить
    for (int row = 1; row <= field_size; ++row) {
        for (int column = 1; column <= field_size; ++column) {
            if (field[row][column].color == bot_color) {
                bool is_go = false;
                bool is_cut = false;
                BotCheckUpLeft(row, column, is_go, is_cut);
                AddCoordinate(go, cut, is_go, is_cut, row, column, row - 1, column - 1, row - 2, column - 2);

                BotCheckUpRight(row, column, is_go, is_cut);
                AddCoordinate(go, cut, is_go, is_cut, row, column, row - 1, column + 1, row - 2, column + 2);

                BotCheckDownLeft(row, column, is_go, is_cut);
                AddCoordinate(go, cut, is_go, is_cut, row, column, row + 1, column - 1, row + 2, column - 2);

                BotCheckDownRight(row, column, is_go, is_cut);
                AddCoordinate(go, cut, is_go, is_cut, row, column, row + 1, column + 1, row + 2, column + 2);
            }
        }
    }
    if (!cut.empty()) {
        int rand_index = std::rand() % cut.size();
        FieldUpdate(cut[rand_index].row, cut[rand_index].column, cut[rand_index].new_row, cut[rand_index].new_column, bot_color, true);
        --count_figures_player;
        FieldOutput();
    } else {
        int rand_index = std::rand() % go.size();
        FieldUpdate(go[rand_index].row, go[rand_index].column, go[rand_index].new_row, go[rand_index].new_column, bot_color, false);
        FieldOutput();
    }
}

void Checkers::FieldOutput() const {
    for (int i = 0; i <= field_size; ++i) {
        for (int j = 0; j <= field_size; ++j) {
            std::cout << field[i][j].color << '\t';
        }
        std::cout << '\n';
    }
}

void Checkers::AddCoordinate(std::vector<Go>& go, std::vector<Cut>& cut, bool is_go, bool is_cut,
                             int row, int column, int row_go, int column_go, int row_cut, int column_cut) {
    if (is_go) {
        go.emplace_back(row, column, row_go, column_go);
    }
    if (is_cut) {
        cut.emplace_back(row, column, row_cut, column_cut);
    }
}

void Checkers::FieldUpdate(const std::string& prev_coordinates, const std::string& new_coordinates,
                           char color, bool is_cut_down, const std::string& coordinates_cut) {
    field[CountToFirstCoordinate(prev_coordinates)][LetterToSecondCoordinate(prev_coordinates)].color = empty_color;
    field[CountToFirstCoordinate(new_coordinates)][LetterToSecondCoordinate(new_coordinates)].color = color;
    if (is_cut_down) {
        field[CountToFirstCoordinate(coordinates_cut)][LetterToSecondCoordinate(coordinates_cut)].color = empty_color;
    }
}

void Checkers::FieldUpdate(int row, int column, int new_row, int new_column, char color, bool is_cut) {
    field[row][column].color = empty_color;
    field[new_row][new_column].color = color;
    if (is_cut) {
        int row_cut = -1;
        int col_cut = -1;
        for (int i = 1; i <= field_size; ++i) {
            if ((i > row && i < new_row) || (i < row && i > new_row)) {
                row_cut = i;
            }
            if ((i > column && i < new_column) || (i < column && i > new_column)) {
                col_cut = i;
            }
            if (row_cut != -1 && col_cut != -1) {
                break;
            }
        }
        field[row_cut][col_cut].color = empty_color;
    }
}

[[nodiscard]] bool Checkers::InsideBorder(const std::string& coordinates) const {
    if (coordinates.size() == 2 && coordinates[0] <= 'A' + field_size && coordinates[0] >= 'A' &&
        coordinates[1] <= '1' + field_size && coordinates[1] >= '1') {
        return true;
    }
    return false;
}

bool Checkers::IsRightTurn(const std::string& prev_coordinates, const std::string& new_coordinates, char my_color) {
    if (InsideBorder(prev_coordinates) && InsideBorder(new_coordinates) &&
        field[CountToFirstCoordinate(prev_coordinates)][LetterToSecondCoordinate(prev_coordinates)].color == my_color &&
        field[CountToFirstCoordinate(new_coordinates)][LetterToSecondCoordinate(new_coordinates)].color == empty_color) {
        return true;
    }
    return false;
}

void Game(const int field_size) {
    std::cout << "Каким цветом вы хотите играть?\nЕсли белым - нажмите w, если черным - нажмите b ";
    char let;
    std::cin >> let;
    std::cout << "Белые ходят первыми\n";
    Checkers check(field_size, let);
    bool is_player_turn = false;
    if (let == 'w') {
        is_player_turn = true;
    }
    std::cout << "Исходное поле\n";
    check.FieldOutput();
    std::cout << '\n';
    while (check.CountFiguresPlayer() && check.CountFiguresBot()) {
        if (is_player_turn) {
            std::cout << "Введите начальные и конечные координаты фигуры через пробел (в виде A4 B5)\n";
            std::string prev_coordinates, new_coordinates;
            while (true) {
                std::cin >> prev_coordinates >> new_coordinates;
                std::cout << "Ход игрока";
                if (!check.PlayerTurn(prev_coordinates, new_coordinates)) {
                    std::cout << "Вы неккоректно ввели координаты, пожалуйста выберите координаты заново\n";
                } else {
                    break;
                }
            }
            std::cout << '\n';
            is_player_turn = false;
        } else {
            std::cout << "Ход бота\n";
            check.BotTurn();
            is_player_turn = true;
            std::cout << '\n';
        }
    }
    if (check.CountFiguresBot() == 0) {
        std::cout << "Игрок выиграл\n";
    } else {
        std::cout << "Бот выиграл\n";
    }
}

int main() {
    const int field_size = 8;
    Game(field_size);
}
