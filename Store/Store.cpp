#include <iostream>
#include "Product.h"

using namespace std;

class Product {
public:
    int article;
    string name;
    string category;
    double price;
    int quantity;
    int minQuantity;

    Product() : article(0), price(0), quantity(0), minQuantity(0) {}

    Product(int Article, string Name, string Category, double Price, int Quantity, int MinQuantity)
    {
        article = Article;
        name = Name;
        category = Category;
        price = Price;
        quantity = Quantity;
        minQuantity = MinQuantity;
    }

    void AddQuantity(int amount) 
    {
        if (amount <= 0)
        {
            throw invalid_argument("Количество должно быть положительным");
        }
        quantity += amount;
    }

    void ReduceQuantity(int amount) 
    {
        if (amount <= 0)
        {
            throw invalid_argument("Количество должно быть положительным");
        }
        if (amount > quantity)
        {
            throw runtime_error("Нельзя списать больше, чем есть на складе");
        }
        quantity -= amount;
    }
};

int LoadFromFile(string filename, Product items[], int maxSize)
{
    ifstream file(filename);
    if (!file)
    {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }

    string line;
    int count = 0;

    while(getline(file, line) && count < maxSize) 
    {
        if (line.empty())
        {
            continue;
        }

        string originalLine = line;

        string artStr, nameStr, catStr, priceStr, quanStr, minStr;
        size_t pos = line.find(';');

        pos = line.find(';');
        if (pos == string::npos)
        {
            throw runtime_error("Точка с запятой не найдена");
        }
        artStr = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(';');
        if (pos == string::npos)
        {
            throw runtime_error("Точка с запятой не найдена");
        }
        nameStr = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(';');
        if (pos == string::npos)
        {
            throw runtime_error("Точка с запятой не найдена");
        }
        catStr = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(';');
        if (pos == string::npos)
        {
            throw runtime_error("Точка с запятой не найдена");
        }
        priceStr = line.substr(0, pos);
        line.erase(0, pos + 1);

        pos = line.find(';');
        if (pos == string::npos)
        {
            throw runtime_error("Точка с запятой не найдена");
        }
        quanStr = line.substr(0, pos);
        line.erase(0, pos + 1);

        minStr = line;

        try 
        {
            int article1 = stoi(artStr);
            double price1 = stod(priceStr);
            int quantity1 = stoi(quanStr);
            int minQuantity1 = stoi(minStr);

            if (article1 <= 0 || price1 < 0 || quantity1 < 0 || minQuantity1 < 0)
            {
                throw invalid_argument("Недопустимые значения");
            }

            items[count] = Product(article1, nameStr, catStr, price1, quantity1, minQuantity1);
            count++;
        }
        catch (const exception& e) 
        {
            throw runtime_error("Ошибка данных в строке: " + originalLine + " : " + e.what());
        }
    }
    file.close();

    #ifdef DEBUG_MODE
        cout << "Загружено записей: " << count << endl;
    #endif

    return count;
}

void SaveToFile(string filename, Product items[], int count) 
{
    ofstream file(filename);
    if (!file)
    {
        throw runtime_error("Не удалось открыть файл для записи: " + filename);
    }
    for (int i = 0; i < count; i++) {
        file << items[i].article << ";" << items[i].name << ";" << items[i].category << ";"
             << items[i].price << ";" << items[i].quantity << ";" << items[i].minQuantity << endl;
    }
    file.close();

    #ifdef DEBUG_MODE
        cout << "Сохранено записей: " << count << endl;
    #endif
}

int FindProductByArticle(Product items[], int count, int article) 
{
    for (int i = 0; i < count; i++)
    {
        if (items[i].article == article)
        {
            return i;
        }  
    }
    return -1;
}

void PrintByCategory(Product items[], int count, string category) 
{
    bool found = false;
    for (int i = 0; i < count; i++) 
    {
        if (items[i].category == category) 
        {
            cout << items[i].article << " " << items[i].name << " Цена: "
                 << items[i].price << " Кол-во: " << items[i].quantity << endl;
            found = true;
        }
    }
    if (!found)
    {
        cout << "Товаров в категории \"" << category << "\" нет" << endl;
    }
}

