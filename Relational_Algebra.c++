//packages 
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <cctype>
#include <utility>

// dont need to use std::cout and can just you cout
using namespace std;

// creates the table like an array
struct Table{
    string name;
    vector<string> colNames;
    vector<string> colTypes;
    vector<vector<string>> rows;
};

// this converts an input string to all lowercase letters to help avoid errors
static inline string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
             [](unsigned char c){ return std::tolower(c); });
    return s;
}

// reads a full line of input as a string, then converts that string into integar,
// incase user puts 42, it will return 42 
int readInt(const string& prompt) {
    cout << prompt;
    string line; getline(cin, line);
    try { return stoi(line); }
    catch (...) { cout << "[input] expected integer, got '" << line << "'. Using 0.\n"; return 0; }
}


// same as the one above(readint, but this would be for string returning a string)
string readLine(const string& prompt){
    cout << prompt;
    string s;
    getline(cin,s);
    return s;
}

// this will print a the data without the table format 
void printRaw(const Table& t) {
    for(const auto& row : t.rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i) cout << ", ";
            cout << row[i];
        }
        cout << "\n";
    }
}

// this will print the data but with the table, and it will look better
void printPretty(const Table& t){
    vector<size_t> width(t.colNames.size(), 0);
    for (size_t c = 0; c < t.colNames.size(); ++c) {
        width[c] = t.colNames[c].size();
    }
    for (const auto& row : t.rows) {
        for (size_t c = 0; c < row.size(); ++c) {
            width[c] = max(width[c], row[c].size());
        }
    }
    auto line = [&]() {
        cout << "+";
        for (size_t c = 0; c <width.size(); ++c) {
            cout << string(width[c] + 2, '-') << "+";
        }
        cout <<"\n";
    };
    line();
    cout << "|";
    for (size_t c = 0; c <width.size(); ++c) {
        cout << " " << left << setw((int)width[c]) << t.colNames[c] << " |";
    }
    cout <<"\n";
    line();
    for(const auto& row : t.rows) {
        cout << "|";
        for(size_t c = 0; c <width.size(); ++c) {
            cout << " " << left << setw((int)width[c]) << row[c] << " |";
        }
        cout << "\n";
    }
    line();
}

// this will clean up text, remove unwanted spaces to help avoid erros
static inline string trim(string s) {
    while (!s.empty() && isspace((unsigned char)s.front())) s.erase(s.begin());
    while (!s.empty() && isspace((unsigned char)s.back()))  s.pop_back();
    return s;
}

