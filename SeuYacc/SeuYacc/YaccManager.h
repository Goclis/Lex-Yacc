#ifndef _YACCMANAGER_H_
#define _YACCMANAGER_H_

#pragma once

#include <vector>
#include <string>
#include <map>
#include "Item.h"
#include "Symbol.h"
#include "Production.h"
#include "Priority.h"

using namespace std;

class YaccManager
{
public:
    YaccManager(void);
    ~YaccManager(void);
    
    // ����.y�ļ����н���
    void read_from_definition_file(char *filename);

    // ���հ�
    Item closure(Item &item);

    // goto
    Item _goto(const Item &item, const Symbol &symbol);

    // first(�����ķ�����)
    vector<Symbol> first(const Symbol &symbol);

    // first(�ķ����Ŵ�)
    vector<Symbol> first_beta_a(const vector<Symbol> &symbols);

    // ����������
    void generate_items();

    // ���ɽ�������ǰ������������
    void generate_parsing_table();

    // ���������ɽ������еĳ�ͻ
    void fix_conflict();

    void generate_code();

    // driver
    void run(char *filename);
    
    // public setter and test_run
    void set_productions(const vector<Production> &ps);
    void set_symbols(const vector<Symbol> &ss);
    void set_priorities(const map<string, Priority> &ps);
    void test_run();
private:
    vector<Production> productions; // ����ʽ����
    vector<Symbol> symbols; // �ķ����ż���
    vector<Item> items; // ���
    vector<map<string, int>> goto_table; // goto��
    vector<map<string, string>> action; // action��
    map<string, Priority> priorities; // ���ȼ���ϵ
    map<string, vector<Symbol>> first_set; // ����first���ռ任ʱ��
	streambuf *log_file; // ��־�ļ�
    
    /* Helper Functions */

    // �ж�ĳ���ķ������Ƿ���ĳ�ķ����ż�����
    bool is_symbol_in_first_set(const vector<Symbol> &s, const Symbol &i);

    // �ϲ������ķ����ż��ϣ���������
    void merge_two_first_set(vector<Symbol> &s1, const vector<Symbol> &s2);

    // ȥ��ĳ�ķ����ż����е�epsilon����
    vector<Symbol> remove_epsilon(const vector<Symbol> &s);

    // �ж�ĳ��ItemLine�Ƿ���ĳ��Item��
    bool is_item_line_in_item(const ItemLine &il, const vector<ItemLine> &vil);

    // �ж�ĳ��Item�Ƿ��Ѵ���(��items�У�
    Item is_item_exist(const Item &i);

    // �ж�ĳ��Symbol�Ƿ��Ѵ��ڣ���symbols�У�
    bool is_symbol_exsit(const Symbol &);

    // string + int
    string string_concat_int(const string &head, int i);

    // string.split(delim)
    vector<string> string_split(const string &s, char delim);

    // ��YaccFrontת����YaccManger
    void convert_from_front_to_manager(char *);

	// ��ӡItemLine
	void print_item_line(const ItemLine &il);

	// ��ӡItem
	void print_item(const Item &item);
};

#endif // _YACCMANAGER_H_