void PrintLowStock(Product items[], int count) 
{
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (items[i].quantity < items[i].minQuantity) 
        {
            cout << items[i].article << " " << items[i].name << " Остаток: " << items[i].quantity
                 << " (мин. " << items[i].minQuantity << ")" << endl;
            found = true;
        }
    }
    if (!found) 
    {
        cout << "Все товары в норме" << endl;
    }
}

double CalculateTotalCost(Product items[], int count) 
{
    double total = 0;
    for (int i = 0; i < count; i++)
    {
        total += items[i].price * items[i].quantity;
    }
    return total;
}

int main()
{
    setlocale(LC_ALL, "Russian");
        
    Product warehouse[MAX_ITEMS];
    int count = 0;
    string filename = "warehouse.txt";

    try 
    {
        count = LoadFromFile(filename, warehouse, MAX_ITEMS);
        cout << "Загружено товаров: " << count << endl;
    }
    catch (const exception& e) 
    {
        cout << "Ошибка загрузки: " << e.what() << endl;
        cout << "Работа с пустым складом" << endl;
    }

    int choice;
    while (true) 
    {
        cout << "\nСКЛАД\n";
        cout << "1.Поиск товара по артикулу\n";
        cout << "2.Вывод товаров по категории\n";
        cout << "3.Товары ниже минимального остатка\n";
        cout << "4.Поступление товара\n";
        cout << "5.Списание товара\n";
        cout << "6.Общая стоимость остатков\n";
        cout << "0.Выход и сохранение\n";
        cout << "Выберите пункт: ";
        cin >> choice;

        if (choice == 0) break;

        try 
        {
            switch (choice) 
            {
            case 1: 
            {
                int art;
                cout << "Введите артикул: ";
                cin >> art;
                int idx = FindProductByArticle(warehouse, count, art);
                if (idx == -1) 
                {
                    cout << "Товар не найден\n";
                }
                else 
                {
                    cout << "Артикул: " << warehouse[idx].article << endl;
                    cout << "Название: " << warehouse[idx].name << endl;
                    cout << "Категория: " << warehouse[idx].category << endl;
                    cout << "Цена: " << warehouse[idx].price << endl;
                    cout << "Кол-во: " << warehouse[idx].quantity << endl;
                    cout << "Мин. остаток: " << warehouse[idx].minQuantity << endl;
                }
                break;
            }
            case 2: 
            {
                string cat;
                cout << "Введите категорию: ";
                cin.ignore();
                getline(cin, cat);
                PrintByCategory(warehouse, count, cat);
                break;
            }
            case 3: 
            {
                PrintLowStock(warehouse, count);
                break;
            }
            case 4: 
            {
                int art, amount;
                cout << "Артикул товара: ";
                cin >> art;
                int idx = FindProductByArticle(warehouse, count, art);
                if (idx == -1) 
                { 
                    cout << "Товар не найден\n"; 
                    break; 
                }
                cout << "Сколько поступило: ";
                cin >> amount;
                warehouse[idx].AddQuantity(amount);
                cout << "Новый остаток: " << warehouse[idx].quantity << endl;
                break;
            }
            case 5: 
            {
                int art, amount;
                cout << "Артикул товара: ";
                cin >> art;
                int idx = FindProductByArticle(warehouse, count, art);
                if (idx == -1) 
                { 
                    cout << "Товар не найден\n"; 
                    break; 
                }
                cout << "Сколько списать: ";
                cin >> amount;
                warehouse[idx].ReduceQuantity(amount);
                cout << "Новый остаток: " << warehouse[idx].quantity << endl;
                break;
            }
            case 6: 
            {
                cout << "Общая стоимость: " << CalculateTotalCost(warehouse, count) << endl;
                break;
            }
            default:
                cout << "Неверный пункт меню\n";
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }
    try 
    {
        SaveToFile(filename, warehouse, count);
        cout << "Данные сохранены\n";
    }
    catch (const exception& e) {
        cout << "Ошибка сохранения: " << e.what() << endl;
    }
    return 0;
}