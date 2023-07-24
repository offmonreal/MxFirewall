#include <iostream>
#include <gtkmm.h>
#include "MainWindow.h"
#include <getopt.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{

/*
    if (getuid() != 0) { // Если мы не root
#ifdef POSTMARKETOS

        execlp("pkexec", "pkexec", argv[0], (char *) NULL); // Запускаем самого себя с pkexec
#else
        execlp("pkexec", "pkexec", argv[0], (char *) NULL); // Запускаем самого себя с sudo pkexec глючит не видит дисплея..
#endif
        
        std::cout << "Start as root = FALSE;" << std::endl;

        return 1;
    }

            std::cout << "Start as root = " << getuid() << std::endl;
    */    

    bool use_gui = true;
    int opt;
    while ((opt = getopt(argc, argv, "c")) != -1) {
        switch (opt) {
        case 'c':
            use_gui = false;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-c]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (use_gui) {
        auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

        // Load the GtkBuilder file and instantiate its widgets
        auto refBuilder = Gtk::Builder::create_from_file("interface.ui");

        MainWindow* pMainWindow = nullptr;
        refBuilder->get_widget_derived("MainWindow", pMainWindow);

        if (pMainWindow) {
            app->run(*pMainWindow);
            delete pMainWindow;
        }
    }
    else {
        // Ваш код для работы в режиме консоли здесь
        std::cout << "Console mod: Active;" << std::endl;
    }

    return 0;
}
