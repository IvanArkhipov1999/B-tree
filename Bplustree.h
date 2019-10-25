/*
 * Bplustree.h
 *
 *  Created on: 14 нояб. 2018 г.
 *      Author: ivan-arhipych
 */

#ifndef BPLUSTREE_H_
#define BPLUSTREE_H_

#include <vector>

namespace Bplus_tree {

struct Item {
	int key;
	std::vector<int> value;
	bool operator==(const Item & i){
		return key == i.key && value == i.value;
	}
};

class Node {
private:
	bool is_leaf;
	std::vector<Item> keys_values;
	std::vector<Node*> children;
	Node *brother;
	Node *parent;
public:
	friend class Bplus_tree;
	Node(const bool & is_leaf, std::vector<Item> const keys_values,
			std::vector<Node*> const children, Node* const brother, Node* const parent);
	Node(const bool & is_leaf, std::vector<Item> const keys_values);
	Node(const bool & is_leaf, std::vector<Item> const keys_values,
			Node* const brother, Node* const parent);
	~Node(){};
	int GetNumber_of_keys() const;
};

class Bplus_tree {
private:
	int degree;
	std::vector<Item> deffered_inserted__key_values;
	std::vector<int> deffered_deleted__keys;
	static Node* SearchNodeToInsert(const int & key, Node* const root);
	static Node* SearchNodeToDelete(const int & key, Node* const root);
public:
	Node *root;
	Bplus_tree(const int & degree);
	virtual ~Bplus_tree();
	void Insert(const Item & key_value);
	void Delete(const int & key);
	Item Search(const int & key, Node* const root) const;
	void Deffer_insert(const Item & key_value);
	void Insert_deffered();
	void Deffer_delete(const int & key);
	void Delete_deffered();
};

} /* namespace Bplus_tree */

#endif /* BPLUSTREE_H_ */