// checks if the given string is actually an int to help avoid errors
static inline bool isInt(const string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '-' || s[0] == '+') i = 1;
    if (i >= s.size()) return false;
    for (; i < s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

// reads the lines until the user puts in END, this is for when the user 
// uses the Paste method/wants to enter the whole table at once
static inline string readMultilineUntilEND(const string& prompt) {
    cout << prompt;
    string line, all;
    while (true) {
        if (!getline(cin, line)) break;
        if (trim(line) == "END") break;
        all += line;
        all.push_back('\n');
    }
    return all;
}

// this takes any comma separated string and organizes in the correct format
// to avoid mismatches and avoid errors
vector<string> splitCSV(const string& s) {
    vector<string> out;
    string cur;
    for (char ch : s) {
        if (ch == ',') {
            out.push_back(trim(cur));
            cur.clear();
        } else {
            cur.push_back(ch);
        }
    }
    out.push_back(trim(cur));
    return out;
}

// ignores upper/lowercase to find columnn, helps avoid upper/lower case erros
int findCol(const Table& t, const string& colName) {
    string q = toLower(colName);
    for (int i = 0; i < (int)t.colNames.size(); ++i)
        if (toLower(t.colNames[i]) == q) return i;
    return -1;
}

// checks when you compare two intergers and returns false if written wrong
bool cmpInt(int a, const string& op, int b) {
    if (op == "=")  return a == b;
    if (op == ">=") return a >= b;
    if (op == "<=") return a <= b;
    if (op == ">")  return a >  b;
    if (op == "<")  return a <  b;
    return false;
}

// takes a condition splits its into parts to make it easier for the code to read, for selection
void parseCondition(const string& cond, string& col, string& op, string& val) {
    size_t pos;
    if ((pos = cond.find(">=")) != string::npos) { op = ">="; }
    else if ((pos = cond.find("<=")) != string::npos) { op = "<="; }
    else if ((pos = cond.find(">"))  != string::npos) { op = ">"; }
    else if ((pos = cond.find("<"))  != string::npos) { op = "<"; }
    else if ((pos = cond.find("="))  != string::npos) { op = "="; }
    col = trim(cond.substr(0, pos));
    val = trim(cond.substr(pos + op.size()));
}

// this implements selection, once given a condtion it filters through rows of given table 
Table sigma(const Table& t, const string& condition) {
    string col, op, val; parseCondition(condition, col, op, val);
    int ci = findCol(t, col);
    if (ci < 0) { cout << "[sigma] unknown column: " << col << "\n"; return t; }
    Table out = t;
    out.rows.clear();
    if (toLower(t.colTypes[ci]) == "int") {
        int v = stoi(val);
        for (const auto& row : t.rows) {
            int x = stoi(row[ci]);
            if (cmpInt(x, op, v)) out.rows.push_back(row);
        }
    } else {
        for (const auto& row : t.rows) {
            if (op == "=" && row[ci] == val) out.rows.push_back(row);
        }
    }
    return out;
}

// implements projection, keeps only columns wanted and removes duplicates
Table project(const Table& t, const vector<string>& cols) {
    vector<int> idx;
    idx.reserve(cols.size());
    for (const auto& col : cols) {
        int i = findCol(t, col);
        if (i < 0) { cout << "[project] unknown column: " << col << "\n"; return t; }
        idx.push_back(i);
    }
    Table out;
    out.name = t.name + "_proj";
    for (int i : idx) {
        out.colNames.push_back(t.colNames[i]);
        out.colTypes.push_back(i < (int)t.colTypes.size() ? t.colTypes[i] : string("string"));
    }
    set<vector<string>> seen;
    for (const auto& row : t.rows) {
        vector<string> r; r.reserve(idx.size());
        for (int i : idx) r.push_back(row[i]);
        if (seen.insert(r).second) out.rows.push_back(std::move(r));
    }
    return out;
}

// finds columns with the same names from two tables
// for each pair of rows, keeps the pair from the left table with non common of right table
// when there is no common columns this does cartesian product
Table naturalJoin(const Table& A, const Table& B) {
    unordered_map<string,int> aIdx, bIdx;
    for (int i = 0; i < (int)A.colNames.size(); ++i) aIdx[toLower(A.colNames[i])] = i;
    for (int j = 0; j < (int)B.colNames.size(); ++j) bIdx[toLower(B.colNames[j])] = j;

    vector<pair<int,int>> common;
    for (auto& kv : aIdx) {
        auto it = bIdx.find(kv.first);
        if (it != bIdx.end()) common.push_back({kv.second, it->second});
    }

    Table out;
    out.name = A.name + "_join_" + B.name;
    out.colNames = A.colNames;
    out.colTypes = A.colTypes;

    vector<int> bKeep;
    for (int j = 0; j < (int)B.colNames.size(); ++j) {
        string lname = toLower(B.colNames[j]);
        if (!aIdx.count(lname)) {
            bKeep.push_back(j);
            out.colNames.push_back(B.colNames[j]);
            out.colTypes.push_back(j < (int)B.colTypes.size() ? B.colTypes[j] : string("string"));
        }
    }

    bool cartesian = common.empty();

    for (const auto& ra : A.rows) {
        for (const auto& rb : B.rows) {
            bool ok = true;
            if (!cartesian) {
                for (auto& p : common) {
                    int ai = p.first;
                    int bj = p.second;
                    if (ra[ai] != rb[bj]) { ok = false; break; }
                }
            }
            if (ok) {
                vector<string> merged = ra;
                for (int j : bKeep) merged.push_back(rb[j]);
                out.rows.push_back(std::move(merged));
            }
        }
    }
    return out;
}

// checks to see if two tables have the same set up, if not returns false, for set operations
bool sameHeader(const Table& A, const Table& B) {
    if (A.colNames.size() != B.colNames.size()) return false;
    for (size_t i = 0; i < A.colNames.size(); ++i) {
        if (toLower(A.colNames[i]) != toLower(B.colNames[i])) return false;
    }
    return true;
}

// implements union, needs table a and b to be the same, avoids duplicates 
Table setUnion(const Table& A, const Table& B) {
    if (!sameHeader(A, B)) {
        cout << "[union] set mismatch; please make sure columns are in order and named the same.\n";
        return A;
    }
    Table out = A;
    set<vector<string>> seen;
    for (const auto& r : A.rows) { seen.insert(r); }
    for (const auto& r : B.rows) {
        if (seen.insert(r).second) out.rows.push_back(r);
    }
    return out;
}

// implements intersection, needs table a and b to be the same, keeps rows that are the same, no duplicate, 
Table setIntersect(const Table& A, const Table& B) {
    if (!sameHeader(A, B)) {
        cout << "[intersect] set mismatch; please make sure columns are in order and named the same.\n";
        return A;
    }
    Table out = A;
    out.rows.clear();
    set<vector<string>> inB(B.rows.begin(), B.rows.end());
    set<vector<string>> added;
    for (const auto& r : A.rows) {
        if (inB.count(r) && added.insert(r).second) out.rows.push_back(r);
    }
    return out;
}

//implements difference, needs table a and b to be the same, keeps what appear left table and not in right table 
Table setDifference(const Table& A, const Table& B) {
    if (!sameHeader(A, B)) {
        cout << "[difference] set mismatch, please make sure columns are in order and named the same.\n";
        return A;
    }
    Table out = A;
    out.rows.clear();
    set<vector<string>> inB(B.rows.begin(), B.rows.end());
    for (const auto& r : A.rows) {
        if (!inB.count(r)) out.rows.push_back(r);
    }
    return out;
}

// this is to create that relax type format, locates the () {} to split header/body
// takes name first before hitting the ( then takes the column list inside and ends at )
// takes the rows from {}, and checks it line by line, ignores empty spaces and splits on commas to get each value
// assumes all input is valid as well 
Table parseRelationText(const string& block) {
    string s = trim(block);

    size_t pL = s.find('(');
    size_t pR = s.find(')', pL == string::npos ? 0 : pL);
    size_t eq = s.find('=', pR == string::npos ? 0 : pR);
    size_t braceL = s.find('{', eq == string::npos ? 0 : eq);
    size_t braceR = s.rfind('}');

    string name = trim(s.substr(0, pL));
    string colsStr = (pL != string::npos && pR != string::npos && pR > pL)
        ? s.substr(pL + 1, pR - pL - 1)
        : "";
    vector<string> colNames = splitCSV(colsStr);

    string rowsBlock = (braceL != string::npos && braceR != string::npos && braceR > braceL)
        ? s.substr(braceL + 1, braceR - braceL - 1)
        : "";

    vector<vector<string>> rows;
    {
        istringstream iss(rowsBlock);
        string line;
        while (getline(iss, line)) {
            string t = trim(line);
            if (t.empty()) continue;
            if (!t.empty() && t.back() == ',') t.pop_back();
            vector<string> vals = splitCSV(t);
            rows.push_back(vals);
        }
    }

    vector<string> colTypes(colNames.size(), "string");
    if (!rows.empty()) {
        for (size_t c = 0; c < colNames.size(); ++c) {
            bool allInt = true;
            for (const auto& r : rows) {
                if (c >= r.size() || !isInt(trim(r[c]))) { allInt = false; break; }
            }
            if (allInt) colTypes[c] = "int";
        }
    }

    Table t;
    t.name = name;
    t.colNames = std::move(colNames);
    t.colTypes = std::move(colTypes);
    t.rows = std::move(rows);
    return t;
}

// parsers a relational algabra express into a table 
struct RAParser {
    const string& s;
    size_t i = 0;
    unordered_map<string, Table>& db;
    RAParser(const string& src, unordered_map<string, Table>& database) : s(src), db(database) {}
    // eats alllll whitespace lol
    void skip() { while (i < s.size() && isspace((unsigned char)s[i])) ++i; }
    // matches keyword without advancing the cursor
    bool startsWith(const string& t) const { return s.compare(i, t.size(), t) == 0; }
    // identify table name, lower case it to reduce erros, move it along
    string readIdent() {
        size_t j = i;
        while (j < s.size() && (isalnum((unsigned char)s[j]) || s[j]=='_')) ++j;
        string id = s.substr(i, j - i);
        i = j;
        return toLower(id);
    }
    // reads until it hits a whitespace or () for like conditions and column lists
    string readToken() {
        size_t j = i;
        while (j < s.size() && !isspace((unsigned char)s[j]) && s[j] != '(' && s[j] != ')') ++j;
        string tkn = s.substr(i, j - i);
        i = j;
        return tkn;
    }
    // read an expression that is wrapped in parentheses, getting its values
    Table parseParenExpr() {
        ++i;
        Table t = parseExpr();
        skip();
        if (i < s.size() && s[i] == ')') ++i;
        return t;
    }
    // read either a table or parenthsixed expression by name
    Table parseAtom() {
    skip();
    if (i < s.size() && s[i] == '(') return parseParenExpr();
    string name = readIdent();
    auto it = db.find(name);
    if (it == db.end()) { cout << "[ref] unknown table: " << name << "\n"; return Table{}; }
    return it->second;
    }
    // reads 0 symbol that apply to the expression on their right, if not then reutnr parseAtom()
    Table parseFactor() {
        skip();
        if (i < s.size()) {
            if (s[i] == '#' || startsWith("sigma")) {
                if (s[i] == '#') ++i; else i += 5;
                skip();
                string cond = readToken();
                skip();
                Table rhs = parseFactor();
                return sigma(rhs, cond);
            }
            if (s[i] == '$' || startsWith("pi")) {
                if (s[i] == '$') ++i; else i += 2;
                skip();
                string colsTok = readToken();
                vector<string> cols = splitCSV(colsTok);
                skip();
                Table rhs = parseFactor();
                return project(rhs, cols);
            }
        }
        return parseAtom();
    }
    // start with a factor then fold any sequence of ops left to right, then give back final result
    Table parseExpr() {
        Table cur = parseFactor();
        while (true) {
            skip();
            if (i >= s.size()) break;
            if (s[i] == '%' || startsWith("join")) {
                if (s[i] == '%') ++i; else i += 4;
                skip();
                Table rhs = parseFactor();
                cur = naturalJoin(cur, rhs);
                continue;
            }
            if (s[i] == '|' || startsWith("union")) {
                if (s[i] == '|') ++i; else i += 5;
                skip();
                Table rhs = parseFactor();
                cur = setUnion(cur, rhs);
                continue;
            }
            if (s[i] == '&' || startsWith("intersect")) {
                if (s[i] == '&') ++i; else i += 9;
                skip();
                Table rhs = parseFactor();
                cur = setIntersect(cur, rhs);
                continue;
            }
            if (s[i] == '-' || startsWith("minus") || startsWith("diff") || startsWith("difference")) {
                if (s[i] == '-') ++i;
                else if (startsWith("minus")) i += 5;
                else if (startsWith("difference")) i += 10;
                else i += 4;
                skip();
                Table rhs = parseFactor();
                cur = setDifference(cur, rhs);
                continue;
            }
            break;
        }
        return cur;
    }
};


int main(){
    // print statments to provide information 
    cout <<"INFORMATION BEFORE STARTING: \n";
    cout <<"Selection=#, Projection=$, Join=%, Union=|, intersection=&, and Difference=-\n";
    cout <<"Examples:  #condition TableName   |   $col1,col2 TableName   |   Table1 % Table2\n";

    // holds all tables made
    vector<Table> database;

    // for making tables until the users enter "n"
    while (true) {
        // asks user to choose to make his own tables or create relax type table 
        string mode = toLower(readLine("Add table by (I)nteractive prompts or (P)aste Relax-style text? [I/P]: "));
        if (!mode.empty() && mode[0] == 'p') {
            string blk = readMultilineUntilEND(
                "\nPaste relation text, end with a single line containing 'END'\n"
                "Example:\n"
                "Employees (EID, Name, Age) = {\n"
                "  E1, John, 32\n"
                "  E2, Alice, 28\n"
                "}\n"
                "END\n\n> "
            );
            // puts it into a table, 
            Table t = parseRelationText(blk);
            database.push_back(t);
        } else {
            // this for when the user wants to handmake a table gets all inputs 
            Table t;
            t.name = readLine("Enter table name: ");

            int nCols = readInt("Number of columns: ");
            t.colNames.resize(nCols);
            t.colTypes.resize(nCols);

            for (int i = 0; i < nCols; ++i) {
                t.colNames[i] = readLine(" column " + to_string(i) + " name: ");
                t.colTypes[i] = readLine(" Column " + to_string(i) + " type (int/string): ");
            }

            int nRows = readInt("Number of rows to insert: ");
            t.rows.resize(nRows, vector<string>(nCols));
            for (int r = 0; r < nRows; ++r) {
                cout << "Row" << r << " values (enter each value when prompted): \n";
                for (int c = 0; c < nCols; ++c) {
                    t.rows[r][c] = readLine(" " + t.colNames[c] + ": ");
                }
            }
            database.push_back(t);
        }
        // gives the option to make anoter table 
        string more = readLine("\nAdd another table? (y/n): ");
        if (more != "y" && more != "Y") break;
    }

    // builds a catalog case insenstivive to check and look up
    unordered_map<string, Table> db;
    for (const auto& t : database) db[toLower(t.name)] = t;
    // shows both tables to the user both formats
    cout << "\n=== All Tables Created ===\n";
    for (const auto& t : database) {
        cout << "\nTable: " << t.name << " (raw)\n";
        printRaw(t);
        cout << "\nTable: " << t.name << " (pretty)\n";
        printPretty(t);
    }

    // loop that prints the results raw and pretty tables 
    while (true) {
        string expr = readLine("\nEnter RA expression (right-to-left). Empty line to quit:\n> ");
        if (trim(expr).empty()) break;

        RAParser parser(expr, db);
        Table res = parser.parseExpr();

        cout << "\nResult (raw)\n";
        printRaw(res);
        cout << "\nResult (pretty)\n";
        printPretty(res);
    }

    return 0;
}
