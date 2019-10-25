/*
 * Bplustree.cpp
 *
 *  Created on: 14 нояб. 2018 г.
 *      Author: ivan-arhipych
 */

#include <algorithm>
#include "Bplustree.h"

namespace Bplus_tree {
bool operator<(const Item & i1, const Item & i2) {
	return i1.key < i2.key;
}

Node::Node(const bool & is_leaf, std::vector<Item> const keys_values,
		std::vector<Node*> const children, Node* const brother, Node* const parent) {
	this->is_leaf = is_leaf;
	this->keys_values = keys_values;
	this->children = children;
	this->brother = brother;
	this->parent = parent;
}

Node::Node(const bool & is_leaf, std::vector<Item> const keys_values) {
	this->is_leaf = is_leaf;
	this->keys_values = keys_values;
	this->brother = nullptr;
	this->parent = nullptr;
}

Node::Node(const bool & is_leaf, std::vector<Item> const keys_values,
		Node* const brother, Node* const parent) {
	this->is_leaf = is_leaf;
	this->keys_values = keys_values;
	this->brother = brother;
	this->parent = parent;
}

int Node::GetNumber_of_keys() const {
	return this->keys_values.size();
}


Bplus_tree::Bplus_tree(const int & degree) {
	this->degree = degree;
	this->root = nullptr;
}

Bplus_tree::~Bplus_tree() {
	while (this->root != nullptr) {
		Node* to_delete = this->root;
		while (!(to_delete->is_leaf))
			to_delete = to_delete->children.front();
		this->Delete(to_delete->keys_values.front().key);
	}
}

Node* Bplus_tree::SearchNodeToInsert(const int & key, Node* const root) {
	if (root == nullptr)
		return nullptr;
	if (root->is_leaf == true)
			return root;
	if (key < root->keys_values.front().key)
		return SearchNodeToInsert(key, root->children.front());
	for (int i = 0; i < root->GetNumber_of_keys() - 1; i++)
		if (key >= root->keys_values[i].key && key < root->keys_values[i + 1].key)
			return SearchNodeToInsert(key, root->children[i + 1]);
	return SearchNodeToInsert(key, root->children.back());
}
// && std::move emplace
void Bplus_tree::Insert(const Item & key_value) {
	if (this->root == nullptr) {
		std::vector<Item> a = {key_value};
		this->root = new Node(true, a);
		return;
	}
	Node *node_to_insert = SearchNodeToInsert(key_value.key, this->root);
	node_to_insert->keys_values.emplace_back(key_value);
	std::sort(node_to_insert->keys_values.begin(), node_to_insert->keys_values.end());
	if (node_to_insert->GetNumber_of_keys() < this->degree)
		return;
	while (node_to_insert->GetNumber_of_keys() == this->degree) {
		//создаем два новых узла
		std::vector<Item> key_values_for_right = node_to_insert->keys_values;
		key_values_for_right.erase(key_values_for_right.begin(),
				key_values_for_right.begin() + this->degree / 2);
		std::vector<Item> key_values_for_left = node_to_insert->keys_values;
		key_values_for_left.erase(key_values_for_left.begin() + this->degree / 2,
				key_values_for_left.end());
		Node* brother_for_left = nullptr;
		Node* brother_for_right = nullptr;
		if (node_to_insert->is_leaf)
			brother_for_right = node_to_insert->brother;
		Node* new_right_node = new Node(node_to_insert->is_leaf,
				key_values_for_right, brother_for_right, node_to_insert->parent);
		if (node_to_insert->is_leaf)
			brother_for_left = new_right_node;
		Node* new_left_node = new Node(node_to_insert->is_leaf,
				key_values_for_left, brother_for_left, node_to_insert->parent);
		//делим детей
		if (!node_to_insert->children.empty()) {
			std::vector<Node*> children_for_right = node_to_insert->children;
			children_for_right.erase(children_for_right.begin(), children_for_right.begin() +
					this->degree / 2 + 1);
			for (Node* n : children_for_right)
				n->parent = new_right_node;
			std::vector<Node*> children_for_left = node_to_insert->children;
			children_for_left.erase(children_for_left.begin()  + this->degree / 2 + 1,
					children_for_left.end());
			for (Node* n : children_for_left)
				n->parent = new_left_node;
			new_right_node->children = children_for_right;
			new_left_node->children = children_for_left;
		}
		//если не было родителя, то создаем, он становится корнем
		if (node_to_insert->parent == nullptr) {
			std::vector<Item> a = {new_right_node->keys_values.front()};
			std::vector<Node*> children = {new_left_node, new_right_node};
			this->root = new Node(false, a, children, nullptr, nullptr);
			this->root->keys_values.front().value.clear();
			if (!node_to_insert->is_leaf) {
				new_right_node->keys_values.erase(new_right_node->keys_values.begin());
			}
			new_left_node->parent = this->root;
			new_right_node->parent = this->root;
			delete node_to_insert;
			return;
		}
		//разбираемся с родителем
		Node* to_delete = node_to_insert;
		node_to_insert = new_left_node->parent;
		node_to_insert->children.erase(std::remove(node_to_insert->children.begin(),
				node_to_insert->children.end(), to_delete), node_to_insert->children.end());
		node_to_insert->children.push_back(new_left_node);
		node_to_insert->children.push_back(new_right_node);
		std::sort(node_to_insert->children.begin(), node_to_insert->children.end(),
				[](const Node* i1, const Node* i2) {
			return i1->keys_values.front().key < i2->keys_values.front().key;});
		node_to_insert->keys_values.push_back(new_right_node->keys_values.front());
		if (!new_right_node->children.empty()) {
			new_right_node->keys_values.erase(std::remove(new_right_node->keys_values.begin(),
					new_right_node->keys_values.end(), new_right_node->keys_values.front()), new_right_node->keys_values.end());
		}
		node_to_insert->keys_values.back().value.clear();
		std::sort(node_to_insert->keys_values.begin(), node_to_insert->keys_values.end());
		if (node_to_insert->children.front() != new_left_node) {
			std::vector<Node*>::iterator it = std::find(node_to_insert->children.begin(),
					node_to_insert->children.end(), new_left_node) - 1;
			(*it)->brother = new_left_node;
		}
		delete to_delete;
	}
}

Node* Bplus_tree::SearchNodeToDelete(const int & key, Node* const root) {
	if (root == nullptr)
		return nullptr;
	if (root->is_leaf == true) {
		for (Item n : root->keys_values) {
			if (n.key == key)
				return root;
		}
		return nullptr;
	}
	if (key < root->keys_values.front().key)
		return SearchNodeToInsert(key, root->children.front());
	for (int i = 0; i < root->GetNumber_of_keys() - 1; i++)
		if (key >= root->keys_values[i].key && key < root->keys_values[i + 1].key)
			return SearchNodeToInsert(key, root->children[i + 1]);
	return SearchNodeToInsert(key, root->children.back());
}

void Bplus_tree::Delete(const int & key) {
	for (Item n : this->deffered_inserted__key_values) {
		if (n.key == key) {
			this->deffered_inserted__key_values.erase(std::remove(this->deffered_inserted__key_values.begin(),
					this->deffered_inserted__key_values.end(), n), this->deffered_inserted__key_values.end());
		}
	}
	Node* node_to_delete = SearchNodeToDelete(key, this->root);
	if (node_to_delete == nullptr)
		return;
	// удаляем
	for (Item n : node_to_delete->keys_values) {
		if (n.key == key) {
			node_to_delete->keys_values.erase(std::remove(node_to_delete->keys_values.begin(),
					node_to_delete->keys_values.end(), n), node_to_delete->keys_values.end());
			break;
		}
	}
	if (this->root->GetNumber_of_keys() == 0) {
		delete this->root;
		this->root = nullptr;
		return;
	}
	if (node_to_delete == this->root)
		return;
	while (true) {
		// если не меньше минимального количества или корень, то закачиваем
		if (node_to_delete->GetNumber_of_keys() >= this->degree / 2) {
			return;
		}
		std::vector<Node*>::iterator left_brother;
		std::vector<Node*>::iterator right_brother;
		int index_node_to_delete = std::distance(node_to_delete->parent->children.begin(), std::find(node_to_delete->parent->children.begin(),
				node_to_delete->parent->children.end(), node_to_delete));
		Item parent_deleted_key_value;
		//если он не самый правый ребенок
		if (!(node_to_delete->parent->children.back() == node_to_delete)) {
			parent_deleted_key_value = node_to_delete->parent->keys_values[index_node_to_delete];
			node_to_delete->parent->keys_values.erase(std::remove(node_to_delete->parent->keys_values.begin(),
						node_to_delete->parent->keys_values.end(), node_to_delete->parent->keys_values[index_node_to_delete]), node_to_delete->parent->keys_values.end());
			right_brother = std::find(node_to_delete->parent->children.begin(),
					node_to_delete->parent->children.end(), node_to_delete) + 1;
			//так работает только для листьев
			if ((*right_brother)->GetNumber_of_keys() > this->degree / 2 && (*right_brother)->is_leaf) {
				node_to_delete->keys_values.push_back((*right_brother)->keys_values.front());
				(*right_brother)->keys_values.erase(std::remove((*right_brother)->keys_values.begin(),
						(*right_brother)->keys_values.end(), (*right_brother)->keys_values.front()), (*right_brother)->keys_values.end());
				node_to_delete->parent->keys_values.insert(std::find(node_to_delete->parent->keys_values.begin(),
						node_to_delete->parent->keys_values.end(), node_to_delete->parent->keys_values[index_node_to_delete]), (*right_brother)->keys_values.front());
				node_to_delete->parent->keys_values[index_node_to_delete].value.clear();
				//про детей у node_to_delete
				if (!(node_to_delete->children.empty())) {
					node_to_delete->children.push_back((*right_brother)->children.front());
					(*right_brother)->children.erase(std::remove((*right_brother)->children.begin(),
							(*right_brother)->children.end(), (*right_brother)->children.front()), (*right_brother)->children.end());
					node_to_delete->children.back()->parent = node_to_delete;
				}
				return;
			}
			//если не лист
			else if ((*right_brother)->GetNumber_of_keys() > this->degree / 2) {
				node_to_delete->keys_values.push_back(parent_deleted_key_value);
				node_to_delete->parent->keys_values.push_back((*right_brother)->keys_values.front());
				std::sort(node_to_delete->parent->keys_values.begin(), node_to_delete->parent->keys_values.end());
				(*right_brother)->keys_values.erase(std::remove((*right_brother)->keys_values.begin(),
						(*right_brother)->keys_values.end(), (*right_brother)->keys_values.front()), (*right_brother)->keys_values.end());
				//про детей у node_to_delete
				if (!(node_to_delete->children.empty())) {
					node_to_delete->children.push_back((*right_brother)->children.front());
					(*right_brother)->children.erase(std::remove((*right_brother)->children.begin(),
							(*right_brother)->children.end(), (*right_brother)->children.front()), (*right_brother)->children.end());
					node_to_delete->children.back()->parent = node_to_delete;
				}
				// братья
				if (node_to_delete->is_leaf) {
					if (node_to_delete != node_to_delete->parent->children.front())
						node_to_delete->parent->children[index_node_to_delete - 1]->brother = (*right_brother);
					else if (node_to_delete->parent->parent != nullptr && node_to_delete->parent != node_to_delete->parent->parent->children.front()) {
						std::vector<Node*>::iterator bro = std::find(node_to_delete->parent->parent->children.begin(),
								node_to_delete->parent->parent->children.end(), node_to_delete->parent) - 1;
						(*bro)->children.back()->brother = node_to_delete;
					}
				}
				return;
			}
			//иначе объединяем братьев, корректируем родителя
			while (!node_to_delete->keys_values.empty()) {
				(*right_brother)->keys_values.insert((*right_brother)->keys_values.begin(), node_to_delete->keys_values.back());
				node_to_delete->keys_values.erase(std::remove(node_to_delete->keys_values.begin(),
						node_to_delete->keys_values.end(), node_to_delete->keys_values.back()), node_to_delete->keys_values.end());
				//про детей у node_to_delete
				if (!(node_to_delete->children.empty())) {
					(*right_brother)->children.insert((*right_brother)->children.begin(), node_to_delete->children.back());
					node_to_delete->children.erase(std::remove(node_to_delete->children.begin(),
							node_to_delete->children.end(), node_to_delete->children.back()), node_to_delete->children.end());
					(*right_brother)->children.front()->parent = (*right_brother);
				}
			}
			//про детей у node_to_delete
			if (!(node_to_delete->children.empty())) {
				(*right_brother)->children.insert((*right_brother)->children.begin(), node_to_delete->children.back());
				node_to_delete->children.erase(std::remove(node_to_delete->children.begin(),
						node_to_delete->children.end(), node_to_delete->children.back()), node_to_delete->children.end());
				(*right_brother)->children.front()->parent = (*right_brother);
				(*right_brother)->keys_values.insert((*right_brother)->keys_values.begin(), parent_deleted_key_value);
				std::sort((*right_brother)->keys_values.begin(), (*right_brother)->keys_values.end());
			}
		}
		//если самый правый
		if (node_to_delete->parent->children.back() == node_to_delete) {
			parent_deleted_key_value = node_to_delete->parent->keys_values[index_node_to_delete - 1];
			node_to_delete->parent->keys_values.erase(std::remove(node_to_delete->parent->keys_values.begin(),
						node_to_delete->parent->keys_values.end(), node_to_delete->parent->keys_values[index_node_to_delete - 1]), node_to_delete->parent->keys_values.end());
			left_brother = std::find(node_to_delete->parent->children.begin(),
					node_to_delete->parent->children.end(), node_to_delete) - 1;
			if ((*left_brother)->GetNumber_of_keys() > this->degree / 2 && (*right_brother)->is_leaf) {
				node_to_delete->keys_values.insert(node_to_delete->keys_values.begin(),
						(*left_brother)->keys_values.back());
				(*left_brother)->keys_values.erase(std::remove((*left_brother)->keys_values.begin(),
						(*left_brother)->keys_values.end(), (*left_brother)->keys_values.back()), (*left_brother)->keys_values.end());
				node_to_delete->parent->keys_values.insert(std::find(node_to_delete->parent->keys_values.begin(),
						node_to_delete->parent->keys_values.end(), node_to_delete->parent->keys_values[index_node_to_delete]), node_to_delete->keys_values.front());
				if (!(node_to_delete->children.empty())) {
					node_to_delete->children.insert(node_to_delete->children.begin(), (*left_brother)->children.back());
					(*left_brother)->children.erase(std::remove((*left_brother)->children.begin(),
							(*left_brother)->children.end(), (*left_brother)->children.back()), (*left_brother)->children.end());
					node_to_delete->children.front()->parent = node_to_delete;
				return;
				}
			}
			else if ((*left_brother)->GetNumber_of_keys() > this->degree / 2) {
				node_to_delete->keys_values.insert(node_to_delete->keys_values.begin(), parent_deleted_key_value);
				node_to_delete->parent->keys_values[index_node_to_delete] = (*left_brother)->keys_values.back();
				(*left_brother)->keys_values.erase(std::remove((*left_brother)->keys_values.begin(),
						(*left_brother)->keys_values.end(), (*left_brother)->keys_values.back()), (*left_brother)->keys_values.end());
				if (!(node_to_delete->children.empty())) {
					node_to_delete->children.insert(node_to_delete->children.begin(), (*left_brother)->children.back());
					(*left_brother)->children.erase(std::remove((*left_brother)->children.begin(),
							(*left_brother)->children.end(), (*left_brother)->children.back()), (*left_brother)->children.end());
					node_to_delete->children.front()->parent = node_to_delete;
				return;
				}
			}
			//иначе объединяем братьев, корректируем родителя
			while (!node_to_delete->keys_values.empty()) {
				(*left_brother)->keys_values.push_back(node_to_delete->keys_values.front());
				node_to_delete->keys_values.erase(std::remove(node_to_delete->keys_values.begin(),
						node_to_delete->keys_values.end(), node_to_delete->keys_values.front()), node_to_delete->keys_values.end());
				if (!(node_to_delete->children.empty())) {
					(*left_brother)->children.push_back(node_to_delete->children.front());
					node_to_delete->children.erase(std::remove(node_to_delete->children.begin(),
							node_to_delete->children.end(), node_to_delete->children.front()), node_to_delete->children.end());
					(*left_brother)->children.back()->parent = (*left_brother);
				}
			}
			if (!(node_to_delete->children.empty())) {
				(*left_brother)->children.push_back(node_to_delete->children.front());
				node_to_delete->children.erase(std::remove(node_to_delete->children.begin(),
						node_to_delete->children.end(), node_to_delete->children.front()), node_to_delete->children.end());
				(*left_brother)->children.back()->parent = (*left_brother);
				(*left_brother)->keys_values.insert((*left_brother)->keys_values.end(), parent_deleted_key_value);
				std::sort((*left_brother)->keys_values.begin(), (*left_brother)->keys_values.end());
			}
			(*left_brother)->brother = node_to_delete->brother;
		}
		//если у родителя достаточно ключей или если это непустой корень, то удалялем нужного ребенка у родителя
		if (node_to_delete->parent->GetNumber_of_keys() >= this->degree / 2 || (node_to_delete->parent == this->root && !(this->root->keys_values.empty()))) {
			node_to_delete->parent->children.erase(std::remove(node_to_delete->parent->children.begin(),
					node_to_delete->parent->children.end(), node_to_delete->parent->children[index_node_to_delete]), node_to_delete->parent->children.end());
			delete node_to_delete;
			return;
		}
		//иначе поднимаемся на уровень выше
		//в случае, если родитель - корень
		if (node_to_delete->parent == this->root) {
			if (node_to_delete->parent->children.back() != node_to_delete) {
				(*right_brother)->parent = node_to_delete->parent->parent;
				if (this->root == node_to_delete->parent)
					this->root = (*right_brother);
			}
			else {
				(*left_brother)->parent = node_to_delete->parent->parent;
				if (this->root == node_to_delete->parent)
					this->root = (*left_brother);
			}
			delete node_to_delete->parent;
			delete node_to_delete;
			return;
		}
		Node* to_delete = node_to_delete;
		node_to_delete = to_delete->parent;
		node_to_delete->children.erase(std::remove(node_to_delete->children.begin(),
							node_to_delete->children.end(), to_delete), node_to_delete->children.end());
		delete to_delete;
	}
}

Item Bplus_tree::Search(const int & key, Node* const root) const {
	if (root == nullptr) {
		std::vector<int> v;
		Item i = {-1, v};
		return i;
	}
	if (root->is_leaf == true) {
		for (Item n : this->deffered_inserted__key_values) {
			if (n.key == key) {
				for (int n1 : this->deffered_deleted__keys) {
					if (n1 == key) {
						std::vector<int> v;
						Item i = {-1, v};
						return i;
					}
				}
				return n;
			}
		}
		for (Item n : root->keys_values) {
			if (n.key == key) {
				for (int n1 : this->deffered_deleted__keys) {
					if (n1 == key) {
						std::vector<int> v;
						Item i = {-1, v};
						return i;
					}
				}
				return n;
			}
		}
		std::vector<int> v;
		Item i = {-1, v};
		return i;
	}
	if (key < root->keys_values.front().key)
		return this->Search(key, root->children.front());
	for (int i = 0; i < root->GetNumber_of_keys() - 1; i++)
		if (key >= root->keys_values[i].key && key < root->keys_values[i + 1].key)
			return this->Search(key, root->children[i + 1]);
	return this->Search(key, root->children.back());
}

void Bplus_tree::Deffer_insert(const Item & key_value) {
	this->deffered_inserted__key_values.push_back(key_value);
}

void Bplus_tree::Insert_deffered() {
	for (Item n : this->deffered_inserted__key_values) {
		this->Insert(n);
	}
	this->deffered_inserted__key_values.clear();
}

void Bplus_tree::Deffer_delete(const int & key) {
	this->deffered_deleted__keys.push_back(key);
}

void Bplus_tree::Delete_deffered() {
	for (int n : this->deffered_deleted__keys) {
		this->Delete(n);
	}
	this->deffered_deleted__keys.clear();
}
} /* namespace Bplus_tree */
