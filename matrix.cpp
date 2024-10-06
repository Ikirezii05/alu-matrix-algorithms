#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <regex>
#include <stdexcept>

class SparseMatrix {
private:
    int rows;
    int cols;
    std::map<std::pair<int, int>, int> elements;

public:
    SparseMatrix(int num_rows, int num_cols) : rows(num_rows), cols(num_cols) {}

    static SparseMatrix from_file(const std::string& matrix_file_path) {
        std::ifstream file(matrix_file_path);
        if (!file.is_open()) {
            throw std::runtime_error("File not found: " + matrix_file_path);
        }

        std::string line;
        std::getline(file, line);
        int total_rows = std::stoi(line.substr(5));
        std::getline(file, line);
        int total_cols = std::stoi(line.substr(5));

        SparseMatrix sparse_matrix(total_rows, total_cols);

        std::regex element_regex(R"(\((\d+),\s*(\d+),\s*(-?\d+)\))");
        std::smatch match;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            if (std::regex_match(line, match, element_regex)) {
                int row = std::stoi(match[1]);
                int col = std::stoi(match[2]);
                int value = std::stoi(match[3]);
                sparse_matrix.set_element(row, col, value);
            } else {
                throw std::runtime_error("Invalid format in file: " + line);
            }
        }

        return sparse_matrix;
    }

    int get_element(int row, int col) const {
        auto it = elements.find({row, col});
        return it != elements.end() ? it->second : 0;
    }

    void set_element(int row, int col, int value) {
        if (row >= rows) rows = row + 1;
        if (col >= cols) cols = col + 1;
        elements[{row, col}] = value;
    }

    SparseMatrix add(const SparseMatrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices must have the same dimensions for addition.");
        }

        SparseMatrix result(rows, cols);

        for (const auto& elem : elements) {
            result.set_element(elem.first.first, elem.first.second, elem.second);
        }

        for (const auto& elem : other.elements) {
            int current_value = result.get_element(elem.first.first, elem.first.second);
            result.set_element(elem.first.first, elem.first.second, current_value + elem.second);
        }

        return result;
    }

    SparseMatrix subtract(const SparseMatrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            throw std::invalid_argument("Matrices must have the same dimensions for subtraction.");
        }

        SparseMatrix result(rows, cols);

        for (const auto& elem : elements) {
            result.set_element(elem.first.first, elem.first.second, elem.second);
        }

        for (const auto& elem : other.elements) {
            int current_value = result.get_element(elem.first.first, elem.first.second);
            result.set_element(elem.first.first, elem.first.second, current_value - elem.second);
        }

        return result;
    }

    SparseMatrix multiply(const SparseMatrix& other) const {
        if (cols != other.rows) {
            throw std::invalid_argument("Number of columns of first matrix must equal number of rows of second matrix.");
        }

        SparseMatrix result(rows, other.cols);

        for (const auto& elem : elements) {
            for (int k = 0; k < other.cols; ++k) {
                int other_value = other.get_element(elem.first.second, k);
                if (other_value != 0) {
                    int current_value = result.get_element(elem.first.first, k);
                    result.set_element(elem.first.first, k, current_value + elem.second * other_value);
                }
            }
        }

        return result;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "rows=" << rows << "\ncols=" << cols << "\n";
        for (const auto& elem : elements) {
            oss << "(" << elem.first.first << ", " << elem.first.second << ", " << elem.second << ")\n";
        }
        return oss.str();
    }

    void save_to_file(const std::string& file_path) const {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file for writing: " + file_path);
        }
        file << to_string();
    }
};

void performCalculations() {
    try {
        std::map<char, std::pair<std::string, std::string>> matrix_operations = {
            {'1', {"addition", "add"}},
            {'2', {"subtraction", "subtract"}},
            {'3', {"multiplication", "multiply"}}
        };

        std::cout << "Available operations:\n";
        for (const auto& op : matrix_operations) {
            std::cout << op.first << ": " << op.second.first << "\n";
        }

        std::string matrix_file_path1, matrix_file_path2;
        std::cout << "Enter the file path for the first matrix: ";
        std::cin >> matrix_file_path1;
        SparseMatrix matrix1 = SparseMatrix::from_file(matrix_file_path1);
        std::cout << "First matrix loading........\n";

        std::cout << "Enter the file path for the second matrix: ";
        std::cin >> matrix_file_path2;
        SparseMatrix matrix2 = SparseMatrix::from_file(matrix_file_path2);
        std::cout << "Second matrix loading.......\n";

        char operation_choice;
        std::cout << "Choose an operation (1, 2, or 3): ";
        std::cin >> operation_choice;

        auto it = matrix_operations.find(operation_choice);
        if (it == matrix_operations.end()) {
            throw std::invalid_argument("Invalid operation choice.");
        }

        SparseMatrix result_matrix(0, 0);
        if (it->second.second == "add") {
            result_matrix = matrix1.add(matrix2);
        } else if (it->second.second == "subtract") {
            result_matrix = matrix1.subtract(matrix2);
        } else if (it->second.second == "multiply") {
            result_matrix = matrix1.multiply(matrix2);
        }

        std::cout << "Output of " << it->second.first << "........\n";

        std::string output_file_path;
        std::cout << "Enter the file path to save the result: ";
        std::cin >> output_file_path;
        result_matrix.save_to_file(output_file_path);
        std::cout << "Output file saved to " << output_file_path << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    performCalculations();
    return 0;
}