#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <string>

using namespace std;

struct Node
{
    char ch;
    int freq;
    Node *left;
    Node *right;

    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}
};

struct compare
{
    bool operator()(Node *l, Node *r)
    {
        return l->freq > r->freq;
    }
};

void buildHuffmanTree(ifstream &in, priority_queue<Node *, vector<Node *>, compare> &pq)
{
    map<char, int> charFreq;
    char ch;
    while (in.get(ch))
    {
        charFreq[ch]++;
    }

    for (auto it : charFreq)
    {
        pq.push(new Node(it.first, it.second));
    }

    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        Node *parent = new Node('#', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }
}

void generateCodes(Node *root, string code, map<char, string> &codes)
{
    if (!root)
        return;

    if (root->ch != '#')
    {
        codes[root->ch] = code;
    }

    generateCodes(root->left, code + '0', codes);
    generateCodes(root->right, code + '1', codes);
}

void writeTree(Node *root, ofstream &out, map<char, string> &codes)
{
    if (!root)
        return;

    out << root->ch;

    writeTree(root->left, out, codes);
    writeTree(root->right, out, codes);
}

void writeCompressedFile(ifstream &in, ofstream &out, map<char, string> &codes)
{
    out << endl;
    char ch;
    while (in.get(ch))
    {
        out << codes[ch];
    }
}

string decToBin(int num)
{
    string binary = "";
    while (num > 0)
    {
        binary = to_string(num % 2) + binary;
        num /= 2;
    }
    return binary;
}

void writeASCIIFile(ifstream &in, ofstream &out)
{
    char ch;
    int ASCII;
    while (in.get(ch))
    {
        ASCII = static_cast<int>(ch);
        out << decToBin(ASCII);
    }
}

void decompressTree(ifstream &in, Node *&root)
{
    char ch;
    in.get(ch);

    if (ch == '#')
    {
        root = new Node('#', 0);
        decompressTree(in, root->left);
        decompressTree(in, root->right);
    }
    else
    {
        root = new Node(ch, 0);
    }
}

void decompressFile(ifstream &in, ofstream &out, Node *root)
{
    char ch;
    Node *temp = root;
    while (in.get(ch))
    {
        if (ch == '0')
        {
            temp = temp->left;
        }
        else if (ch == '1')
        {
            temp = temp->right;
        }

        if (temp->ch != '#')
        {
            out << temp->ch;
            temp = root;
        }
    }
}

int main()
{

    int choice;
    cout << "Enter 1 to compress, 2 to decompress: ";
    cin >> choice;

    if (choice != 1 && choice != 2)
    {
        cerr << "Invalid choice." << endl;
        return 1;
    }

    string inputFilename, outputFilename;
    cout << "Enter input filename: ";
    cin >> inputFilename;
    cout << "Enter output filename: ";
    cin >> outputFilename;

    ifstream in(inputFilename);
    ofstream out(outputFilename);
    ofstream out2("ASCII.txt");

    if (!in.is_open())
    {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    if (!out.is_open())
    {
        cerr << "Error opening output file." << endl;
        return 1;
    }

    priority_queue<Node *, vector<Node *>, compare> pq;
    map<char, string> codes;
    Node *root = nullptr;

    if (choice == 1)
    {
        if (!out2.is_open())
        {
            cerr << "Error opening output file." << endl;
            return 1;
        }

        buildHuffmanTree(in, pq);
        root = pq.top();
        generateCodes(root, "", codes);
        writeTree(pq.top(), out, codes);
        in.close();

        in.open(inputFilename);
        writeCompressedFile(in, out, codes);
        in.close();

        in.open(inputFilename);
        writeASCIIFile(in, out2);

        cout << "File compressed successfully." << endl;
    }

    else
    {
        decompressTree(in, root);
        in.close();

        in.open(inputFilename);
        decompressFile(in, out, root);

        cout << "File decompressed successfully." << endl;
    }

    in.close();
    out.close();

    return 0;
}
