#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>
using namespace std;
double sum = 0;
double numChars = 0;
//string encodedText = ""; //zmienna do testowania krotkiego ciagu znakow
unordered_map<char, string> codes;
// Tworzymy mapę częstości wystąpień znaków
unordered_map<char, int> freq;
// Struktura dla węzła drzewa Huffmana
struct Node
{
	char ch;
	int freq;
	Node* left, * right;
	Node(char ch, int freq)
	{
		this->ch = ch;
		this->freq = freq;
		this->left = this->right = nullptr;
	}
};
// Klasa dla porównywania węzłów w kolejce priorytetowej
struct Compare
{
	bool operator()(Node* l, Node* r)
	{
		return l->freq > r->freq;
	}
};
// Funkcja do budowy drzewa Huffmana z podanego ciągu znaków i ich częstości wystąpień
Node* buildHuffmanTree(unordered_map<char, int>& freq)
{
	// Tworzymy kolejkę priorytetową
	priority_queue<Node*, vector<Node*>, Compare> pq;
	// Dodajemy każdy znak do kolejki
	for (auto p : freq)
	{
		pq.push(new Node(p.first, p.second));
	}
	// Dopóki w kolejce jest więcej niż jeden węzeł
	while (pq.size() > 1)
	{
		// Pobieramy dwie najmniej często występujące litery
		Node* left = pq.top();
		pq.pop();
		Node* right = pq.top();
		pq.pop();
		// Tworzymy nowy węzeł, którego częstość wystąpienia jest równa sumie częstości wystąpienia obu pobranych węzłów
		Node* node = new Node('\0', left->freq + right->freq);
		node->left = left;
		node->right = right;
		// Dodajemy nowy węzeł do kolejki
		pq.push(node);
	}
	// Zwracamy korzeń drzewa Huffmana
	return pq.top();
}
// Funkcja rekurencyjna do wyświetlania drzewa Huffmana
void printTree(Node* root, int level)
{
	if (root == nullptr)
		return;
	printTree(root->right, level + 1);
	for (int i = 0; i < level; i++)
		cout << " ";
	cout << root->ch << " (" << root->freq << ")" << endl;
	printTree(root->left, level + 1);
}
void generateFixedLengthCodes(Node* root, string str, unordered_map<char, string>& codes, int& codeLength)
{
	if (root == nullptr)
		return;
	// Jeśli węzeł jest liściem, zapisujemy kod dla znaku
	if (!root->left && !root->right)
	{
		codes[root->ch] = str;
		codeLength = str.length();
		sum += freq[root->ch] * str.length();
		numChars += freq[root->ch];
		return;
	}
	// Generujemy kody dla lewego i prawego dziecka
	generateFixedLengthCodes(root->left, str + "0", codes, codeLength);
	generateFixedLengthCodes(root->right, str + "1", codes, codeLength);
}
int main()
{
	// Otwieramy plik
	ifstream file("input.txt");
	// Sprawdzamy, czy plik został otwarty
	if (!file.is_open())
	{
		cout << "Nie można otworzyć pliku!" << endl;
		return 1;
	}
	char ch;
	// Wczytujemy plik i zliczamy częstość wystąpienia każdego znaku
	while (file >> ch)
	{
		freq[ch]++;
	}
	// Budujemy drzewo Huffmana
	Node* root = buildHuffmanTree(freq);
	// Określamy stałą długość kodów
	int codeLength = 8;
	// Tworzymy mapę kodów Huffmana o stałej długości dla każdego znaku
	generateFixedLengthCodes(root, "", codes, codeLength);

	// Zmienna do zliczania liczby bitów potrzebnych do zakodowania ciągu znaków za pomocą kodów o stałej długości
	int fixedLengthBits = 0;
	// Zmienna do zliczania liczby bitów potrzebnych do zakodowania ciągu znaków za pomocą kodów o zmiennym długości
	int variableLengthBits = 0;

	file.clear();
	file.seekg(0, ios::beg);
	while (file >> ch)
	{
		fixedLengthBits += codeLength;
		variableLengthBits += codes[ch].length();
		//encodedText += codes[ch];
	}
	// Obliczamy średnią długość kodu
	double avgCodeLength = variableLengthBits / freq.size();
	double avgLength = sum / numChars;
	//cout << "Ciag zer i jedynek dla krotkiego tekstu: " << encodedText << endl;
	// Wypisujemy wyniki
	cout << "Liczba bitow potrzebna do zakodowania ciagu znakow za pomoca kodow o stalej dlugosci: " << fixedLengthBits << endl;
	cout << "Liczba bitow potrzebna do zakodowania ciagu znakow za pomoca kodow o zmiennej dlugosci: " << variableLengthBits <<
		endl;
	cout << "Srednia dlugosc kodu dla zakodowanej informacji: " << avgCodeLength << endl;
	cout << "Srednia długosc kodu (z tablica czestosci wystapien): " << avgLength << endl;
	// Tworzymy tablicę alfabetu i liczby wystąpień znaków
	vector<pair<char, int>> alphabet;
	for (auto p : freq)
		alphabet.push_back(make_pair(p.first, p.second));
	// Sortujemy tablicę według liczby wystąpień znaków
	sort(alphabet.begin(), alphabet.end(), [](auto& left, auto& right) {
		return left.second < right.second;
		});
	// Wypisujemy tablicę alfabetu i liczby wystąpień znaków
	cout << "Alfabet i liczby wystapien znakow:" << endl;
	for (auto p : alphabet)
		cout << p.first << ": " << p.second << endl;
	// Wypisujemy tablicę wygenerowanych kodów
	cout << "Tabela kodow:" << endl;
	for (auto p : codes)
		cout << p.first << ": " << p.second << endl;
	//cout << "Drzewo Huffmana:" << endl;
	//printTree(root, 0);
	cout << "Bit Rate: " << avgLength << endl;
	double compressionRatio = (double)fixedLengthBits / variableLengthBits;
	cout << "Compression ratio (CR): " << compressionRatio << endl;
	double compressionPercentage = (1 - 1 / compressionRatio) * 100;
	cout << "Compression percentage (CP): " << compressionPercentage << "%" << endl;
	return 0;
}
