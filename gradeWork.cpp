#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <vector>
#include <exception>
#include <string>

class TamagotchiException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Tamagotchi has encountered a critical issue!";
    }
};

class Attribute {
protected:
    int value;
    int min_value;
    int max_value;

public:
    Attribute(int initial, int min_val, int max_val) : value(initial), min_value(min_val), max_value(max_val) {}

    virtual void increase(int amount) { value = std::min(max_value, value + amount); }
    virtual void decrease(int amount) { value = std::max(min_value, value - amount); }
    int get() const { return value; }
};

class Health : public Attribute {
public:
    Health() : Attribute(50, 0, 100) {}
};

class Hunger : public Attribute {
public:
    Hunger() : Attribute(50, 0, 100) {}
};

class Happiness : public Attribute {
public:
    Happiness() : Attribute(50, 0, 100) {}
};

class Energy : public Attribute {
public:
    Energy() : Attribute(50, 0, 100) {}
};

class Cleanliness : public Attribute {
public:
    Cleanliness() : Attribute(50, 0, 100) {}
};

class Age {
private:
    int age;

public:
    Age() : age(0) {}
    void increase() { ++age; }
    int get() const { return age; }
};

class SaveLoad {
public:
    void saveToFile(const std::string& filename, std::vector<std::reference_wrapper<Attribute>>& attributes, const Age& age, int money, const std::string& name) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << name << "\n";
            for (const auto& attribute : attributes) {
                file << attribute.get().get() << "\n";
            }
            file << age.get() << "\n";
            file << money << "\n";
            file.close();
        }
        else {
            throw TamagotchiException();
        }
    }

    void loadFromFile(const std::string& filename, std::vector<std::reference_wrapper<Attribute>>& attributes, Age& age, int& money, std::string& name) {
        std::ifstream file(filename);
        if (file.is_open()) {
            std::getline(file, name);
            int value;
            for (auto& attribute : attributes) {
                file >> value;
                attribute.get().increase(value - attribute.get().get());
            }
            file >> value;
            for (int i = 0; i < value; ++i) {
                age.increase();
            }
            file >> money;
            file.close();
        }
        else {
            throw TamagotchiException();
        }
    }
};

class RandomEvent {
public:
    virtual void triggerEvent(Attribute& hunger, Attribute& happiness, Attribute& energy, Attribute& cleanliness) = 0;
};

class SimpleRandomEvent : public RandomEvent {
public:
    void triggerEvent(Attribute& hunger, Attribute& happiness, Attribute& energy, Attribute& cleanliness) override {
        int event = rand() % 4;
        switch (event) {
        case 0:
            hunger.decrease(10);
            std::cout << "Your Tamagotchi got hungry.\n";
            break;
        case 1:
            happiness.decrease(10);
            std::cout << "Your Tamagotchi is feeling sad.\n";
            break;
        case 2:
            energy.decrease(10);
            std::cout << "Your Tamagotchi is tired.\n";
            break;
        case 3:
            cleanliness.decrease(10);
            std::cout << "Your Tamagotchi got dirty.\n";
            break;
        }
    }
};

template<typename T>
class MiniGame {
public:
    virtual ~MiniGame() {}
    virtual void play(T& happiness, int& money) = 0;
};

class GuessingGame : public MiniGame<Attribute> {
public:
    void play(Attribute& happiness, int& money) override {
        int number = rand() % 100 + 1;
        int guess;
        std::cout << "Guess the number between 1 and 100: ";
        std::cin >> guess;

        if (guess == number) {
            happiness.increase(20);
            money += 10; // Награда за игру
            std::cout << "Congratulations! You guessed the number and earned 10 coins.\n";
        }
        else {
            happiness.decrease(10);
            std::cout << "Sorry, the number was " << number << ". Better luck next time!\n";
        }
    }
};

class ArithmeticGame : public MiniGame<Attribute> {
public:
    void play(Attribute& happiness, int& money) override {
        int a = rand() % 10 + 1;
        int b = rand() % 10 + 1;
        int answer;
        std::cout << "What is " << a << " + " << b << "? ";
        std::cin >> answer;

        if (answer == a + b) {
            happiness.increase(20);
            money += 10; // Награда за игру
            std::cout << "Correct! Great job. You earned 10 coins.\n";
        }
        else {
            happiness.decrease(10);
            std::cout << "Incorrect. The answer is " << a + b << ". Try again!\n";
        }
    }
};

class Economy {
private:
    int money;

public:
    Economy() : money(50) {}

    int getMoney() {
        return money;
    }

    void earnMoney(int amount) {
        money += amount;
    }

    bool spendMoney(int amount) {
        if (money >= amount) {
            money -= amount;
            return true;
        }
        return false;
    }

    void buyFood(Attribute& hunger) {
        if (spendMoney(20)) {
            hunger.increase(20);
            std::cout << "You bought food and fed your Tamagotchi.\n";
        }
        else {
            std::cout << "Not enough money to buy food.\n";
        }
    }

    void buyMedicine(Attribute& health) {
        if (spendMoney(30)) {
            health.increase(30);
            std::cout << "You bought medicine and healed your Tamagotchi.\n";
        }
        else {
            std::cout << "Not enough money to buy medicine.\n";
        }
    }

    void buyToy(Attribute& happiness) {
        if (spendMoney(15)) {
            happiness.increase(15);
            std::cout << "You bought a toy and made your Tamagotchi happy.\n";
        }
        else {
            std::cout << "Not enough money to buy a toy.\n";
        }
    }
};

