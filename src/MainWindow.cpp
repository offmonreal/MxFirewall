#include "MainWindow.h"
#include <iostream>

#include "Utilites.h"
#include "NftablesParser.h"

MainWindow::MainWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade) : Gtk::Window(cobject), builder(refGlade)
{
    //На верь экран на телефоне
#ifdef POSTMARKETOS
    this->maximize();
#endif

    builder->get_widget("ServiceStatus", pSwitch);
    builder->get_widget("button1", b_home);
    builder->get_widget("button2", b_rules);
    builder->get_widget("button3", b_status);
    builder->get_widget("button4", b_log);
    builder->get_widget("Notebook1", pNotebook);
    builder->get_widget("TreeView1", pTreeView);

    builder->get_widget("ComboBoxIncomming", pComboBoxIncomming);
    builder->get_widget("ComboBoxOutgoing", pComboBoxOutgoing);
    builder->get_widget("ComboBoxForward", pComboBoxForward);




    if (b_home) {
        b_home->set_image_from_icon_name("go-home-symbolic", Gtk::ICON_SIZE_DND, true);
        b_home->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_home));
    }

    if (b_rules) {
        b_rules->set_image_from_icon_name("network-wired-no-route-symbolic", Gtk::ICON_SIZE_DND, true);
        b_rules->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_rules));

    }

    if (b_status) {
        b_status->set_image_from_icon_name("view-list-symbolic", Gtk::ICON_SIZE_DND, true);
        b_status->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_status));

    }
    if (b_log) {
        b_log->set_image_from_icon_name("text-x-generic-symbolic", Gtk::ICON_SIZE_DND, true);
        b_log->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_clicked_log));

    }



    bool service_is_work = Utilites::SrvNftablesIsLife();
    //Status service nftables
    if (pSwitch) {
        pSwitch->set_active(service_is_work);
        pSwitch->signal_state_set().connect(sigc::mem_fun(*this, &MainWindow::on_switch_state_set), false);

    }



    if (pTreeView) {
        // Создание модели данных.
        m_refTreeModel = Gtk::TreeStore::create(m_Columns);
        pTreeView->set_model(m_refTreeModel);

        // Добавление столбцов в представление.
        pTreeView->append_column("Target", m_Columns.m_col_target);
        pTreeView->append_column("Type", m_Columns.m_col_type);
        pTreeView->append_column("Action", m_Columns.m_col_action);
        pTreeView->append_column("Interface", m_Columns.m_col_interface);
        pTreeView->append_column("Ip address", m_Columns.m_col_ip);
        pTreeView->append_column("Protocol", m_Columns.m_col_protocol);
        pTreeView->append_column("Port", m_Columns.m_col_port);
        pTreeView->append_column("Comment", m_Columns.m_col_comment);


    }


    //Utilites::test();
    if (service_is_work)
        UpdateTreeViewRules();
}

void MainWindow::UpdateTreeViewRules()
{

    NftablesParser parser;
    std::string script = Utilites::ExecToStr("pkexec nft list ruleset");
    auto Tables = parser.ParseScriptFromString(script);


    if (pTreeView) {
        //Очищаем значения но не мобель
        if (auto refTreeModel = Glib::RefPtr<Gtk::TreeStore>::cast_dynamic(pTreeView->get_model()))
            refTreeModel->clear();


        for (auto t : Tables) {
            // Заполнение дерева данными.
            auto table = *(m_refTreeModel->append());
            table[m_Columns.m_col_target] = "Table";
            table[m_Columns.m_col_type] = t.name; //inet
            table[m_Columns.m_col_action] = t.type; //filter

            for (auto c : t.chains) {
                auto chain = *(m_refTreeModel->append(table.children()));
                chain[m_Columns.m_col_target] = "Chain";
                chain[m_Columns.m_col_type] = c.name; //input
                chain[m_Columns.m_col_action] = c.policy; //drop

                for (auto r : c.rules) {
                    auto rules = *(m_refTreeModel->append(chain.children()));
                    rules[m_Columns.m_col_target] = "";
                    rules[m_Columns.m_col_type] = "Rule";
                    rules[m_Columns.m_col_action] = r.action; //accept
                    rules[m_Columns.m_col_protocol] = r.protocol;
                    rules[m_Columns.m_col_port] = r.port;
                    rules[m_Columns.m_col_interface] = r.interface;
                    rules[m_Columns.m_col_comment] = r.comment;

                }
            }
        }
    }

    if (pComboBoxIncomming && pComboBoxOutgoing && pComboBoxForward)
        for (auto t : Tables) {
            if (t.name == "inet") {
                for (auto c : t.chains) {
                    if (c.name == "input") {
                        if (c.policy == "accept") {
                            pComboBoxIncomming->set_active(0);
                        }
                        else if (c.policy == "drop") {
                            pComboBoxIncomming->set_active(1);
                        }
                        else {
                            pComboBoxIncomming->set_active(2);
                        }

                    }
                    else if (c.name == "output") {
                        if (c.policy == "accept") {
                            pComboBoxOutgoing->set_active(0);
                        }
                        else if (c.policy == "drop") {
                            pComboBoxOutgoing->set_active(1);
                        }
                        else {
                            pComboBoxOutgoing->set_active(2);
                        }

                    }
                    else if (c.name == "forward") {
                        if (c.policy == "accept") {
                            pComboBoxForward->set_active(0);
                        }
                        else if (c.policy == "drop") {
                            pComboBoxForward->set_active(1);
                        }
                        else {
                            pComboBoxForward->set_active(2);
                        }

                    }

                }
            }
        }
}

void MainWindow::on_button_clicked_home()
{
    if (pNotebook)
        pNotebook->set_current_page(0);
    //std::cout << "Hello, World!" << std::endl;
}

void MainWindow::on_button_clicked_rules()
{
    if (pNotebook)
        pNotebook->set_current_page(1);

}

void MainWindow::on_button_clicked_status()
{
    if (pNotebook)
        pNotebook->set_current_page(2);

}

void MainWindow::on_button_clicked_log()
{
    if (pNotebook)
        pNotebook->set_current_page(3);

}

bool MainWindow::on_switch_state_set(bool state)
{
    //Возникает только когда не получилось включить или выключить службу
    if (Utilites::SrvNftablesIsLife() == state)
        state = !state;


    if (state) {
        std::cout << "Switch turned on." << std::endl;
    }
    else {
        std::cout << "Switch turned off." << std::endl;
    }

    //Переключение состояния службы
    Utilites::SetSrvNftablesState(state);

    //Если все получилось
    if (Utilites::SrvNftablesIsLife() == state) {
      
        //Обновляем если соужбу включали
        if (state)
            UpdateTreeViewRules();
        
        return false;
    }


    //Ошибка аунтификации
    return true; // Верните false, если вы хотите, чтобы переключатель обновил свое состояние автоматически

}

MainWindow::~MainWindow()
{

}