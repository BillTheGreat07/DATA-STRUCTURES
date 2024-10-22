# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.


# Test της γενικής υλοποίησης, χρησιμοποιώντας Set βασισμένο σε BST
#
UsingADTSet_BST_set_utils_test_OBJS = \
	set_utils_test.o \
	$(MODULES)/UsingADTSet/set_utils.o \
	$(MODULES)/UsingBinarySearchTree/ADTSet.o \
	$(MODULES)/UsingDynamicArray/ADTVector.o


# Test της γενικής υλοποίησης, χρησιμοποιώντας Set βασισμένο σε BTree
#
UsingADTSet_BTree_set_utils_test_OBJS = \
	set_utils_test.o \
	$(MODULES)/UsingADTSet/set_utils.o \
	$(MODULES)/UsingBTree/ADTSet.o \
	$(MODULES)/UsingDynamicArray/ADTVector.o


# Test της ειδική υλοποίηση του set_utils για BST
#
UsingBinarySearchTree_set_utils_test_OBJS = \
	set_utils_test.o \
	$(MODULES)/UsingBinarySearchTree/set_utils.o \
	$(MODULES)/UsingBinarySearchTree/ADTSet.o \
	$(MODULES)/UsingDynamicArray/ADTVector.o


# Ο βασικός κορμός του Makefile
include ../common.mk