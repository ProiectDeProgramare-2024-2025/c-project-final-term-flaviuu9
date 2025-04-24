#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARS 100
#define MAX_LOADED_FILES 10

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RESET   "\033[0m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"

char loaded_files[MAX_LOADED_FILES][100];
int loaded_file_count = 0;

// Car struct
typedef struct {
    char chassis_number[20];
    int mileage;
    int year;
    char body_type[20];
    char brand[20];
    float price;
} Car;

Car cars[MAX_CARS];
int car_count = 0;
int csv_loaded = 0;

// Clears the screen
void clear_screen() {
    #ifdef _WIN32
        system("cls");  // For Windows
    #else
        system("clear");  // For Linux/macOS
    #endif
}

// Adds a new car
void add_car() {
    clear_screen();
    printf("=== Add Car ===\n");

    if (car_count >= MAX_CARS) {
        printf("Car list is full!\n");
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    printf("✅ " YELLOW "Chassis number" RESET " (max 19 characters, format:GH5437): ");
    scanf("%19s", cars[car_count].chassis_number);

    printf("✅ " YELLOW "Mileage" RESET " (positive integer, format: 123456): ");
    while (scanf("%d", &cars[car_count].mileage) != 1 || cars[car_count].mileage < 0) {
        printf("🚫 " RED "Invalid input" RESET ". Please enter a positive integer for mileage: ");
        while (getchar() != '\n');
    }

    printf("✅ " YELLOW "Year of fabrication" RESET " (format: yyyy, range: 1900–2025): ");
    while (scanf("%d", &cars[car_count].year) != 1 || cars[car_count].year < 1900 || cars[car_count].year > 2025) {
        printf("🚫 " RED "Invalid year" RESET ". Enter a year between 1900 and 2025: ");
        while (getchar() != '\n');
    }

    printf("✅ " YELLOW "Body type" RESET " (max 19 characters, format: SEDAN, SUV, COUPE): ");
    scanf("%19s", cars[car_count].body_type);

    printf("✅ " YELLOW "Brand" RESET " (max 19 characters, format: TOYOTA, BMW): ");
    scanf("%19s", cars[car_count].brand);

    printf("✅ " YELLOW "Price" RESET " (positive integer, format: 12345 EUR): ");
    while (scanf("%f", &cars[car_count].price) != 1 || cars[car_count].price <= 0) {
        printf("🚫 " RED "Invalid input" RESET ". Please enter a positive price: ");
        while (getchar() != '\n');
    }

    car_count++;
    printf("\n" GREEN "✅ Car added successfully!" RESET "\n");
    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

// Loads car data from a CSV file specified by the user (but only once per file)
void load_cars_from_csv() {
    if (loaded_file_count >= MAX_LOADED_FILES) {
        printf(RED "Maximum number of files already loaded." RESET "\n");
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    char filename[100];
    printf(CYAN "Enter the CSV filename to load cars from (e.g., cars.csv): " RESET);
    getchar();
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';  // Remove trailing newline

    // Check if file was already loaded
    for (int i = 0; i < loaded_file_count; i++) {
        if (strcmp(loaded_files[i], filename) == 0) {
            printf(RED "File '%s' has already been loaded." RESET "\n", filename);
            printf("\nPress Enter to return to main menu...");
            getchar(); getchar();
            clear_screen();
            return;
        }
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf(RED "File '%s' not found or cannot be opened." RESET "\n", filename);
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    char line[256];
    int loaded_count = 0;

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        printf(RED "CSV file is empty or unreadable." RESET "\n");
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    while (fgets(line, sizeof(line), file) && car_count < MAX_CARS) {
        Car car;
        char *token = strtok(line, ",");
        if (token) strncpy(car.chassis_number, token, 19);

        token = strtok(NULL, ",");
        if (token) car.mileage = atoi(token);

        token = strtok(NULL, ",");
        if (token) car.year = atoi(token);

        token = strtok(NULL, ",");
        if (token) strncpy(car.body_type, token, 19);

        token = strtok(NULL, ",");
        if (token) strncpy(car.brand, token, 19);

        token = strtok(NULL, ",");
        if (token) car.price = atof(token);

        cars[car_count++] = car;
        loaded_count++;
    }

    fclose(file);

    // Mark file as loaded
    strncpy(loaded_files[loaded_file_count++], filename, 99);

    printf(GREEN "Loaded %d cars from '%s'." RESET "\n", loaded_count, filename);
    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

// Displays all the cars
void display_cars() {
    clear_screen();
    printf("=== " CYAN "Display Cars" RESET " ===\n");

    if (car_count == 0) {
        printf("No cars available.\n");
    } else {
        for (int i = 0; i < car_count; i++) {
            printf("%d. " BLUE "%s" RESET " | %d km | %d | %s | %s | " GREEN "$%.2f" RESET "\n",
                   i + 1,
                   cars[i].chassis_number,
                   cars[i].mileage,
                   cars[i].year,
                   cars[i].body_type,
                   cars[i].brand,
                   cars[i].price);
        }
    }
    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

// Sorts the cars by price in ascending or descending order
void sort_by_price(int sort_order) {
    clear_screen();
    printf("=== " CYAN "Sort Cars by Price" RESET " ===\n");

    for (int i = 0; i < car_count - 1; i++) {
        for (int j = i + 1; j < car_count; j++) {
            if ((sort_order == 1 && cars[i].price > cars[j].price) ||
                (sort_order == 2 && cars[i].price < cars[j].price)) {
                Car temp = cars[i];
                cars[i] = cars[j];
                cars[j] = temp;
            }
        }
    }
    printf(GREEN "Cars sorted by price!" RESET "\n");
    display_cars();
    clear_screen();
}

// Sorts the cars by mileage in ascending or descending order
void sort_by_mileage(int sort_order) {
    clear_screen();
    printf("=== " CYAN "Sort Cars by Mileage" RESET " ===\n");

    for (int i = 0; i < car_count - 1; i++) {
        for (int j = i + 1; j < car_count; j++) {
            if ((sort_order == 1 && cars[i].mileage > cars[j].mileage) ||
                (sort_order == 2 && cars[i].mileage < cars[j].mileage)) {
                Car temp = cars[i];
                cars[i] = cars[j];
                cars[j] = temp;
            }
        }
    }
    printf(GREEN "Cars sorted by mileage!" RESET "\n");
    display_cars();
    clear_screen();
}

// Filters cars by the brand
void filter_by_brand() {
    clear_screen();
    printf("=== " CYAN "Filter by Brand" RESET " ===\n");

    char brand[20];
    printf("Enter brand to filter by (max 19 characters, format: TOYOTA, BMW): ");
    scanf("%s", brand);

    int found = 0;
    for (int i = 0; i < car_count; i++) {
        if (strcmp(cars[i].brand, brand) == 0) {
            printf("%d. " BLUE "%s" RESET " | %d km | %d | %s | %s | " GREEN "$%.2f" RESET "\n",
                   i + 1,
                   cars[i].chassis_number,
                   cars[i].mileage,
                   cars[i].year,
                   cars[i].body_type,
                   cars[i].brand,
                   cars[i].price);
            found = 1;
        }
    }
    if (!found) {
        printf(RED "No cars found for brand '%s'." RESET "\n", brand);
    }
    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

// Allows the user to buy a car from the list and it's added to the transaction history
void buy_car() {
    clear_screen();
    printf("=== " CYAN "Buy Car" RESET " ===\n");

    if (car_count == 0) {
        printf("No cars available to buy.\n");
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    for (int i = 0; i < car_count; i++) {
        printf("%d. %s %s - " GREEN "%.2f USD" RESET "\n", i + 1, cars[i].brand, cars[i].body_type, cars[i].price);
    }

    int choice;
    printf("Select the number of the car to buy (0 to cancel): ");
    scanf("%d", &choice);

    if (choice <= 0 || choice > car_count) {
        printf(RED "Cancelled or invalid choice." RESET "\n");
        printf("\nPress Enter to return to main menu...");
        getchar(); getchar();
        clear_screen();
        return;
    }

    // Save in file
    FILE *f = fopen("transactions_history.txt", "a");
    if (f != NULL) {
        Car selected = cars[choice - 1];
        fprintf(f, "Brand: %s, Model: %s, Price: %.2f EUR, Year: %d, Chassis: %s\n",
                selected.brand, selected.body_type, selected.price, selected.year, selected.chassis_number);
        fclose(f);
    } else {
        printf(RED "Unable to save the order. Please try again." RESET "\n");
    }

    printf(GREEN "You have successfully bought the car!" RESET "\n");

    // Remove the car from stock
    for (int i = choice - 1; i < car_count - 1; i++) {
        cars[i] = cars[i + 1];
    }
    car_count--;

    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

void showTransactionHistory() {
    clear_screen();
    printf("=== " CYAN "Transaction History" RESET " ===\n");

    FILE *f = fopen("transactions_history.txt", "r");
    if (f == NULL) {
        printf(RED "No transaction history found." RESET "\n");
    } else {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            char brand[30], model[30], chassis[30];
            int year;
            float price;
            if (sscanf(line, "Brand: %[^,], Model: %[^,], Price: %f EUR, Year: %d, Chassis: %s", brand, model, &price, &year, chassis) == 5) {
                printf(YELLOW "Brand: %s" RESET", Model: %s, " GREEN "Price: %.2f EUR" RESET ", Year: %d, " BLUE "Chassis: %s" RESET "\n",
                       brand, model, price, year, chassis);
            } else {
                printf("%s", line);
            }
        }
        fclose(f);
    }

    printf("\nPress Enter to return to main menu...");
    getchar(); getchar();
    clear_screen();
}

// Main menu
void menu() {
    int choice;
    do {
        clear_screen();
        printf("\n=== " CYAN "Vehicle Buying/Selling System" RESET " ===\n");
        printf("1 - Add Car\n");
        printf("2 - Display Cars\n");
        printf("3 - Sort by Price\n");
        printf("4 - Sort by Mileage\n");
        printf("5 - Filter by Brand\n");
        printf("6 - Buy Car (Save to History)\n");
        printf("7 - Load Cars from CSV File\n");
        printf("8 - Show Transaction History\n");
        printf("0 - Exit\n");
        printf(YELLOW "Enter your choice: " RESET);
        scanf("%d", &choice);

        clear_screen();  // Clear screen after each choice

        switch (choice) {
            case 1: add_car(); break;
            case 2: display_cars(); break;
            case 3: {
                int order;
                printf("Sort by price:\n1 - Ascending\n2 - Descending\nEnter your choice: ");
                scanf("%d", &order);
                sort_by_price(order);
                break;
            }
            case 4: {
                int order;
                printf("Sort by mileage:\n1 - Ascending\n2 - Descending\nEnter your choice: ");
                scanf("%d", &order);
                sort_by_mileage(order);
                break;
            }
            case 5: filter_by_brand(); break;
            case 6: buy_car(); break;
            case 7: load_cars_from_csv(); break;
            case 8: showTransactionHistory(); break;
            case 0: clear_screen(); printf(CYAN "Exiting..." RESET "\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 0);
}

int main() {
    menu();
    return 0;
}