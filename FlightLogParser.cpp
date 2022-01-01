#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <map>
#include <vector>

#undef max

int n;
int lines;

std::string native_flightlog_path;
std::string modded_flightlog_path;

struct Aircraft {
    std::string aircraft_manufacturer = "";
    std::string aircraft_type = "";
};

struct Flight {
    std::string date = "ERROR!";
    std::string departure_airport = "----";
    std::string arrival_airport = "----";
    short unsigned int landings_count = 0;
    float total_hours = 0.0;
    float night_hours = 0.0;
    float instrument_hours = 0.0;
    float cross_country_hours = 0.0;
    std::string tailnumber = "";
    std::string aircraft_manufacturer = "";
    std::string aircraft_type = "";
};

void change_settings() {

    std::ifstream fin;
    fin.open("./settings.txt");

    std::string path;

    try {
        if (!fin) {
            throw std::invalid_argument("Settings file not found!");
        }
    }
    catch (std::invalid_argument const& ex) {
        std::cerr << "\nCERR: " << ex.what() << '\n';
    }

    fin >> path;
    std::cout << "Current logbook path: " << path << std::endl;
    fin.close();

    std::cout << "Please enter native logbook file location and name" << std::endl;
    std::cin.ignore();
    std::getline(std::cin, path);

    std::ofstream fout;
    fout.open("./settings.txt");
    fout << path << std::endl;

    native_flightlog_path = path;

    fout.close();
};

Aircraft aircraft_type_and_manufacturer_resolver(std::string input) {
    std::map <std::string, int> mapping;

    mapping["727-200Adv"] = 0;
    mapping["Rotate-MD-80-XP11"] = 1;
    mapping["B733"] = 2;
    mapping["B38M"] = 3;
    mapping["a321_StdDef"] = 4;
    mapping["a321"] = 5;
    mapping["A340-600_StdDef"] = 6;
    mapping["A340-600"] = 7;
    mapping["tu154"] = 8;
    mapping["29A_XP11"] = 9;
    mapping["a320neo"] = 10;
    mapping["Cessna_172SP"] = 11;

    switch (mapping[input]) {
        case 0: return{ "Boeing", "737-200 Advanced" }; break;
        case 1: return{ "McDonnell Douglas", "MD-88" }; break;
        case 2: return{ "Boeing", "737-300" }; break;
        case 3: return{ "Boeing", "737-800" }; break;
        case 4:
        case 5: return{ "Airbus", "A321" }; break;
        case 6:
        case 7: return{ "Airbus", "A340-600" }; break;
        case 8: return{ "Tupolev", "TU154M" }; break;
        case 9: return{ "Aero", "L-29" }; break;
        case 10: return{ "Airbus", "A320neo" }; break;
        case 11: return{ "Cessna", "172SP" }; break;
        default: return{ "Unknown", "Unknown" }; break;
    }

}

void input_from_file(std::vector<Flight>& Flights) {
    std::ifstream fin;
    fin.open("./settings.txt");

    try {
        if (!fin) {
            throw std::invalid_argument("Settings file not found!");
        }
    }
    catch (std::invalid_argument const& ex) {
        std::cerr << "\nCERR: " << ex.what() << '\n';
        change_settings();
    }

    char temp[50] = "";
    fin.getline(temp, 50);
    native_flightlog_path = temp;
    fin.close();

    std::ifstream linecounter{ native_flightlog_path };
    lines = std::count(std::istream_iterator<char>(linecounter >> std::noskipws), {}, '\n');
    linecounter.close();

    std::cout << native_flightlog_path << " " << lines << std::endl;

    std::ifstream readlog;
    readlog.open(native_flightlog_path.c_str());

    try {
        if (!readlog) {
            throw std::invalid_argument("Flight log file not found!");
        }
    }
    catch (std::invalid_argument const& ex) {
        std::cerr << "\nCERR: " << ex.what() << '\n';
    }

    readlog.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    readlog.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i < lines - 3; i++) {
        std::string fldate = "", dep = "", arr = "";
        short unsigned int dummy, ldg_cnt = 0;
        float total_h = 0.0, night_h = 0.0, ifr_h = 0.0, cc_h = 0.0;
        std::string tail, aircraft, aircraft_man, aircraft_type;

        readlog >> dummy >> fldate >> dep >> arr >> ldg_cnt >> total_h >> night_h >> ifr_h >> cc_h >> tail >> aircraft;
        char cdate[7] = "nodate", temp[3] = "00";
        strcpy_s(cdate, fldate.c_str());             // yymmdd
        temp[0] = cdate[0]; temp[1] = cdate[1];      // yymmdd yy
        cdate[0] = cdate[4]; cdate[1] = cdate[5];    // ddmmdd yy
        cdate[4] = temp[0]; cdate[5] = temp[1];
        
        Aircraft a = aircraft_type_and_manufacturer_resolver(aircraft);
        aircraft_man = a.aircraft_manufacturer;
        aircraft_type = a.aircraft_type;

        Flights.push_back({ cdate, dep, arr, ldg_cnt, total_h, night_h, ifr_h, cc_h, tail, aircraft_man, aircraft_type });
    };

    n = Flights.size();

    readlog.close();

}

