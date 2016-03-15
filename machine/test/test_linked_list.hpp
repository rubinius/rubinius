#include "linkedlist.hpp"

class TestLinkedList : public CxxTest::TestSuite {
public:

  class MyNode : public LinkedList::Node {

  };

  void test_create() {
    LinkedList ll;
    TS_ASSERT_EQUALS(ll.size(), 0U);
  }

  void test_add() {
    LinkedList ll;
    MyNode node;

    ll.add(&node);

    TS_ASSERT(!node.next());
    TS_ASSERT(!node.prev());

    TS_ASSERT_EQUALS(ll.size(), 1U);

    MyNode node2;

    ll.add(&node2);

    TS_ASSERT_EQUALS(node.prev(), &node2);
    TS_ASSERT(!node.next());

    TS_ASSERT_EQUALS(node2.next(), &node);
    TS_ASSERT(!node2.prev());

    TS_ASSERT_EQUALS(ll.size(), 2U);
  }

  void test_remove() {
    LinkedList ll;
    MyNode node;
    MyNode node2;

    ll.add(&node);
    ll.add(&node2);

    TS_ASSERT_EQUALS(ll.size(), 2U);

    ll.remove(&node);
    TS_ASSERT(!node.next());
    TS_ASSERT(!node.prev());

    TS_ASSERT_EQUALS(ll.size(), 1U);

    TS_ASSERT_EQUALS(ll.head(), &node2);
    TS_ASSERT(!node2.next());
    TS_ASSERT(!node2.prev());
  }

  void test_add_remove_3() {
    LinkedList ll;
    MyNode node;
    MyNode node2;
    MyNode node3;

    ll.add(&node);
    ll.add(&node2);
    ll.add(&node3);

    TS_ASSERT_EQUALS(&node2, node3.next());
    TS_ASSERT_EQUALS(&node3, node2.prev());

    TS_ASSERT_EQUALS(&node,  node2.next());
    TS_ASSERT_EQUALS(&node2, node.prev());

    TS_ASSERT(!node.next());
    TS_ASSERT(!node3.prev());

    ll.remove(&node2);

    TS_ASSERT_EQUALS(&node, node3.next());
    TS_ASSERT_EQUALS(&node3, node.prev());

  }
};
