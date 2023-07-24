#include "TableModelColumns.h"

TableModelColumns::TableModelColumns()
{
    add(m_col_target);
    add(m_col_type);
    add(m_col_action);

    add(m_col_interface);
    add(m_col_ip);
    add(m_col_protocol);
    add(m_col_port);
    add(m_col_comment);
}