void print_flights(std::vector<Flight>& Flights) {
    std::cout << "Num | Flight | Dep. | Arr. |  LDG  | Total | Night |  IFR  |  C/C  |  Tail  |      Aircraft      |    Aircraft    " << std::endl;
    std::cout << "    |  date  | ICAO | ICAO | Count | hours | hours | hours | hours | number |    manufacturer    |      type      " << std::endl;
    std::cout << "----+--------+------+------+-------+-------+-------+-------+-------+--------+--------------------+----------------" << std::endl;
    for (int i = 0; i < n; i++) {

        std::string strnum;
        if (i + 1 > 10) { strnum = std::to_string(i) + "."; }
        else { strnum = "0" + std::to_string(i) + "."; };

        std::string formatted_tailnum = Flights[i].tailnumber;
        while(formatted_tailnum.length() < 6) { formatted_tailnum += " "; };

        std::string formatted_manufacturer = Flights[i].aircraft_manufacturer;
        while (formatted_manufacturer.length() < 16) { formatted_manufacturer = " " + formatted_manufacturer + " "; };
        while (formatted_manufacturer.length() < 18) { formatted_manufacturer += " "; };

        std::cout << strnum << " | " << Flights[i].date << " | " << Flights[i].departure_airport << " | " << Flights[i].arrival_airport << " |   " << Flights[i].landings_count << "   |  ";
        std::cout << std::fixed << std::setprecision(1); 
        std::cout << Flights[i].total_hours << "  |  " << Flights[i].night_hours << "  |  " << Flights[i].instrument_hours;
        std::cout << "  |  " << Flights[i].cross_country_hours << "  | " << formatted_tailnum << " | " << formatted_manufacturer << " | " << Flights[i].aircraft_type << std::endl;
    }
}

void edit_flights(std::vector<Flight> Flights) {
    int line = 1;
    print_flights(Flights);
    std::cout << "Enter line to be edited number (1-" << n << "): ";
    std::cin >> line;

    try {
        if (line <= 0 || line > n) {
            throw std::invalid_argument("Entered line number not found!");
        }
    }
    catch (std::invalid_argument const& ex) {
        std::cerr << "\nCERR: " << ex.what() << '\n';
        return;
    }

    std::cout << "Enter flight date: ";
    std::cin >> Flights[line - 1].date;

    std::cout << "Enter departure airport ICAO code: ";
    std::cin >> Flights[line - 1].departure_airport;

    std::cout << "Enter arrival airport ICAO code: ";
    std::cin >> Flights[line - 1].arrival_airport;

    std::cout << "Enter landings count: ";
    std::cin >> Flights[line - 1].landings_count;

    std::cout << "Enter total hours: ";
    std::cin >> Flights[line - 1].total_hours;

    std::cout << "Enter night hours: ";
    std::cin >> Flights[line - 1].night_hours;
        
    std::cout << "Enter instrument hours: ";
    std::cin >> Flights[line - 1].instrument_hours;

    std::cout << "Enter cross country hours: ";
    std::cin >> Flights[line - 1].cross_country_hours;

    std::cout << "Enter tailnumber: ";
    std::cin >> Flights[line - 1].tailnumber;

    std::cout << "Enter aircraft manufacturer: ";
    std::cin >> Flights[line - 1].aircraft_manufacturer;

    std::cout << "Enter aircraft type: ";
    std::cin >> Flights[line - 1].aircraft_type;

    std::cout << std::endl << "Changes saved!";
}

void calculate_stats(std::vector<Flight>& Flights) {
    return;
}

void write_to_file(std::vector<Flight>& Flights) {
    std::string temp;

    std::cout << "Enter file name for output: ";
    std::cin >> temp;

    temp = "./output/" + temp + ".flightlog";

    std::ofstream fout;

    fout.open(temp.c_str());

    for (int i = 0; i < n; i++) {
        fout << Flights[i].date << " " << Flights[i].departure_airport << " " << Flights[i].arrival_airport << " " << Flights[i].landings_count << " " << Flights[i].total_hours << " " << Flights[i].night_hours << " " << Flights[i].instrument_hours << " " << Flights[i].cross_country_hours << " " << Flights[i].tailnumber << " " << Flights[i].aircraft_manufacturer << " " << Flights[i].aircraft_type << std::endl;
    }

    fout.close();

    std::cout << "\nSave complete! (" << temp << ")\n";
}

void print_main_menu() {
    std::cout << std::endl;
    std::cout << "Select action:" << std::endl;
    std::cout << "1. Print logbook" << std::endl;
    std::cout << "2. Edit logbook" << std::endl;
    std::cout << "3. Statistics" << std::endl;
    std::cout << "8. Settings" << std::endl;
    std::cout << "9. Save and exit" << std::endl;
    std::cout << "0. Exit\n" << std::endl;
}

int choice() {
    std::cout << "> ";
    short unsigned int menuchoice = 0;
    std::cin >> menuchoice;
    return menuchoice;
}

int main() {

    std::vector<Flight> Flights;

    input_from_file(Flights);
    print_main_menu();
    short unsigned int menuchoice = choice();

    while (menuchoice != 0) {
        switch (menuchoice) {
            case 1:
                system("cls");
                std::cout << std::endl;
                print_flights(Flights);
                break;

            case 2:
                system("cls");
                std::cout << std::endl;
                edit_flights(Flights);
                break;

            case 3:
                system("cls");
                std::cout << std::endl;
                calculate_stats(Flights);
                break;

            case 8:
                system("cls");
                std::cout << std::endl;
                change_settings();
                break;

            case 9:
                system("cls");
                std::cout << std::endl;
                write_to_file(Flights);

            case 0:
                return 0;
        }
    }

    return 0;
}
