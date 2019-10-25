/*
 * Main.cpp
 *
 *  Created on: 14 нояб. 2018 г.
 *      Author: ivan-arhipych
 */
#include <iostream>
#include "Bplustree.h"

int main() {
	Bplus_tree::Bplus_tree a(3);
	std::vector<int> v1 = {1}, v2 = {2}, v3 = {3}, v4 = {4},
			v5 = {5}, v6 = {6}, v7 = {7}, v8 = {8}, v9 = {9}, v10 = {10};
	Bplus_tree::Item i1 = {1, v1};
	Bplus_tree::Item i2 = {2, v2};
	Bplus_tree::Item i3 = {3, v3};
	Bplus_tree::Item i4 = {4, v4};
	Bplus_tree::Item i5 = {5, v5};
	Bplus_tree::Item i6 = {6, v6};
	Bplus_tree::Item i7 = {7, v7};
	Bplus_tree::Item i8 = {8, v8};
	Bplus_tree::Item i9 = {9, v9};
	Bplus_tree::Item i10 = {10, v10};
	a.Insert(i3);
	a.Insert(i2);
	a.Insert(i5);
	a.Insert(i4);
	a.Insert(i1);
	a.Insert(i6);
	a.Insert(i9);
	a.Insert(i8);
	a.Insert(i7);
	std::cout << a.Search(5, a.root).key;
//	a.Delete(i10.key);
//	a.Delete(i2.key);
//	a.Delete(i3.key);
//	a.Delete(i4.key);
//	a.Delete(i5.key);
//	a.Delete(i6.key);
//	a.Delete(i7.key);
//	a.Delete(i8.key);
//	a.Delete(i9.key);
//	a.Delete(i1.key);
//	for (int i = 0; i < 10000; i++) {
//		std::vector<int> v = {i};
//		Bplus_tree::Item iv = {i, v};
//		a.Insert(iv);
//	}
//	for (int i = 9999; i >= 0; i--) {
//		a.Delete(i);
//	}
	return 0;
}