class Interaction {
public:
    void showOptions() const {
        std::cout << "\nOptions:\n";
        std::cout << "1. Feed\n";
        std::cout << "2. Play\n";
        std::cout << "3. Sleep\n";
        std::cout << "4. Clean\n";
        std::cout << "5. Status\n";
        std::cout << "6. Shop\n";
        std::cout << "7. Quit\n";
        std::cout << "Choose an option: ";
    }

    char getUserChoice() const {
        char choice;
        std::cin >> choice;
        return choice;
    }

    void handleChoice(char choice, Attribute& hunger, Attribute& happiness, Attribute& energy, Attribute& cleanliness, Economy& economy) {
        switch (choice) {
        case '1':
            hunger.decrease(10);
            std::cout << "You fed your Tamagotchi.\n";
            break;
        case '2': {
            std::unique_ptr<MiniGame<Attribute>> game;
            int gameChoice;
            std::cout << "Choose a game:\n1. Guessing Game\n2. Arithmetic Game\n";
            std::cin >> gameChoice;

            if (gameChoice == 1) {
                game = std::make_unique<GuessingGame>();
            }
            else {
                game = std::make_unique<ArithmeticGame>();
            }
            int econ = economy.getMoney();
            game->play(happiness, econ);
            energy.decrease(5);
            break;
        }
        case '3':
            energy.increase(20);
            hunger.increase(5);
            std::cout << "Your Tamagotchi slept and regained energy.\n";
            break;
        case '4':
            cleanliness.increase(20);
            std::cout << "You cleaned your Tamagotchi.\n";
            break;
        case '5':
            std::cout << "Hunger: " << hunger.get() << "\n";
            std::cout << "Happiness: " << happiness.get() << "\n";
            std::cout << "Energy: " << energy.get() << "\n";
            std::cout << "Cleanliness: " << cleanliness.get() << "\n";
            std::cout << "Money: " << economy.getMoney() << "\n";
            break;
        case '6':
            showShopOptions();
            char shopChoice;
            std::cin >> shopChoice;
            handleShopChoice(shopChoice, hunger, happiness, cleanliness, economy);
            break;
        case '7':
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid option. Try again.\n";
        }
    }

    void showShopOptions() const {
        std::cout << "\nShop:\n";
        std::cout << "1. Buy Food (20 coins)\n";
        std::cout << "2. Buy Medicine (30 coins)\n";
        std::cout << "3. Buy Toy (15 coins)\n";
        std::cout << "Choose an option: ";
    }

    void handleShopChoice(char choice, Attribute& hunger, Attribute& happiness, Attribute& cleanliness, Economy& economy) {
        switch (choice) {
        case '1':
            economy.buyFood(hunger);
            break;
        case '2':
            economy.buyMedicine(cleanliness);
            break;
        case '3':
            economy.buyToy(happiness);
            break;
        default:
            std::cout << "Invalid shop option. Try again.\n";
        }
    }
};

class Tamagotchi {
private:
    std::string name;
    Attribute hunger{ 50, 0, 100 };
    Attribute happiness{ 50, 0, 100 };
    Attribute energy{ 50, 0, 100 };
    Attribute cleanliness{ 50, 0, 100 };
    Age age;
    Health health;
    SaveLoad saveLoad;
    SimpleRandomEvent randomEvent;
    Interaction interaction;
    Economy economy;

    static int instanceCount;

public:
    Tamagotchi() { instanceCount++; }

    ~Tamagotchi() { instanceCount--; }

    static int getInstanceCount() { return instanceCount; }

    void setName(const std::string& newName) {
        name = newName;
    }

    const std::string& getName() const {
        return name;
    }

    void run() {
        srand(static_cast<unsigned>(time(0)));
        std::vector<std::reference_wrapper<Attribute>> attributes = { hunger, happiness, energy, cleanliness };
        try {
            int econ = economy.getMoney();
            saveLoad.loadFromFile("tamagotchi_status.txt", attributes, age, econ, name);
        }
        catch (const TamagotchiException& e) {
            std::cerr << e.what() << "\n";
        }

        std::cout << "Welcome to Tamagotchi, " << name << "!\n";

        char choice;
        do {
            interaction.showOptions();
            choice = interaction.getUserChoice();
            interaction.handleChoice(choice, hunger, happiness, energy, cleanliness, economy);

            if (isAlive()) {
                randomEvent.triggerEvent(hunger, happiness, energy, cleanliness);
                age.increase();
                std::cout << "Age: " << age.get() << "\n";
            }
            else {
                std::cout << "Your Tamagotchi has passed away. RIP.\n";
                break;
            }
        } while (choice != '7');

        try {
            saveLoad.saveToFile("tamagotchi_status.txt", attributes, age, economy.getMoney(), name);
        }
        catch (const TamagotchiException& e) {
            std::cerr << e.what() << "\n";
        }
    }

    bool isAlive() const {
        return hunger.get() > 0 && happiness.get() > 0 && energy.get() > 0 && cleanliness.get() > 0;
    }
};

int Tamagotchi::instanceCount = 0;

int main() {
    Tamagotchi pet;

    std::string name;
    std::cout << "Enter a name for your Tamagotchi: ";
    std::getline(std::cin, name);
    pet.setName(name);

    pet.run();
    std::cout << "Current Tamagotchi instances: " << Tamagotchi::getInstanceCount() << "\n";
    return 0;
}
